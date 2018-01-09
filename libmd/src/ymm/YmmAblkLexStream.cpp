#include "ymm/YmmAblkLexStream.h"
#include "exception/TGenericException.h"

using namespace BlackT;

namespace Md {


YmmAblkLexStream::YmmAblkLexStream()
  : pos_(0) { }

YmmAblkLexeme YmmAblkLexStream::get() {
  if (pos_ < 0) {
    throw TGenericException(T_SRCANDLINE,
                            "YmmAblkLexStream::get()",
                            "Accessed before start of stream");
  }
  else if (pos_ >= lexemes_.size()) {
    throw TGenericException(T_SRCANDLINE,
                            "YmmAblkLexStream::get()",
                            "Accessed beyond end of stream");
  }
  
  return lexemes_[pos_++];
}

void YmmAblkLexStream::unget() {
  --pos_;
}

YmmAblkLexeme YmmAblkLexStream::peek() {
  if (pos_ < 0) {
    throw TGenericException(T_SRCANDLINE,
                            "YmmAblkLexStream::peek()",
                            "Accessed before start of stream");
  }
  else if (pos_ >= lexemes_.size()) {
    throw TGenericException(T_SRCANDLINE,
                            "YmmAblkLexStream::peek()",
                            "Accessed beyond end of stream");
  }
  
  return lexemes_[pos_];
}

bool YmmAblkLexStream::eof() const {
  return (pos_ >= lexemes_.size());
}

void YmmAblkLexStream::reset() {
  pos_ = 0;
}

void YmmAblkLexStream::append(YmmAblkLexeme lexeme) {
  lexemes_.push_back(lexeme);
}


}
