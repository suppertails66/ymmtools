#ifndef YMM_ABLK_OP_H
#define YMM_ABLK_OP_H


#include "util/TStream.h"
#include "ymm/YmmAblkOpParam.h"
#include "ymm/YmmAblkLexStream.h"
#include "ymm/YmmAblkEnv.h"
#include <vector>
#include <iostream>

namespace Md {


class YmmAblkOp {
public:
  YmmAblkOp(int id__, const char* name__,
            const char* paramString__);
  
  int id;
  const char* name;
  const char* paramString;
  
  void readParams(BlackT::TStream& ifs);
  void readParams(YmmAblkLexStream& src,
                  YmmAblkEnv& env);
  void writeParams(BlackT::TStream& ofs) const;
  void write(BlackT::TStream& ofs) const;
  void printParams(std::ostream& ofs) const;
protected:
  std::vector<YmmAblkOpParam> params_;
  
  YmmAblkOpParam readParam(YmmAblkLexStream& src,
                 YmmAblkEnv& env);
};


}


#endif
