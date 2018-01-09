#include "ymm/YmmAblkEvaluator.h"
#include "ymm/YmmAblkOps.h"
#include "util/TStringConversion.h"
#include "exception/TGenericException.h"
#include <cstring>
#include <string>

using namespace BlackT;

namespace Md {


YmmAblkEvaluator::YmmAblkEvaluator(YmmAblkLexStream& src__,
                 BlackT::TStream& dst__)
  : src_(&src__),
    dst_(&dst__) { }

void YmmAblkEvaluator::evaluate() {
  while (!src_->eof()) {
    evaluateStatement();
  }
  dst_->writeu8be(0x00);
}
  
void YmmAblkEvaluator::evaluateStatement() {
  // Check whether this is a dot directive
  bool isDot = false;
  if (src_->peek().type == "DOT") {
    isDot = true;
    src_->get();
  }

  // Get name
  int lineNum = src_->peek().lineNum;
  std::string name = src_->get().stringVal;
  
  // If followed by a colon, this is a label
  if (src_->peek().type == ":") {
    src_->get();
    // make a note of current script putpos
    // ...
    return;
  }
  
  if (isDot) {
    // ...
    return;
  }
  
  // find matching function in op list
  for (int i = 0; i < numYmmAblkOps; i++) {
    if (strcmp(name.c_str(), ymmAblkOps[i].name) == 0) {
//      std::cout << i << std::endl;
      
      // match open paren
      if (src_->get().type.compare("(") != 0) {
        throw TGenericException(T_SRCANDLINE,
                                "YmmAblkEvaluator::evaluateStatement()",
                                "Error:\n  Line "
                                  + TStringConversion::intToString(lineNum)
                                  + ": no open parenthesis for arguments");
      }
      
      YmmAblkOp op = makeYmmAblkOp(i);
      op.readParams(*src_, env_);
      
      // match close paren
      if (src_->get().type.compare(")") != 0) {
        throw TGenericException(T_SRCANDLINE,
                                "YmmAblkEvaluator::evaluateStatement()",
                                "Error:\n  Line "
                                  + TStringConversion::intToString(lineNum)
                                  + ": no close parenthesis for arguments");
      } 
      
//      op.printParams(std::cerr);
//      std::cerr << std::endl;
      op.write(*dst_);
      
      return;
    }
  }
  
  throw TGenericException(T_SRCANDLINE,
                          "YmmAblkEvaluator::evaluateStatement()",
                          "Error:\n  Line "
                            + TStringConversion::intToString(lineNum)
                            + ": unknown function "
                            + name);
}


}
