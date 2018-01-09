#include "util/TIfstream.h"
#include "util/TBufStream.h"
#include <iostream>

using namespace std;
using namespace BlackT;

const static int targetSize = 0x100000;

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "Battle Golfer Yui ROM prep tool" << endl;
    cout << "Usage: " << argv[0] << " <infile> <outfile>" << endl;
    
    return 0;
  }
  
  TBufStream rom(targetSize);
  {
    TIfstream ifs(argv[1], ios_base::binary);
    rom.writeFrom(ifs, ifs.size());
  }
  int romend = rom.tell();
  
  // Pad to 1MB
  int padsize = targetSize - romend;
  rom.seek(romend);
  rom.seekoff(padsize);
  int romsize = rom.tell();
  
  // Recompute checksum
  rom.seek(0x200);
  unsigned short int sum = 0;
  while (rom.tell() < romsize) {
    sum += (unsigned short int)(rom.readu16be());
//    cout << hex << sum << endl;
//    char c; cin >> c;
  }
  rom.seek(0x18E);
  rom.writeu16be(sum);
  
  // update rom end field
  rom.seek(0x1A4);
  rom.writeu32be(targetSize - 1);
  
  rom.seek(romsize);
  rom.save(argv[2]);
  
  return 0;
}
