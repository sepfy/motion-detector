#include "detector_factory.h"
#include "detector_xnnc.h"
#include "detector_tvm.h"

Detector* DetectorFactory::Create() {
  DetectorTVM *detector = new DetectorTVM();
  return detector;
}

