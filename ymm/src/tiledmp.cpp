#include "md/MdPattern.h"
#include "md/MdPaletteLine.h"
#include "util/TStringConversion.h"
#include "util/TOpt.h"
#include "util/TFileManip.h"
#include "util/TIfstream.h"
#include "util/TGraphic.h"
#include "util/TPngConversion.h"

using namespace std;
using namespace BlackT;
using namespace Md;

const static int defaultTilesPerRow = 16;

int main(int argc, char* argv[]) {
  if (argc < 5) {
    cout << "Mega Drive pattern data dumper" << endl;
    cout << "Usage: " << argv[0] << " <infile> <offset> <numtiles> <outfile>"
      " [options]" << endl;
    cout << "Options: " << endl;
    cout << "  -p   Input palette line (default: grayscale)" << endl;
    cout << "  -r   Number of tiles per output row (default: 16)" << endl;
    
    return 0;
  }
  
  MdPaletteLine palette;
  bool hasPalette = false;
  int tilesPerRow = defaultTilesPerRow;
  
  char* infileName = argv[1];
  int offset = TStringConversion::stringToInt(string(argv[2]));
  int numTiles = TStringConversion::stringToInt(string(argv[3]));
  char* outfileName = argv[4];
  
  char* paletteFileName = TOpt::getOpt(argc, argv, "-p");
  if (paletteFileName != NULL) {
    hasPalette = true;
    
    TArray<TByte> palFile;
    TFileManip::readEntireFile(palFile, paletteFileName);
    palette = MdPaletteLine(palFile.data());
  }
  
  TOpt::readNumericOpt(argc, argv, "-r", &tilesPerRow);
  
  TIfstream ifs(infileName, ios_base::binary);
  ifs.seek(offset);
  
  TGraphic dst;
  
  int dstW = tilesPerRow * MdPattern::w;
  
  int dstH = numTiles / tilesPerRow;
  if (numTiles % tilesPerRow != 0) ++dstH;
  dstH *= MdPattern::h;
  
  dst.resize(dstW, dstH);
  dst.clearTransparent();
  
  for (int i = 0; i < numTiles; i++) {
    int x = (i % tilesPerRow) * MdPattern::w;
    int y = (i / tilesPerRow) * MdPattern::h;
  
    char buffer[MdPattern::uncompressedSize];
    ifs.read(buffer, sizeof(buffer));
    
    MdPattern pattern;
    pattern.read(buffer);
    
    if (hasPalette) {
      pattern.toColorGraphic(dst, palette, x, y);
    }
    else {
      pattern.toGrayscaleGraphic(dst, x, y);
    }
  }
  
  TPngConversion::graphicToRGBAPng(string(outfileName), dst);
  
  return 0;
}
