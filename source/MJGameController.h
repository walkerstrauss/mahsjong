//
//  MJMatchController.h
//  Mahsjong
//
//  Created by Patrick Choo on 3/27/25.
//

#ifndef __MJ_MATCH_CONTROLLER_H__
#define __MJ_MATCH_CONTROLLER_H__
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
class MatchController {
public:
    /** Enum for states of the game */
    enum Choice {
        NONE,
        WIN,
        LOSE,
        TIE
    };
    
protected:
    /** The network connection */
    std::shared_ptr<NetworkController> _network;
    /** The asset manager */
    std::shared_ptr<cugl::AssetManager> _assets;
    /** The TileSet (the model holding all of our tiles) */
    std::shared_ptr<TileSet> _tileset;
    /** The pile (the model holding the tiles in the layer) */
    std::shared_ptr<Pile> _pile;
    
public:
    /** The host player */
    std::shared_ptr<Player> hostPlayer;
    /** The client player */
    std::shared_ptr<Player> clientPlayer;
    
    /**
     * Creates a new game mode with the default values
     *
     * This constructor does not allocate any objects or start the game.
     * This allows us to use the object without a heap pointer
     */
    MatchController();
    
    /**
     * Initializes the controller contents, and starts the game
     *
     * The constructor does not allocate any objects or memory. This allows
     * us to have a non-pointer reference to this controller, reducing our memory
     * allocation. Instead, allocation happens in this method
     *
     * @param assets    the asset manager for the game
     * @param network   the network controller for the game
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<NetworkController> network);
    
    /**
     * Initializes the host game with objects and sends INGAME status to client.
     */
    void initHost();
    
    /**
     * Initializes the host game with objects and sends INGAME status to client.
     */
    void initClient();
};

#endif /* __MJ_MATCH_CONTROLLER__ */
