#include "ymm/YmmAblkEnv.h"

namespace Md {


YmmAblkEnv::YmmAblkEnv() { }

void YmmAblkEnv::addLabel(std::string name, int pos) {
  labels[name] = pos;
}

void YmmAblkEnv::addLabelTarget(std::string name, int width) {
  LabelTarget target { name, 0, width };
  labelTargets.push_back(target);
}


}
