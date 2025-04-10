//
//  MJMatchModel.h
//  Mahsjong
//
//  Created by Patrick Choo on 3/27/25.
//

#ifndef __MJ_MATCHMODEL_H__
#define __MJ_MATCHMODEL_H__
#include <cugl/cugl.h>
#include "MJTileSet.h"
#include "MJPlayer.h"
#include "MJPile.h"
#include "MJDiscardPile.h"
#include "MJDiscardUIScene.h"
#include "MJNetworkController.h"

/**
 * A managing the current state of the game. This includes the pile, deck, and the tiles.
 */
class MatchModel {
public:
    /** Enum for states of the game */
    enum Choice {
        NONE,
        PAUSE,
        TILESET,
        SETS,
        DISCARDED,
        DRAW_DISCARD,
        WIN,
        LOSE
    }
    
protected:
    /** The network connection */
    std::shared_ptr<NetworkController> _network;
    /** The TileSet (the model holding all of our tiles) */
    std::shared_ptr<TileSet> _tileset;
    
public:
    
    
}

#endif /* __MJ_TILESET_H__ */
