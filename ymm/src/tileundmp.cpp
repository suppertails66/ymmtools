#include "md/MdPattern.h"
#include "md/MdPaletteLine.h"
#include "util/TStringConversion.h"
#include "util/TOpt.h"
#include "util/TFileManip.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TGraphic.h"
#include "util/TPngConversion.h"

using namespace std;
using namespace BlackT;
using namespace Md;

const static int defaultTilesPerRow = 16;

int main(int argc, char* argv[]) {
  if (argc < 4) {
    cout << "Mega Drive pattern data undumper" << endl;
    cout << "Usage: " << argv[0] << " <infile> <numtiles> <outfile>"
      " [options]" << endl;
    cout << "Options: " << endl;
    cout << "  -p   Specify input palette line (default: grayscale)" << endl;
    cout << "  -r   Specify tiles per row (default: 16)" << endl;
    
    return 0;
  }
  
  MdPaletteLine palette;
  bool hasPalette = false;
  int tilesPerRow = defaultTilesPerRow;
  
  char* infileName = argv[1];
  int numTiles = TStringConversion::stringToInt(string(argv[2]));
  char* outfileName = argv[3];
  
  char* paletteFileName = TOpt::getOpt(argc, argv, "-p");
  if (paletteFileName != NULL) {
    hasPalette = true;
    
    TArray<TByte> palFile;
    TFileManip::readEntireFile(palFile, paletteFileName);
    palette = MdPaletteLine(palFile.data());
  }
  
  TOpt::readNumericOpt(argc, argv, "-r", &tilesPerRow);
  
  TGraphic src;
  TPngConversion::RGBAPngToGraphic(string(infileName), src);
  
  TOfstream ofs(outfileName, ios_base::binary);
  
  for (int i = 0; i < numTiles; i++) {
    int x = (i % tilesPerRow) * MdPattern::w;
    int y = (i / tilesPerRow) * MdPattern::h;
    
    MdPattern pattern;
    
    if (hasPalette) {
      pattern.fromColorGraphic(src, palette, x, y);
    }
    else {
      pattern.fromGrayscaleGraphic(src, x, y);
    }
  
    char buffer[MdPattern::uncompressedSize];
    pattern.write(buffer);
    ofs.write(buffer, sizeof(buffer));
  }
  
  return 0;
}
