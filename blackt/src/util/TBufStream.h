#ifndef TBUFSTREAM_H
#define TBUFSTREAM_H


#include "util/TStream.h"
#include "util/TArray.h"
#include <string>

namespace BlackT {


class TBufStream : public TStream {
public:
  TBufStream(int sz);
  virtual ~TBufStream();
  
  virtual void open(const char* filename);
  virtual void save(const char* filename);
  
  virtual char get();
  virtual void unget();
  virtual char peek();
  virtual void put(char c);
  virtual void read(char* dst, int size);
  virtual void write(const char* src, int size);
  virtual bool good() const;
  virtual bool bad() const;
  virtual bool fail() const;
  virtual bool eof() const;
  virtual void clear();
  virtual int tell();
  virtual void seek(int pos);
  virtual int size();
  
  virtual void alignToBoundary(int byteBoundary);
  
  virtual int capacity() const;
  virtual void writeFrom(TStream& ifs, int sz);
  virtual void writeTo(TStream& ofs, int sz);
  virtual void fromString(const std::string& str);
  
  TArray<char>& data();
  const TArray<char>& data() const;
  
protected:
  virtual void updateEndPos();

  TArray<char> data_;
  int pos_;
  int endPos_;
};


}


#endif
