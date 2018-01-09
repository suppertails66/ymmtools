#include "ymm/YmmSubscene.h"
#include "ymm/YmmCompress.h"
#include "ymm/YmmAblkDecompiler.h"
#include "ymm/YmmAblkLexer.h"
#include "ymm/YmmAblkEvaluator.h"
#include "md/MdVram.h"
#include "util/TBufStream.h"
#include "util/TGraphic.h"
#include "util/TPngConversion.h"
#include "util/TStringConversion.h"
#include "util/TIniFile.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace BlackT;

namespace Md {


const char* YmmSubscene::infoFileName_("_info.txt");
const char* YmmSubscene::initPaletteName_("_initpal.bin");
const char* YmmSubscene::patternsName_("_patterns.bin");
const char* YmmSubscene::bgMapsName_("_bgmap");
const char* YmmSubscene::objMapsName_("_objmap");
const char* YmmSubscene::scriptName_("_script.txt");
const char* YmmSubscene::pcmDataName_("_pcm");
  
YmmSubscene::YmmSubscene() { }
  
void YmmSubscene::read(BlackT::TStream& ifs, int baseOffset,
          const YmmPcmList& pcmList) {
/*  int origPos = ifs.tell();
  
  for (int i = 0; i < pcmList.pcmIds.size(); i++) {
//    std::cout << pcmList.pcmIds[i].offset
//      << " " << pcmList.pcmIds[i].size << std::endl;
  
    pcmData_.push_back(BlackT::TArray<BlackT::TByte>());
    BlackT::TArray<BlackT::TByte>& data = pcmData_[
      pcmData_.size() - 1];
    data.resize(pcmList.pcmIds[i].size);
    
    ifs.seek(pcmList.pcmIds[i].offset);
    ifs.read((char*)data.data(), data.size());
  }
  
  ifs.seek(origPos); */
  
  //************************************
  // Initial palette
  //************************************
  
  // colors are stored in non-standard format (low 3 bits of nybble),
  // so we have to manually shift it left a bit before converting
  for (int i = 0; i < MdPalette::numLines; i++) {
    for (int j = 0; j < MdPaletteLine::numColors; j++) {
      initPalette_.lines[i].colors[j].fromRawColor(ifs.readu16be() << 1);
    }
  }
  
  //************************************
  // Data counts
  //************************************
  
  int numTiles = ifs.readu16be();
  int numBgMaps = ifs.readu16be();
  int numObjMaps = ifs.readu16be();
  
//  std::cerr << ifs.tell() << std::endl;
  
  //************************************
  // Pattern data
  //************************************
  
  {
    TBufStream ofs((numTiles + 1) * MdPattern::uncompressedSize);
    YmmCompress::decmpYmmRleTypes(ifs, ofs);
    ofs.seek(0);
    patterns_.resize(numTiles);
    for (int i = 0; i < patterns_.size(); i++) {
      patterns_[i].read(ofs);
    }
  }
  
  //************************************
  // BG maps
  //************************************
  
  if (numBgMaps != 0xFFFF) {
    for (int i = 0; i < numBgMaps + 1; i++) {
      TBufStream ofs(bytesPerBgMap_);
      YmmCompress::decmpYmmTilemap(ifs, ofs);
      bgMaps_.push_back(YmmTilemap());
      YmmTilemap& tilemap = bgMaps_[bgMaps_.size() - 1];
      ofs.seek(0);
      tilemap.read(ofs, bgMapTileWidth_, bgMapTileHeight_);
    }
  }
  
  //************************************
  // Object maps
  //************************************
  
  if (numObjMaps != 0) {
    for (int i = 0; i < numObjMaps; i++) {
      int x = ifs.readu16be();
      int y = ifs.readu16be();
      
      int pixelW = ifs.readu16be();
      int pixelH = ifs.readu16be();
      
      int tileW = pixelW >> 3;
      int tileH = pixelH >> 3;
//      int numTiles = tileW * tileH;
      
      objMaps_.push_back(YmmTilemap());
      YmmTilemap& tilemap = objMaps_[objMaps_.size() - 1];
      tilemap.read(ifs, tileW, tileH);
      tilemap.setX(x);
      tilemap.setY(y);
      
    }
  }
  
  //************************************
  // ???
  //************************************
  
//  ifs.seekoff(3);
  unknown_ = ifs.readInt(3, EndiannessTypes::big);
//  unknown_ = ifs.readu16be();
//  unknown_ = 0;
  
  //************************************
  // Script data
  //************************************
  {
    YmmAblkDecompiler decompiler
      = YmmAblkDecompiler("??", YmmEngineVersions::mcd);
    std::ostringstream ofs;
    decompiler.decompile(ifs, ofs);
    script_ = ofs.str();
  }
  
  //************************************
  // PCM files
  //************************************
  
  for (int i = 0; i < pcmList.pcmIds.size(); i++) {
//    std::cout << pcmList.pcmIds[i].offset
//      << " " << pcmList.pcmIds[i].size << std::endl;
  
    pcmData_.push_back(BlackT::TArray<BlackT::TByte>());
    BlackT::TArray<BlackT::TByte>& data = pcmData_[
      pcmData_.size() - 1];
    data.resize(pcmList.pcmIds[i].size);
    
    ifs.seek(pcmList.pcmIds[i].offset + baseOffset);
    ifs.read((char*)data.data(), data.size());
  }
}

void YmmSubscene::write(BlackT::TStream& ofs,
          YmmPcmList& pcmList) const {
//  initPalette_.write(ofs);
  for (int i = 0; i < MdPalette::numLines; i++) {
    for (int j = 0; j < MdPaletteLine::numColors; j++) {
      ofs.writeu16be(initPalette_.lines[i].colors[j].rawColor() >> 1);
    }
  }
  ofs.writeu16be(patterns_.size());
  ofs.writeu16be(bgMaps_.size() - 1);
  ofs.writeu16be(objMaps_.size());
  
  // patterns
  {
    TBufStream patternsRaw(0x100000);
    for (int i = 0; i < patterns_.size(); i++) {
      patterns_[i].write(patternsRaw);
    }
    TBufStream patternsCompressed(0x100000);
    patternsRaw.seek(0);
    YmmCompress::cmpYmmRleTypes(patternsRaw, patternsCompressed);
    ofs.write((char*)patternsCompressed.data().data(),
              patternsCompressed.size());
  }
  
//  std::cerr << std::hex << ofs.tell() << std::endl;
  
  // background maps
  {
    for (int i = 0; i < bgMaps_.size(); i++) {
      TBufStream mapRaw(0x100000);
      bgMaps_[i].write(mapRaw);
      TBufStream mapCompressed(0x100000);
      mapRaw.seek(0);
      YmmCompress::cmpYmmTilemap(mapRaw, mapCompressed);
//      std::cerr << std::hex << ofs.tell() << std::endl;
      ofs.write((char*)mapCompressed.data().data(),
                mapCompressed.size());
//      bgMaps_[i].write(ofs);
    }
  }
  
  for (int i = 0; i < objMaps_.size(); i++) {
    ofs.writeu16be(objMaps_[i].x());
    ofs.writeu16be(objMaps_[i].y());
    ofs.writeu16be(objMaps_[i].w() * MdPattern::w);
    ofs.writeu16be(objMaps_[i].h() * MdPattern::h);
    objMaps_[i].write(ofs);
  }
  ofs.writeInt(unknown_, 3, EndiannessTypes::big);
//  ofs.writeu16be(unknown_);
  
  // lex and evaluate the script, then output it
  {
    TBufStream scriptStream(1);
    scriptStream.fromString(script_);
    YmmAblkLexStream lexStream;
    YmmAblkLexer lexer(scriptStream, lexStream);
    lexer.lex();
    TBufStream scriptBuf(0x100000);
    YmmAblkEvaluator evaluator(lexStream, scriptBuf);
    evaluator.evaluate();
    int compSz = scriptBuf.tell();
    ofs.write((char*)scriptBuf.data().data(), compSz);
  }
  
  // ?
//  ofs.alignToWriteBoundary(2);
  
  for (int i = 0; i < pcmData_.size(); i++) {
    int pos = ofs.tell();
    ofs.write((char*)pcmData_[i].data(), pcmData_[i].size());
    pcmList.addEntry(pos, pcmData_[i].size());
  }
}
  
void YmmSubscene::save(const char* prefix) const {
  std::string prefixStr(prefix);
  
  {
    TIniFile infoFile;
    
    infoFile.setValue("Properties", "Unknown",
                      TStringConversion::intToString(unknown_,
                        TStringConversion::baseHex));
    
    infoFile.setValue("Properties", "NumBgMaps",
                      TStringConversion::intToString(bgMaps_.size(),
                        TStringConversion::baseHex));
    
    infoFile.setValue("Properties", "NumObjMaps",
                      TStringConversion::intToString(objMaps_.size(),
                        TStringConversion::baseHex));
    
    infoFile.setValue("Properties", "NumPcmData",
                      TStringConversion::intToString(pcmData_.size(),
                        TStringConversion::baseHex));
    
    for (int i = 0; i < objMaps_.size(); i++) {
      infoFile.setValue("ObjMap_"
                          + TStringConversion::intToString(i),
                        "x",
                        TStringConversion::intToString(objMaps_[i].x(),
                          TStringConversion::baseHex));
      infoFile.setValue("ObjMap_"
                          + TStringConversion::intToString(i),
                        "y",
                        TStringConversion::intToString(objMaps_[i].y(),
                          TStringConversion::baseHex));
    }
    
    infoFile.writeFile(prefixStr + infoFileName_);
  }
  
  // Palette
  {
    TOfstream ofs((prefixStr + initPaletteName_).c_str());
    initPalette_.write(ofs);
  }
  
  // PCM data
  for (int i = 0; i < pcmData_.size(); i++) {
    TOfstream ofs((prefixStr + pcmDataName_
                    + TStringConversion::intToString(i)
                    + ".bin").c_str(),
                  std::ios_base::binary);
    ofs.write((char*)pcmData_[i].data(), pcmData_[i].size());
  }
  
  // Patterns
  {
    TOfstream ofs((prefixStr + patternsName_).c_str(),
                  std::ios_base::binary);
    for (int i = 0; i < patterns_.size(); i++) {
      patterns_[i].write(ofs);
    }
  }
  
  // Set up VRAM
  MdVram vram;
  for (int i = 0; i < patterns_.size(); i++) {
    vram.setPattern(i, patterns_[i]);
  }
  
  // Background maps
  for (int i = 0; i < bgMaps_.size(); i++) {
    TOfstream ofs((prefixStr + bgMapsName_
                    + TStringConversion::intToString(i)
                    + ".bin").c_str(),
                  std::ios_base::binary);
    bgMaps_[i].write(ofs);
    
    // Convert to graphic
    TGraphic g;
//    if (hasPalette) {
//      orientedMap.toColorGraphic(g, vram, initPalette_);
//    }
//    else {
      bgMaps_[i].toGrayscaleGraphic(g, vram);
//    }
    
    // Save result
    TPngConversion::graphicToRGBAPng((prefixStr + bgMapsName_
                    + TStringConversion::intToString(i)
                    + ".png"), g);
  }
  
  // Object maps
  for (int i = 0; i < objMaps_.size(); i++) {
    TOfstream ofs((prefixStr + objMapsName_
                    + TStringConversion::intToString(i)
                    + ".bin").c_str(),
                  std::ios_base::binary);
    objMaps_[i].write(ofs);
    
    // Convert to graphic
    TGraphic g;
//    if (hasPalette) {
//      orientedMap.toColorGraphic(g, vram, initPalette_);
//    }
//    else {
      objMaps_[i].toGrayscaleGraphic(g, vram);
//    }
    
    // Save result
    TPngConversion::graphicToRGBAPng((prefixStr + objMapsName_
                    + TStringConversion::intToString(i)
                    + ".png"), g);
  }
  
  // Script
  {
    std::ofstream ofs((prefixStr + scriptName_).c_str());
    ofs << script_;
  }
  
}

void YmmSubscene::load(const char* prefix) {
  std::string prefixStr(prefix);
  
  patterns_.clear();
  bgMaps_.clear();
  objMaps_.clear();
  pcmData_.clear();
  script_ = "";
  
  {
    TIniFile infoFile(prefixStr + infoFileName_);
    unknown_ = TStringConversion::stringToInt(
      infoFile.valueOfKey("Properties", "Unknown"));
    bgMaps_.resize(TStringConversion::stringToInt(
      infoFile.valueOfKey("Properties", "NumBgMaps")));
    objMaps_.resize(TStringConversion::stringToInt(
      infoFile.valueOfKey("Properties", "NumObjMaps")));
    pcmData_.resize(TStringConversion::stringToInt(
      infoFile.valueOfKey("Properties", "NumPcmData")));
    
    for (int i = 0; i < objMaps_.size(); i++) {
      objMaps_[i].setX(TStringConversion::stringToInt(
        infoFile.valueOfKey("ObjMap_"
                          + TStringConversion::intToString(i),
                          "x")));
      objMaps_[i].setY(TStringConversion::stringToInt(
        infoFile.valueOfKey("ObjMap_"
                          + TStringConversion::intToString(i),
                          "y")));
    }
  }
  
  // Palette
  {
    TIfstream ifs((prefixStr + initPaletteName_).c_str());
    initPalette_.read(ifs);
  }
  
  // PCM data
  for (int i = 0; i < pcmData_.size(); i++) {
    TIfstream ifs((prefixStr + pcmDataName_
                    + TStringConversion::intToString(i)
                    + ".bin").c_str(),
                  std::ios_base::binary);
    pcmData_[i].resize(ifs.size());
    ifs.read((char*)pcmData_[i].data(), pcmData_[i].size());
  }
  
  // Patterns
  // We don't read anything, since these are generated from the maps.
  // However, we do have to make sure pattern zero is clear (all zeroes),
  // since the game (probably?) depends on this when zero-initializing
  // tilemaps.
  patterns_.push_back(MdPattern());
  for (int j = 0; j < MdPattern::h; j++) {
    for (int i = 0; i < MdPattern::w; i++) {
      patterns_.back().pattern.data(i, j) = 0;
    }
  }
  
  // Background maps
  for (int i = 0; i < bgMaps_.size(); i++) {
    TGraphic g;
    TPngConversion::RGBAPngToGraphic((prefixStr + bgMapsName_
                    + TStringConversion::intToString(i)
                    + ".png"), g);
    bgMaps_[i].fromGrayscaleGraphic(g, patterns_);
  }
  
  // Object maps
  for (int i = 0; i < objMaps_.size(); i++) {
    TGraphic g;
    TPngConversion::RGBAPngToGraphic((prefixStr + objMapsName_
                    + TStringConversion::intToString(i)
                    + ".png"), g);
    objMaps_[i].fromGrayscaleGraphic(g, patterns_);
  }
  
  // Script
  {
    TIfstream ifs((prefixStr + scriptName_).c_str());
    int sz = ifs.size();
    TBufStream buf(sz);
    ifs.read((char*)buf.data().data(), sz);
    script_ = std::string((char*)buf.data().data(), sz);
  }
}


}
