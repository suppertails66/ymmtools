#ifndef YMM_SCENE_H
#define YMM_SCENE_H


#include "util/TStream.h"
#include "ymm/YmmSubscene.h"
#include <vector>

namespace Md {


namespace YmmSceneTypes {
  enum YmmSceneType {
    ablk,
    oneshot
  };
}

class YmmScene {
public:
  YmmScene();
  
  void read(BlackT::TStream& ifs);
  void write(BlackT::TStream& ofs) const;
  
  void save(const char* prefix) const;
  void load(const char* prefix);
protected:
  const static char* sceneName_;
  const static char* infoFileName_;

  YmmSceneTypes::YmmSceneType sceneType_;
  
  std::vector<YmmSubscene> subscenes_;
};


}


#endif
