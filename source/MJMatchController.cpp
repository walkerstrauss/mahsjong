//
//  MJMatchController.cpp
//  Mahsjong
//
//  Created by Patrick Choo on 3/27/25.
//

#include <cugl/cugl.h>
#include <iostream>
#include <sstream>
#include "MJMatchController.h"

using namespace cugl;
using namespace cugl::graphics;
using namespace cugl::audio;
using namespace std;

#pragma mark -
#pragma mark Constructors

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
bool MatchController::init(const std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<NetworkController> network) {
    _assets = assets;
    _network = network;
    
    hostPlayer = std::make_shared<Player>();
    clientPlayer = std::make_shared<Player>();
    
    _tileSet = std::make_shared<TileSet>();
    _pile = std::make_shared<Pile>();
    _discardPile = std::make_shared<DiscardPile>();
    _discardPile->init(_assets);
    
    _choice = Choice::NONE;
    
    return true; 
}

/**
 * Initializes the host game with objects and sends INGAME status to client. This
 * function initializes the initial representation and state of all in-game objects; this
 * includes the tileset, deck, pile, discard pile, and players.
 */
void MatchController::initHost() {
    //Initializing the host deck
    _tileSet->initHostDeck();
    _tileSet->addActionAndCommandTiles();
    _tileSet->setAllTileTexture(_assets);
    _tileSet->shuffle();
    
    //Initializing host and client players
    hostPlayer->getHand().initHand(_tileSet, true);
    clientPlayer->getHand().initHand(_tileSet, false);
    
    //Initializing pile
    _pile->initPile(4, _tileSet, true);
    
    //Broadcast initial state
    _network->broadcastClientStart(_tileSet->mapToJson());
}

/**
 * Initializes the client game with the representation as made by host. Reads the tileSet
 * map JSON, instantiating the tilset, deck, players, and pile as specificed by the JSON.
 */
void MatchController::initClient() {
    //Initialzing the client deck
    _tileSet->initClientDeck(_network->getClientStart());
    _tileSet->setAllTileTexture(_assets);
    _tileSet->updateDeck(_network->getClientStart());
    
    // Assigning client and host hands
    for(auto& pairs : _tileSet->tileMap) {
        std::shared_ptr<TileSet::Tile> currTile = pairs.second;
        if(currTile->inHostHand) {
            hostPlayer->getHand()._tiles.push_back(currTile);
        }
        else if(currTile->inClientHand) {
            clientPlayer->getHand()._tiles.push_back(currTile);
        }
    }
    
    //Initializing the pile
    _pile->initPile(4, _tileSet, false);
    _pile->remakePile();
}

/**
 * Draws a tile from the pile to the player that called the method. After drawing, it broadcasts
 * the state of the pile and updates any tiles associated with the action performed
 */
void MatchController::drawTile(bool isHost) {
    // Host draw logic
    if(isHost) {
        if(hostPlayer->getHand()._tiles.size() <= hostPlayer->getHand()._size) {
            hostPlayer->getHand().drawFromPile(_pile, 1, true);
            // If tile was winning tile
            if(hostPlayer->getHand().isWinningHand()) {
                _choice = Choice::WIN;
                _network->broadcastEnd(0);
                return;
            }
            // Network drawn tile (automatically accumulated in tilesToJson when drawing)
            _network->broadcastTileDrawn(0, _tileSet->toJson(_tileSet->tilesToJson));
            // Clear tilesToJson for subsequent draws
            _tileSet->clearTilesToJson();
            
            // Remake pile if tile was last tile in pile
            if(_pile->getVisibleSize() == 0) {
                _pile->createPile();
                _network->broadcastTileMap(0, _tileSet->mapToJson(), "remake pile");
            }
        }
        //Client draw logic
        else if (!isHost) {
            if (clientPlayer->getHand()._tiles.size() <= clientPlayer->getHand()._size) {
                clientPlayer->getHand().drawFromPile(_pile, 1, false);
                if(clientPlayer->getHand().isWinningHand()) {
                    _choice = Choice::WIN;
                    _network->broadcastEnd(1);
                }
                
                // Network drawn tile (automatically accumulated in tilesToJson when drawing)
                _network->broadcastTileDrawn(1, _tileSet->toJson(_tileSet->tilesToJson));
                // Clear tilesToJson for subsequent draws
                _tileSet->clearTilesToJson();
                
                // Remake pile if tile was last tile in pile
                if(_pile->getVisibleSize() == 0) {
                    _pile->createPile();
                    _network->broadcastTileMap(1, _tileSet->mapToJson(), "remake pile");
                }
            }
        }
    }
}

/**
 * Discards the currently dragged tile. After discarding, it broadcasts the newly discarded tile
 * to opposing player and updates the discarded pile element.
 *
 * @param tile      The tile that was discarded
 */
bool MatchController::discardTile(bool isHost, std::shared_ptr<TileSet::Tile> tile) {
    
    // If hand is required size, then discard
    if((isHost && hostPlayer->getHand()._tiles.size() > hostPlayer->getHand()._size) ||
            (!isHost && clientPlayer->getHand()._tiles.size() > clientPlayer ->getHand()._size)){
        // Setting fields to discarded
        tile->selected = false;
        tile->inHostHand = false;
        tile->inClientHand = false;
        tile->discarded = true;
        
        //If host
        if(_network->getLocalPid() == 0) {hostPlayer->getHand().discard(tile, true);}
        //If client
        else{clientPlayer->getHand().discard(tile, false);}
        
        // If not a special tile add it to the discard pile
        if (!(tile->_rank == TileSet::Tile::Rank::ACTION || tile->_rank == TileSet::Tile::Rank::COMMAND)){
            _discardPile->addTile(tile);
            _discardPile->updateTilePositions();
            
            // Converting to JSON and broadcasting discarded tile
            _tileSet->tilesToJson.push_back(tile);
            _network->broadcastDiscard(_network->getLocalPid(), _tileSet->toJson(_tileSet->tilesToJson));
            _tileSet->clearTilesToJson();
            
            return true;
        }
    }
    return false; 
}

/**
 * The method called to update the game mode
 *
 * @param timestep The amount of time (in seconds) since the last frame
 */
void MatchController::update(float timestep) {
    // If we receieve end game status, current player loses
    if(_network->getStatus() == NetworkController::ENDGAME) {
        _choice = LOSE;
    }
    
    // Tile has been drawn from the pile 
    if(_network->getStatus() == NetworkController::TILEDRAWN) {
        bool isHost = _network->getCurrentTurn() == 0;
        _pile->removePileTile(_network->getTileDrawn(), isHost);
        
        // Add tile that was drawn into this match controller
        std::shared_ptr<TileSet::Tile> tileDrawn= _tileSet->processTileJson(_network->getTileDrawn())[0];
        std::string key = tileDrawn->toString() + " " + std::to_string(tileDrawn->_id);
        
        // If this match controller is host's
        if(isHost) {clientPlayer->getHand()._tiles.push_back(_tileSet->tileMap[key]);}
        // Else is client's
        else {hostPlayer->getHand()._tiles.push_back(_tileSet->tileMap[key]);}
        
        // Reset network status
        _network->setStatus(NetworkController::INGAME);
    }
    
    // TileMap Update
    if(_network->getStatus() == NetworkController::TILEMAPUPDATE) {
        // Remake pile
        if(_network->getMapUpdateType() == NetworkController::REMAKEPILE) {
            //Updating deck (all tile fields and removing used tiles from deck)
            _tileSet->updateDeck(_network->getTileMapJson());
            _pile->remakePile();
            _network->setMapUpdateType(NetworkController::NONE);
        }
        // Reset to default
        _network->setStatus(NetworkController::INGAME);
    }
    
    //Discard pile update
    if(_network->getStatus() == NetworkController::DISCARDUPDATE) {
        // Fetching discarded tile
        std::shared_ptr<TileSet::Tile> tile = _tileSet->processTileJson(_network->getDiscardTile())[0];
        std::string key = tile->toString() + " " + std::to_string(tile->_id);
        
        // Actual reference to tile from tileMap
        tile = _tileSet->tileMap[key];
        tile->inHostHand = false;
        tile->inClientHand = false;
        tile->discarded = true;
        
        //If host
        if(_network->getLocalPid() == 0) {clientPlayer->getHand().discard(tile, true);}
        //If client
        else{hostPlayer->getHand().discard(tile, false);}
        
        _discardPile->addTile(tile);
        
        //Change state so gamescene can update discardUI scene
        _choice = DISCARDUIUPDATE;
    }
}
