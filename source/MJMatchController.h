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
#include "MJAudioController.h"

/**
 * A managing the current state of the game. This includes the pile, deck, and the tiles.
 */
class MatchController {
public:
    /** Enum for states of the game */
    enum Choice {
        NONE,
        PILEDRAW,
        DISCARDUIUPDATE,
        MONKEYTILE,
        RATTILE,
        DRAGONTILE,
        PIGTILE,
        DRAWNDISCARD,
        SUCCESS_SET,
        FAILED_SET, 
        WIN,
        LOSE,
        TIE
    };
    
    enum OpponentAnimType {
        INACTIVE,
        OX,
        SNAKE,
        RABBIT,
        MONKEY,
        RAT,
        DRAGON,
        ROOSTER,
        PIG
    };
    
    /** If current player has already drawn from the pile/discard or not */
    bool hasDrawn = false;
    /** If current player has already discarded or not */
    bool hasDiscarded = false;
    /** If current player has played a celestial tile or not */
    bool hasPlayedCelestial = false;
    /** If the current player has timed out */
    bool hasTimedOut = false;
    /** If we are in tutorial mode */
    bool inTutorial = false; 
    
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
    /** Opponent anim enum*/
    OpponentAnimType _celestialAnim;
    /** The instance of the monkey tile that was played */
    std::shared_ptr<TileSet::Tile> _monkeyTile;
    /** The instance of the rat tile that was played */
    std::shared_ptr<TileSet::Tile> _ratTile;
    /** The instance of the dragon tile that was played */
    std::shared_ptr<TileSet::Tile> _dragonTile;
    /** The instance of the pig tile that was played */
    std::shared_ptr<TileSet::Tile> _pigTile;
    /** Currnet active state of game */
    bool _active; 
    /** Tiles to display in the opponent played set tab */
    std::vector<std::shared_ptr<TileSet::Tile>> setTiles;
    
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
     * Disposes the MatchController
     */
    ~MatchController() { dispose(); }
    
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
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<NetworkController>& network);
    
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
     * Initializes the game in tutorial mode. Initializes the tileset based on a preset deck JSON
     * and instantiates only one player.
     */
    void initTutorial(); 
    
    /**
     * Draws a tile from the pile to the player that called the method. After drawing, it broadcasts
     * the state of the pile and updates any tiles associated with the action performed
     */
    void drawTile();
    
    /**
     *  Draws the most recently drawn tile from the discard pile. After drawing, it broadcasts that a
     *  tile has been drawn from the discard pile. However, for the sake of efficiency, the broadcasted
     *  message DOES NOT remove the discarded tile from the pile, rather it makes the tile invisible
     *  giving the illusion that it has been drawn.
     *
     * @returns     true if drawing from discard is successful
     */
    bool drawDiscard(); 
    
    /**
     * Discards the currently dragged tile. After discarding, it broadcasts the newly discarded tile
     * to opposing player and updates the discarded pile element.
     *
     * @param isHost       Whether or not current player is host
     * @param tile      The tile that was discarded
     * @returns true if discard was successful, else false
     */
    bool discardTile(std::shared_ptr<TileSet::Tile> tile);
    
    /**
     * Plays the currently selected tiles as a set for this player.
     *
     * @return  true if playing set was successful, false otherwise
     */
    bool playSet();
    
    /**
     * Plays the inputted celestial tile, if tile is not a celestial tile then the function returns false.
     * Calls the appropriate function according to the celestial tile type. It then broadcasts
     * the tile that was played.
     *
     * @param celestialTile     The celestial tile that was played
     * @returns True if celestial was played, false if not
     */
    bool playCelestial(std::shared_ptr<TileSet::Tile>& celestialTile);
    
    /**
     * Executes the Rooster celestial tile effect (reshuffle pile) in current game scene. It then broadcasts the change
     * to opposing player.
     */
    void playRooster(std::shared_ptr<TileSet::Tile>& celestialTile);
    
    /**
     * Executes the Ox celestial tile effect (random debuff 2) in current game scene. It then broadcasts the change
     * to opposing player.
     */
    void playOx(std::shared_ptr<TileSet::Tile>& celestialTile);
    
    /**
     * Executes the Rabbit celestial tile effect  (change rank of random tile) in current game scene. It then broadcasts the change
     * to opposing player.
     */
    void playRabbit(std::shared_ptr<TileSet::Tile>& celestialTile);
    
    /**
     * Executes the Snake celestial tile effect  (change suit of random tile) in current game scene. It then broadcasts the change
     * to opposing player.
     */
    void playSnake(std::shared_ptr<TileSet::Tile>& celestialTile);
    
    /**
     * Executes the Monkey celestial tile effect (trade tiles) given the selected tile by the player. It will give the selected tile to the opponent and then take a random tile from them.
     */
    void playMonkey(std::shared_ptr<TileSet::Tile>& selectedTile);
    
    /**
     * Executes the Rat celestial tile effect (draw any tile from pile) given the selected tile by the player. It will remove the tile from the pile and add it to player's hand.
     */
    void playRat(std::shared_ptr<TileSet::Tile>& selectedTile);
    
    /**
     * Executes the Dragon celestial tile effect (rearrange a pile row) .
     */
    void playDragon();
    
    /**
     * Executes the Pig celestial tile effect (draw any tile from discard) .
     */
    void playPig(std::pair<TileSet::Tile::Suit, TileSet::Tile::Rank> tile);
    
    /** Applies the effect of the celestial tile played by opponent by using the celestial state of the network. */
    void celestialEffect();
    
    /**
     * Call back for ending the turn for the current player. Must have drawn from the pile and dicsarded/played
     * a tile in order to end turn successfully. Resets the current turn requirements with a sucessful end.
     */
    void endTurn();
    
    /**
     * Resets the state of the current turn. Called after the turn ends to allow the next player to draw,
     * play, and discard tiles.
     */
    void resetTurn() {
        hasDrawn = false;
        hasDiscarded = false;
        hasPlayedCelestial = false;
        hasTimedOut = false;
    }
    
    /** Handles game win by broadcasting to opponent that they have a full mahjong hand  */
    void handleGameWin();
    
    /** Gets the current state of game */
    Choice getChoice() {
        return _choice; 
    }
    
    /** Sets the current state of game */
    void setChoice(Choice choice) {
        _choice = choice;
    }
    
    std::vector<std::shared_ptr<TileSet::Tile>> getSetTiles() { return setTiles; }
    
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
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose();
    
<<<<<<< HEAD
    
    std::vector<std::shared_ptr<TileSet::Tile>> getWinningHand() const {
        bool isHost = _network->getHostStatus();
        return isHost ? hostPlayer->getHand()._tiles : clientPlayer->getHand()._tiles;
    }
    
    std::vector<std::shared_ptr<TileSet::Tile>> getHostHand() const {
        return hostPlayer->getHand()._tiles;
    }
    
    std::vector<std::shared_ptr<TileSet::Tile>> getClientHand() const {
        return clientPlayer->getHand()._tiles;
    }

    
    
    
    
    
=======
    void setOpponentAnimType(OpponentAnimType type){
        _celestialAnim = type;
    }
    
    OpponentAnimType getOpponentAnimType(){
        return _celestialAnim;
    }
>>>>>>> origin/main-2.0
};

#endif /* __MJ_MATCH_CONTROLLER__ */
