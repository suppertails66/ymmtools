#ifndef YMM_ABLK_OPS_H
#define YMM_ABLK_OPS_H


#include "ymm/YmmAblkOp.h"

namespace Md {


/*

  Op parameter strings work as follows:
    
    b     A byte (8 bits).
    w     A word (16 bits).
    l     A long (32 bits).
    *     A PARAMWORD (16 bits). If top bit is not set, this is a literal;
          otherwise, the low byte is an index into a runtime word array.
    ^     A 0x80 byte should exist at this position.
    []    Enclosed sequence is optional. Whether the sequence exists or not,
          the byte 0x80 must immediately follow it.
    <>    Enclosed sequence is optional, and repeats until the byte 0x80 is
          reached.
    !     Must be specially handled by user.
    
*/

const YmmAblkOp ymmAblkOps[] = {
  YmmAblkOp(0x00, "END",
            ""),
  YmmAblkOp(0x01, "op01",
            "b"),
  YmmAblkOp(0x02, "op02",
            "w!"),
  YmmAblkOp(0x03, "draw",
            "b"),
  YmmAblkOp(0x04, "setPaletteColors",
            "b<w>"),
  YmmAblkOp(0x05, "op05",
            "bb<bb>"),
  YmmAblkOp(0x06, "renderObjsLow?",
            "<b>"),
  YmmAblkOp(0x07, "loadFgQuadrant",
            "bb"),
  YmmAblkOp(0x08, "changeFgTilemapOrNop",
            "[b]"),
  YmmAblkOp(0x09, "changeFgTilemap",
            "[b]"),
  YmmAblkOp(0x0A, "renderObjsHigh?",
            "<b>"),
  YmmAblkOp(0x0B, "op0B",
            "wbb<b><<w>>"),
  YmmAblkOp(0x0C, "setScrollX?",
            "**[wwww]"),
  YmmAblkOp(0x0D, "setScrollY?",
            "**[wwww]"),
  YmmAblkOp(0x0E, "setScroll",
            "**[wwww]"),
  YmmAblkOp(0x0F, "op0F",
            "b"),
  YmmAblkOp(0x10, "transferPatternData",
            ""),
  YmmAblkOp(0x11, "scrollFg",
            "**[ww]"),
  YmmAblkOp(0x12, "scrollBg",
            "**[ww]"),
  YmmAblkOp(0x13, "scrollFgBg",
            "**[ww]"),
  YmmAblkOp(0x14, "setLoopStart",
            "b"),
  YmmAblkOp(0x15, "triggerLoop",
            ""),
  YmmAblkOp(0x16, "op16_mcpu34",
            "**[ww]"),
  YmmAblkOp(0x17, "nop_17",
            ""),
  YmmAblkOp(0x18, "renderSpriteOverride?",
            "bb**"),
  YmmAblkOp(0x19, "setPaletteColors_19",
            "b<w>"),
  YmmAblkOp(0x1A, "playCdAudio?",
            "b"),
  YmmAblkOp(0x1B, "waitForCdd",
            "www"),
  YmmAblkOp(0x1C, "playPcm?",
            "b"),
  YmmAblkOp(0x1D, "waitForPcm?",
            ""),
  YmmAblkOp(0x1E, "writeWord",
            "b*"),
  YmmAblkOp(0x1F, "addWord",
            "b*"),
  YmmAblkOp(0x20, "advanceSeq",
            "bw[w]"),
  YmmAblkOp(0x21, "createSeq",
            "<l>^"),
  YmmAblkOp(0x22, "playSe??",
            "b^"),
  YmmAblkOp(0x23, "playFm??",
            "b"),
  YmmAblkOp(0x24, "nop_24",
            ""),
  YmmAblkOp(0x25, "op25",
            "www"),
  YmmAblkOp(0x26, "op26",
            "ww"),
};

const YmmAblkOp ymmAblkOp02Subops[] = {
  YmmAblkOp(0x00, "op02_00",
            ""),
  YmmAblkOp(0x01, "op02_01",
            "wwwww"),
  YmmAblkOp(0x04, "op02_04",
            "w*w"),
  YmmAblkOp(0x05, "op02_05",
            "w"),
  YmmAblkOp(0x06, "op02_06",
            "ww"),
  YmmAblkOp(0x07, "op02_07",
            "ww"),
  YmmAblkOp(0x09, "op02_09",
            "w"),
  YmmAblkOp(0x0A, "op02_0A",
            "ww"),
  YmmAblkOp(0x0B, "op02_0B",
            "ww"),
  YmmAblkOp(0x0E, "op02_0E",
            "")
};

const int numYmmAblkOps = sizeof(ymmAblkOps) / sizeof(YmmAblkOp);
const int numYmmAblkOp02Subops = sizeof(ymmAblkOp02Subops) / sizeof(YmmAblkOp);

YmmAblkOp makeYmmAblkOp(int id);
YmmAblkOp makeYmmAblkOp02Subop(int id);


}


#endif
