#include "ymm/YmmScene.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TBufStream.h"
#include <string>
#include <iostream>

using namespace std;
using namespace BlackT;
using namespace Md;

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "Yumimi Mix scene builder" << endl;
    cout << "Usage: " << argv[0] << " <inprefix> <outfile>" << endl;
    
    return 0;
  }
  
  char* inprefix = argv[1];
  char* outfile = argv[2];
  
  YmmScene scene;
  scene.load(inprefix);
  
  TOfstream ofs(outfile, ios_base::binary);
  scene.write(ofs);
  
  return 0;
}
