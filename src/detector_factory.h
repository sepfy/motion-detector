#ifndef DETECTOR_FACTORY_H_
#define DETECTOR_FACTORY_H_

#include "detector.h"

class DetectorFactory {
 public:
  static Detector* Create();
};

#endif  // DETECTOR_FACTORY_H_
