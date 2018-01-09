#ifndef YMM_ABLK_DECOMPILER_H
#define YMM_ABLK_DECOMPILER_H


#include "util/TStream.h"
#include <string>
#include <iostream>

namespace Md {


namespace YmmEngineVersions {
  enum YmmEngineVersion {
    mcd,
    sat
  };
}

class YmmAblkDecompiler {
public:
  YmmAblkDecompiler(const std::string& outprefix__,
                    YmmEngineVersions::YmmEngineVersion version__);
  
  void decompile(BlackT::TStream& ifs,
                 std::ostream& ofs);
protected:

  std::string outprefix_;
  YmmEngineVersions::YmmEngineVersion version_;
  
  int indentLevel_;
  
};


}


#endif
