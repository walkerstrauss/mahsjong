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
        DISCARDUIUPDATE,
        WIN,
        LOSE
    };
    
protected:
    /** The network connection */
    std::shared_ptr<NetworkController> _network;
    /** The asset manager */
    std::shared_ptr<cugl::AssetManager> _assets;
    /** The TileSet (the model holding all of our tiles) */
    std::shared_ptr<TileSet> _tileSet;
    /** The pile (the model holding the tiles in the layer) */
    std::shared_ptr<Pile> _pile;
    /** The discard pile holding the discarded tiles by both players*/
    std::shared_ptr<DiscardPile> _discardPile; 
    /** The current state of the game */
    Choice _choice;
    
    /** If current player has already drawn from the pile/discard or not */
    bool hasDrawn = false;
    /** If current player has already discarded or not */
    bool hasDiscarded = false;
    /** If current player has played a celestial tile or not */
    bool hasPlayedCelestial= false;
    
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
    MatchController() {};
    
    /**
     * Initializes the controller contents, and starts the game
     *
     * The constructor does not allocate any objects or memory. This allows
     * us to have a non-pointer reference to this controller, reducing our memory
     * allocation. Instead, allocation happens in this method
     *
     * @param assets    the asset manager for the game
     * @param network   the network controller for the game
     *  @returns true if init was successful, false otherwise
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<NetworkController> network);
    
    /**
     * Initializes the host game with objects and sends INGAME status to client. This
     * function initializes the initial representation and state of all in-game objects; this
     * includes the tileset, deck, pile, discard pile, and players.
     */
    void initHost();
    
    /**
     * Initializes the client game with the representation as made by host. Reads the tileSet
     * map JSON, instantiating the tilset, deck, players, and pile as specificed by the JSON. 
     */
    void initClient();
    
    /**
     * Draws a tile from the pile to the player that called the method. After drawing, it broadcasts
     * the state of the pile and updates any tiles associated with the action performed
     */
    void drawTile(bool isHost);
    
    /**
     * Discards the currently dragged tile. After discarding, it broadcasts the newly discarded tile
     * to opposing player and updates the discarded pile element.
     *
     * @param isHost       Whether or not current player is host
     * @param tile      The tile that was discarded
     * @returns true if discard was successful, else false
     */
    bool discardTile(bool isHost, std::shared_ptr<TileSet::Tile> tile);
    
    /**
     * Plays the inputted celestial tile, if tile is not a celestial tile then the function returns false.
     * Calls the appropriate function according to the celestial tile type. It then broadcasts
     * the tile that was played.
     *
     * @param isHost       Whether or not current player is host
     * @param celestialTile     The celestial tile that was played
     * @returns True if celestial was played, false if not
     */
    bool playCelestial(bool isHost, std::shared_ptr<TileSet::Tile>& celestialTile);
    
    /**
     * Executes CHAOS celestial tile effect in current game scene. It then broadcasts the change
     * to opposing player.
     *
     * @param isHost       Whether or not current player is host
     */
    bool playEcho(bool isHost);
    
    /**
     * Resets the state of the current turn. Called after the turn ends to allow the next player to draw,
     * play, and discard tiles.
     */
    void resetTurn() {
        hasDrawn = false;
        hasDiscarded = false;
        hasPlayedCelestial = false;
    }
    
    /** Gets the current state of game */
    Choice getChoice() {
        return _choice; 
    }
    
    void setChoice(Choice choice) {
        _choice = choice;
    }
    
    /** Gets the current tileSet representation */
    std::shared_ptr<TileSet> getTileSet() {
        return _tileSet;
    }
    
    /** Gets the current pile representation */
    std::shared_ptr<Pile> getPile() {
        return _pile;
    }
    
    /** Gets the current discard pile representation */
    std::shared_ptr<DiscardPile> getDiscardPile() {
        return _discardPile;
    }
    
    /**
     * The method called to update the game mode
     *
     * @param timestep The amount of time (in seconds) since the last frame
     */
    void update(float timestep);

};

#endif /* __MJ_MATCH_CONTROLLER__ */
