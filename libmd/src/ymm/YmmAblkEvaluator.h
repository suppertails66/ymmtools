#ifndef YMM_ABLK_EVALUATOR_H
#define YMM_ABLK_EVALUATOR_H


#include "ymm/YmmAblkLexStream.h"
#include "ymm/YmmAblkEnv.h"
#include "util/TStream.h"
#include <map>

namespace Md {


class YmmAblkEvaluator {
public:
  YmmAblkEvaluator(YmmAblkLexStream& src__,
                   BlackT::TStream& dst__);
  
  void evaluate();
protected:
  YmmAblkLexStream* src_;
  BlackT::TStream* dst_;
  YmmAblkEnv env_;
  
  void evaluateStatement();
};


}


#endif
