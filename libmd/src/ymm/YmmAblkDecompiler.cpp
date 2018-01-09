#include "ymm/YmmAblkDecompiler.h"
#include "ymm/YmmAblkOps.h"
#include "ymm/YmmAblkOpNotFoundExp.h"
#include <iomanip>
#include <cstring>

namespace Md {


YmmAblkDecompiler::YmmAblkDecompiler(
    const std::string& outprefix__,
    YmmEngineVersions::YmmEngineVersion version__)
  : outprefix_(outprefix__),
    version_(version__),
    indentLevel_(0) { }
  
void YmmAblkDecompiler::decompile(BlackT::TStream& ifs,
                                  std::ostream& ofs) {
//  for (int i = 0; i < numYmmAblkOps; i++) {
//    std::cout << std::hex << ymmAblkOps[i].id << std::endl;
//  }

  // Fetch next op byte
  char next = ifs.get();
  
  // 00 = script terminator
  // TODO: what if more than one of these is allowed and can be conditionally
  // branched over?
  while (next != 0x00) {
    try {
      int pos = ifs.tell() - 1;
      YmmAblkOp op = makeYmmAblkOp((int)next);
      op.readParams(ifs);
      ofs << "loc_" << std::setw(8) << std::setfill('0') << std::hex
        << pos << ": " << op.name;
      int total = 8 + 2 + std::strlen(op.name);
      while (total < 40) { ofs << " "; ++total; }
      ofs << "(";
      op.printParams(ofs);
      ofs << ")";
      ofs << std::endl;
    }
    catch (YmmAblkOpNotFoundExp& e) {
      std::cerr << "Error: unknown op 0x" << std::hex << std::setw(2)
        << std::setfill('0')
        << (int)(unsigned char)next
        << " at 0x" << std::hex << ifs.tell() - 1 << std::endl;
      return;
    }
    
    next = ifs.get();
  }
  
  
  
  
}


}
