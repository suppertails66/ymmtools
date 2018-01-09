#ifndef YMM_SUBSCENE_H
#define YMM_SUBSCENE_H


#include "ymm/YmmPcmList.h"
#include "ymm/YmmTilemap.h"
#include "md/MdPattern.h"
#include "md/MdTilemap.h"
#include "md/MdPalette.h"
#include "util/TArray.h"
#include "util/TByte.h"
#include "util/TStream.h"
#include <string>
#include <vector>

namespace Md {


class YmmSubscene {
public:
  YmmSubscene();
  
  void read(BlackT::TStream& ifs, int baseOffset,
            const YmmPcmList& pcmList);
  void write(BlackT::TStream& ofs,
            YmmPcmList& pcmList) const;
  
  void save(const char* prefix) const;
  void load(const char* prefix);
protected:
  const static int bgMapTileWidth_ = 80;
  const static int bgMapTileHeight_ = 64;
  const static int bytesPerBgMap_ = bgMapTileWidth_ * bgMapTileHeight_
    * MdTileId::size;

  const static char* infoFileName_;
  const static char* initPaletteName_;
  const static char* patternsName_;
  const static char* bgMapsName_;
  const static char* objMapsName_;
  const static char* scriptName_;
  const static char* pcmDataName_;

  MdPalette initPalette_;
  std::vector<MdPattern> patterns_;
//  int patternLoadAddr_;
  std::vector<YmmTilemap> bgMaps_;
  std::vector<YmmTilemap> objMaps_;
  int unknown_;
  std::string script_;
  std::vector< BlackT::TArray<BlackT::TByte> > pcmData_;
};


}


#endif
