#include "ymm/YmmScene.h"
#include "util/TStringConversion.h"
#include "util/TIniFile.h"
#include "util/TBufStream.h"
#include <string>
#include <cstring>

using namespace BlackT;

namespace Md {


const char* YmmScene::sceneName_("scene");
const char* YmmScene::infoFileName_("info.txt");
  
YmmScene::YmmScene() { }
  
void YmmScene::read(BlackT::TStream& ifs) {
  char headcheck[5];
  ifs.read(headcheck, 4);
  headcheck[4] = 0x00;
  
  if (strcmp(headcheck, "ABLK") == 0) sceneType_ = YmmSceneTypes::ablk;
  else {
    sceneType_ = YmmSceneTypes::oneshot;
    
    // Read the one contained subscene and return
    
    ifs.seekoff(-4);
    subscenes_.push_back(YmmSubscene());
    
    // no PCMs allowed -- use empty list
    YmmPcmList pcmList;
    subscenes_[0].read(ifs, 0, pcmList);
    
    return;
  }
  
  // loop over entries in header until subscene terminator (high nybble 0xF)
  // reached
  int next = ifs.readu32be();
  while ((next & 0xF0000000) != 0xF0000000) {
    std::cout << "Processing subscene " << subscenes_.size() << std::endl;
    
//    std::cout << std::hex << next << std::endl;
//    char c; std::cin >> c;
  
    YmmPcmList pcmList;
    pcmList.read(ifs);
    
    int nextpos = ifs.tell();
    
    subscenes_.push_back(YmmSubscene());
    YmmSubscene& scene = subscenes_[subscenes_.size() - 1];
    ifs.seek((next & 0x00FFFFFF) + 4);
    scene.read(ifs, 4, pcmList);
    
    ifs.clear();
    ifs.seek(nextpos);
    
    next = ifs.readu32be();
  }
}

void YmmScene::write(BlackT::TStream& ofs) const {
  if (sceneType_ == YmmSceneTypes::oneshot) {
    if (subscenes_.size() > 0) {
      YmmPcmList pcm;
      subscenes_[0].write(ofs, pcm);
    }
    return;
  }
  
  std::vector<TBufStream> sceneBuffers;
  std::vector<YmmPcmList> pcmLists;
  int numPcmFiles = 0;
  for (int i = 0; i < subscenes_.size(); i++) {
    sceneBuffers.push_back(TBufStream(0x100000));
    pcmLists.push_back(YmmPcmList());
    subscenes_[i].write(sceneBuffers.back(), pcmLists.back());
    numPcmFiles += pcmLists.back().pcmIds.size();
  }
  
  // compute index size
  // 4 bytes per subscene offset
  // 4 bytes per PCM offset
  // 4 bytes for list terminator
  int indexSize = (subscenes_.size() * 4)
    + (numPcmFiles * 4)
    + 4;
  
  ofs.write("ABLK", 4);
  
  int subsceneOffset = indexSize;
  for (int i = 0; i < pcmLists.size(); i++) {
    ofs.writeu32be(subsceneOffset);
    for (int j = 0; j < pcmLists[i].pcmIds.size(); j++) {
      int offset = pcmLists[i].pcmIds[j].offset + subsceneOffset;
      ofs.writeu32be(0x80000000 | offset);
    }
    subsceneOffset += sceneBuffers[i].size();
    
    // write terminator after last entry
    if (i == (pcmLists.size() - 1)) {
      ofs.writeu32be(0xF0000000 | subsceneOffset);
    }
  }
  
  // write each subscene's data
  for (int i = 0; i < sceneBuffers.size(); i++) {
    ofs.write((char*)sceneBuffers[i].data().data(),
              sceneBuffers[i].size());
  }
}
  
void YmmScene::save(const char* prefix) const {
  std::string prefixStr(prefix);
  
  {
    TIniFile infoFile;
    
    infoFile.setValue("Properties", "SceneType",
                      TStringConversion::intToString(sceneType_,
                        TStringConversion::baseHex));
    
    infoFile.setValue("Properties", "NumSubscenes",
                      TStringConversion::intToString(subscenes_.size(),
                        TStringConversion::baseHex));
    
    infoFile.writeFile(prefixStr + infoFileName_);
  }
  
  for (int i = 0; i < subscenes_.size(); i++) {
    std::string name = prefixStr + "scene"
      + TStringConversion::intToString(i);
    subscenes_[i].save(name.c_str());
  }
}

void YmmScene::load(const char* prefix) {
  std::string prefixStr(prefix);
  
  subscenes_.clear();
  
  {
    TIniFile infoFile(prefixStr + infoFileName_);
    
    sceneType_ = static_cast<YmmSceneTypes::YmmSceneType>(
      TStringConversion::stringToInt(
        infoFile.valueOfKey("Properties", "SceneType")));
    
    subscenes_.resize(TStringConversion::stringToInt(
      infoFile.valueOfKey("Properties", "NumSubscenes")));
  }
  
  for (int i = 0; i < subscenes_.size(); i++) {
    subscenes_[i].load((prefixStr + "scene"
      + TStringConversion::intToString(i)).c_str());
  }
}


}
