#include "detector_tvm.h"

DetectorTVM::DetectorTVM() {

}

DetectorTVM::~DetectorTVM() {

}

int DetectorTVM::LoadModel(char *modelname) {

  std::string modelfile(modelname);
  tvm::runtime::Module mod_dylib =
  tvm::runtime::Module::LoadFromFile(modelfile + ".so");

  std::ifstream json_in(modelfile + ".json", std::ios::in);
  std::string json_data((std::istreambuf_iterator<char>(json_in)), std::istreambuf_iterator<char>());
  json_in.close();

  std::ifstream params_in(modelfile + ".params", std::ios::binary);
  std::string params_data((std::istreambuf_iterator<char>(params_in)), std::istreambuf_iterator<char>());
  params_in.close();

  TVMByteArray params_arr;
  params_arr.data = params_data.c_str();
  params_arr.size = params_data.length();

  int dtype_code = kDLFloat;
  int dtype_bits = 32;
  int dtype_lanes = 1;
  int device_type = kDLCPU;
  int device_id = 0;

  mod_ = (*tvm::runtime::Registry::Get("tvm.graph_runtime.create"))
        (json_data, mod_dylib, device_type, device_id);

  tvm::runtime::PackedFunc load_params = mod_.GetFunction("load_params");
  load_params(params_arr);

  // Input
  int in_ndim = 4;
  int64_t in_shape[4] = {1, 3, 224, 224};
  TVMArrayAlloc(in_shape, in_ndim, dtype_code, dtype_bits, dtype_lanes, device_type, device_id, &x_);
  set_input_ = mod_.GetFunction("set_input");

  // Output
  int out_ndim = 2;
  int64_t out_shape[2] = {1, 1000};
  TVMArrayAlloc(out_shape, out_ndim, dtype_code, dtype_bits, dtype_lanes, device_type, device_id, &y_);

  run_ = mod_.GetFunction("run");
  get_output_ = mod_.GetFunction("get_output");
  std::cout << "Load model successfully" << std::endl;

}

void DetectorTVM::HWCToCHW(float *data, float *input) {
  unsigned int im_size = 224*224;
  for (unsigned j = 0; j < im_size; ++j) {
    data[0*im_size + j] = (input[j * 3 + 0]/255.0 - 0.485)/0.229;
    data[1*im_size + j] = (input[j * 3 + 1]/255.0 - 0.456)/0.224;
    data[2*im_size + j] = (input[j * 3 + 2]/255.0 - 0.406)/0.225;
  }

}

void DetectorTVM::Detect() {

  HWCToCHW((float*)x_->data, input_);
  set_input_("input", x_);
  run_();
  get_output_(0, y_);
  output_ = (float*)(y_->data);
  //for(int i = 0; i < 10; i++)
  //  std::cout << output_[0] << std::endl;
}


