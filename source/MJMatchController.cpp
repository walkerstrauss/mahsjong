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
#include "MJAudioController.h"
#include "MJAnimationController.h"

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
 *  @returns true if init was successful, false otherwise
 */
bool MatchController::init(const std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<NetworkController>& network) {
    _assets = assets;
    _network = network;
    
    hostPlayer = std::make_shared<Player>();
    clientPlayer = std::make_shared<Player>();
    
    _tileSet = std::make_shared<TileSet>();
    _pile = std::make_shared<Pile>();
    _discardPile = std::make_shared<DiscardPile>();
    _discardPile->init(_assets);
    
    _choice = Choice::NONE;
    _celestialAnim = OpponentAnimType::INACTIVE;
    _active = true;
    
    // play the background music for the match scene.
    //AudioController::getInstance().init(_assets);
    //AudioEngine::start();
    AudioController::getInstance().playMusic("bgm", true);
    
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
    _tileSet->setAllTileTexture(_assets);
    _tileSet->initTileNodes(_assets);

    //Initializing host and client players
    hostPlayer->getHand().initHand(_tileSet, true);

    clientPlayer->getHand().initHand(_tileSet, false);
    
    //Initializing pile
    _pile->initPile(4, _tileSet, true, _assets);
        
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
    _tileSet->initTileNodes(_assets);
    _tileSet->updateDeck(_network->getClientStart());
    
    std::vector<std::shared_ptr<TileSet::Tile>> host;
    std::vector<std::shared_ptr<TileSet::Tile>> client;
    // Assigning client and host hands
    for(auto& pairs : _tileSet->tileMap) {
        std::shared_ptr<TileSet::Tile> currTile = pairs.second;
        if(currTile->inHostHand) {
            host.push_back(currTile);
        }
        else if(currTile->inClientHand) {
            client.push_back(currTile);
        }
    }
    
    hostPlayer->getHand()._tiles = hostPlayer->getHand().getSortedTiles(host);
    clientPlayer->getHand()._tiles = clientPlayer->getHand().getSortedTiles(client);

    //Initializing the pile
    _pile->initPile(4, _tileSet, false, _assets);
    _pile->setTilePositions(false);
    
    _pile->remakePile(false);
}

/**
 * Initializes the game in tutorial mode. Initializes the tileset based on a preset deck JSON
 * and instantiates only one player.
 */
void MatchController::initTutorial() {
    _network->connectAsTutorial();
    
    std::shared_ptr<JsonValue> tutorialDeck = _assets->get<JsonValue>("constants");
    _tileSet->deck = _tileSet->processDeckJson(tutorialDeck->get(0));
    for (auto& tile : _tileSet->deck) {
        _tileSet->tileMap[std::to_string(tile->_id)] = tile;
    }
    
    _tileSet->setAllTileTexture(_assets);
    _tileSet->initTileNodes(_assets);
    
    //Initializing tutorial player
    hostPlayer->getHand().initHand(_tileSet, true);
    
    //Initializing pile
    _pile->initPile(4, _tileSet, true, _assets);
    
    inTutorial = true; 
}

/**
 * Draws a tile from the pile to the player that called the method. After drawing, it broadcasts
 * the state of the pile and updates any tiles associated with the action performed
 */
void MatchController::drawTile() {
    bool isHost = _network->getHostStatus();
    auto& player = isHost ? hostPlayer : clientPlayer;
    
    if (hasDrawn) {
        return;
    }

    if (player->getHand()._tiles.size() <= player->getHand()._size) {
        player->getHand().drawFromPile(_pile, 1, isHost);
        AudioController::getInstance().playSound("Pile");
        hasDrawn = true;

        if (player->getHand().isWinningHand()) {
            _choice = Choice::WIN;
            _network->broadcastEnd(_network->getLocalPid());
            return;
        }
        
        // Broadcast the draw
        _network->broadcastTileDrawn(_network->getLocalPid(), _tileSet->toJson(_tileSet->tilesToJson));
        _tileSet->clearTilesToJson();

        // Remake pile if last tile was drawn
        if (_pile->getVisibleSize() == 0) {
            _pile->createPile();
            _network->broadcastTileMap(_network->getLocalPid(), _tileSet->mapToJson(), "remake pile");
        }
    }
}

/**
 *  Draws the most recently drawn tile from the discard pile. After drawing, it broadcasts that a
 *  tile has been drawn from the discard pile. However, for the sake of efficiency, the broadcasted
 *  message DOES NOT remove the discarded tile from the pile, rather it makes the tile invisible
 *  giving the illusion that it has been drawn.
 *
 * @returns     true if drawing from discard is successful
 */
bool MatchController::drawDiscard() {
    // If not this player's turn then return
    if(hasDrawn) {
        return false;
    }

    
    if(_network->getCurrentTurn() != _network->getLocalPid()) {
        return false;
    }
    
    // If there is no top tile then return
    if(!_discardPile->getTopTile()) {
        return false;
    }

    
    // Retrieving the current player
    std::shared_ptr<Player> currPlayer = _network->getHostStatus() ? hostPlayer : clientPlayer;
    // If the player's hand is too big return
    if(currPlayer->getHand()._tiles.size() > currPlayer->getHand()._size || hasDrawn) {
        return false;
    }
    
    // Getting top tile from dicsard and setting fields after drawing 
    std::shared_ptr<TileSet::Tile> drawnDiscardTile = _discardPile->drawTopTile();
    // Making tile unselectable
    drawnDiscardTile->selectable = false;
    // Automatically select
    drawnDiscardTile->selected = true;
    // Setting tile state
    drawnDiscardTile->inHostHand = _network->getHostStatus();
    drawnDiscardTile->inClientHand = _network->getHostStatus();
    
    AudioController::getInstance().playSound("Pile");

    // Putting tile in hand and automatically selecting it
    currPlayer->getHand()._tiles.push_back(drawnDiscardTile);
    currPlayer->getHand()._selectedTiles.push_back(drawnDiscardTile);
    
    if (currPlayer->getHand().isWinningHand()) {
        _choice = Choice::WIN;
        _network->broadcastEnd(_network->getLocalPid());
        return true;
    }
    
    _network->broadcastDrawnDiscard(_network->getLocalPid());
    
    _choice = DRAWNDISCARD;
    
    hasDrawn = true;
    
    return true; 
}

/**
 * Discards the currently dragged tile. After discarding, it broadcasts the newly discarded tile
 * to opposing player and updates the discarded pile element.
 *
 * @param tile      The tile that was discarded
 * @returns true if discard was successful, else false
 */
bool MatchController::discardTile(std::shared_ptr<TileSet::Tile> tile) {
    if(hasPlayedCelestial) {
        return false;
    }
    // If hand is required size, then discard
    bool isHost = _network->getHostStatus();
    if((isHost && hostPlayer->getHand()._tiles.size() > hostPlayer->getHand()._size) ||
            (!isHost && clientPlayer->getHand()._tiles.size() > clientPlayer ->getHand()._size)){
        // Setting fields to discarded
        tile->selected = false;
        tile->inHostHand = false;
        tile->inClientHand = false;
        tile->discarded = true;
        
        hasDiscarded = true;
        //If host
        if(isHost) {hostPlayer->getHand().discard(tile, true);}
        //If client
        else{clientPlayer->getHand().discard(tile, false);}
        
        // If not a special tile add it to the discard pile
        if (!(tile->_suit == TileSet::Tile::Suit::CELESTIAL)){
            AudioController::getInstance().playSound("Discard");

            _discardPile->addTile(tile);

            
            _tileSet->clearTilesToJson();
            // Converting to JSON and broadcasting discarded tile
            _tileSet->tilesToJson.push_back(tile);
            _network->broadcastDiscard(_network->getLocalPid(), _tileSet->toJson(_tileSet->tilesToJson));
            _tileSet->clearTilesToJson();
            
            hasDiscarded = true;
            endTurn();
            
            return true;
        }
        
        hasDiscarded = true;
        endTurn();
    }
    return false; 
}

/**
 * Plays the currently selected tiles as a set for this player.
 *
 * @return  true if playing set was successful, false otherwise
 */
bool MatchController::playSet() {
    // Retrieving the current player
    std::shared_ptr<Player> currPlayer = _network->getHostStatus() ? hostPlayer : clientPlayer;
    std::shared_ptr<Player> opponentPlayer = _network->getHostStatus() ? clientPlayer : hostPlayer;
    
    std::vector<std::shared_ptr<TileSet::Tile>> tiles;
    _tileSet->clearTilesToJson();

    // If selected tiles is a valid set
    if(currPlayer->getHand().isSetValid(currPlayer->getHand()._selectedTiles)) {
        // Accumulate tiles from selected set to transform into JSON
        AudioController::getInstance().playSound("PlayedSet");
        for(auto const& tile : currPlayer->getHand()._selectedTiles) {
            _tileSet->tilesToJson.push_back(tile);
        }
        // Played tile JSON
        std::shared_ptr<JsonValue> tilesJson = _tileSet->toJson(_tileSet->tilesToJson);
        _tileSet->clearTilesToJson();
        
        // Broadcast that a successful set has been played
        currPlayer->getHand().playSet(_network->getHostStatus());
       
        _network->broadcastPlaySet(_network->getLocalPid(), true, tilesJson);
    
        // Reset choice for match controller
        _choice = NONE;
        return true;
    }
    else {
        AudioController::getInstance().playSound("WrongAction");
        // Unselect all selected tiles from hand
        for(auto it = currPlayer->getHand()._tiles.begin(); it != currPlayer->getHand()._tiles.end();) {
            (*it)->selected = false;
            
            // If the tile was a discarded tile reset to discarded status and discard from hand
            if((*it)->discarded) {
                (*it)->inHostHand = false;
                (*it)->inClientHand = false;
                (*it)->selectable = false;
                
                (*it)->_scale = 0;
                (*it)->pos = Vec2::ZERO;
                
                _discardPile->addTile(*it);
                currPlayer->getHand().removeTile((*it), _network->getHostStatus());
            } else {
                it++;
            }
        }
        // Clear selected tiles from current player
        currPlayer->getHand()._selectedTiles.clear();
        
        _tileSet->clearTilesToJson();

        // Make empty JSON for broadcasting
        std::shared_ptr<JsonValue> emptyJson = _tileSet->toJson(_tileSet->tilesToJson);
        _network->broadcastPlaySet(_network->getLocalPid(), false, emptyJson);
        
        
        hasDrawn = false;
        // Reset match controller choice
        _choice = NONE;
        return false;
    }
    
    return false;
}

/**
 * Plays the inputted celestial tile, if tile is not a celestial tile then the function returns false.
 * Calls the appropriate function according to the celestial tile type. It then calls the appropriate
 * callback function.
 *
 * @param celestialTile     The celestial tile that was played
 * @returns True if celestial was played, false if not
 */
bool MatchController::playCelestial(std::shared_ptr<TileSet::Tile>& celestialTile) {
    if(_network->getHostStatus() && hostPlayer->getHand()._tiles.size() <= hostPlayer->getHand()._size) {
        return false;
    }
    else if(!_network->getHostStatus() && clientPlayer->getHand()._tiles.size() <= clientPlayer->getHand()._size) {
        return false;
    }
    
    if(!hasPlayedCelestial) {
        // Checking if tile is valid celestial
        TileSet::Tile::Suit suit = celestialTile->_suit;
        TileSet::Tile::Rank rank = celestialTile->_rank;
        if(suit != TileSet::Tile::Suit::CELESTIAL || celestialTile->debuffed) { // Do not allow debuffed celestial tiles to be played
            return false;
        } else if (rank == TileSet::Tile::Rank::PIG && _discardPile->getSize() < 1) { // Do not allow pig tile to be played if no discarded tiles
            return false;
        }
        // Execute appropriate callback
        else{
            // Discarding celestial tile from appropriate player hand
            if(_network->getHostStatus()) {hostPlayer->getHand().discard(celestialTile, true);}
            else{clientPlayer->getHand().discard(celestialTile, false);}
            
            switch(rank) {
                // Rooster celestial tile
                case(TileSet::Tile::Rank::ROOSTER):
                    AudioController::getInstance().playSound("Rooster", false);
                    playRooster(celestialTile);
                    break;
                case(TileSet::Tile::Rank::OX):
                    AudioController::getInstance().playSound("Ox", false);
                    playOx(celestialTile);
                    break;
                case(TileSet::Tile::Rank::RABBIT):
                    AudioController::getInstance().playSound("Rabbit", false);
                    playRabbit(celestialTile);
                    break;
                case(TileSet::Tile::Rank::SNAKE):
                    AudioController::getInstance().playSound("Snake", false);
                    playSnake(celestialTile);
                    break;
                case(TileSet::Tile::Rank::MONKEY):
                    AudioController::getInstance().playSound("Monkey", false);
                    _monkeyTile = celestialTile;
                    _choice = MONKEYTILE;
                    break;
                case(TileSet::Tile::Rank::RAT):
                    AudioController::getInstance().playSound("Rat", false);
                    _ratTile = celestialTile;
                    _choice = RATTILE;
                    break;
                case(TileSet::Tile::Rank::DRAGON):
                    AudioController::getInstance().playSound("Dragon", false);
                    _dragonTile = celestialTile;
                    _choice = DRAGONTILE;
                    break;
                case(TileSet::Tile::Rank::PIG):
                    _pigTile = celestialTile;
                    _choice = PIGTILE;
                    break;
                // Numbered rank
                default:
                    break;
            }
            return true;
        }
    }
    return false;
}

/**
 * Executes Rooster celestial tile effect in current game scene. It then broadcasts the change
 * to opposing player.
 */
void MatchController::playRooster(std::shared_ptr<TileSet::Tile>& celestialTile){
    // Reshuffle current player's pile
    // play the shuffle sound.
    AudioController::getInstance().playSound("shuffle");
    _pile->reshufflePile();
    _pile->setTilePositions(true);
    
    // Clear tilesToJson vector
    _tileSet->clearTilesToJson();
    // Transforming celestial tile to JSON
    _tileSet->tilesToJson.push_back(celestialTile);
    const std::shared_ptr<cugl::JsonValue> celestialTileJson = _tileSet->toJson(_tileSet->tilesToJson);
    // Clear tilesToJson vector
    _tileSet->clearTilesToJson();
    
    // Broadcast celestial tile
    _network->broadcastCelestialTile(_network->getLocalPid(), _tileSet->toJson(_pile->flattenedPile()), celestialTileJson, "ROOSTER");
    // Clear tilesToJson vector
    _tileSet->clearTilesToJson();
    hasPlayedCelestial = true;
    endTurn();
    return;
}

/**
 * Executes Ox celestial tile effect in current game scene. It then broadcasts the change
 * to opposing player.
 */
void MatchController::playOx(std::shared_ptr<TileSet::Tile>& celestialTile) {
    auto& opponent = _network->getHostStatus()
                         ? clientPlayer->getHand()
                         : hostPlayer->getHand();
    
    opponent.rdHand.init();
    opponent.rdHand.shuffle(opponent._tiles);
    
    
    // Makes sure that effect is applied on tiles that aren't already debuffed or discarded
    int debuffed = 0;
    _tileSet->clearTilesToJson();

    std::shared_ptr<cugl::JsonValue> changedTilesJson;
    for (auto& tile : opponent._tiles) {
        if (!tile->debuffed && !tile->discarded) {
            tile->_scale = 0.325;
            tile->debuffed = true;
            tile->getFaceSpriteNode()->setVisible(false);
            _tileSet->tilesToJson.push_back(tile);
            debuffed++;
        }
        if (debuffed == 2) break;
    }
    
    changedTilesJson = _tileSet->toJson(_tileSet->tilesToJson);
    // Clear tilesToJson vector
    _tileSet->clearTilesToJson();
    
    // Transforming celestial tile to JSON
    _tileSet->tilesToJson.push_back(celestialTile);
    const std::shared_ptr<cugl::JsonValue> celestialTileJson = _tileSet->toJson(_tileSet->tilesToJson);
    // Clear tilesToJson vector
    _tileSet->clearTilesToJson();
    
    // Broadcast celestial tile
    _network->broadcastCelestialTile(_network->getLocalPid(), changedTilesJson, celestialTileJson, "OX");
    // Clear tilesToJson vector
    _tileSet->clearTilesToJson();
    
    hasPlayedCelestial = true;
    endTurn();
    
    return;
}

/**
 * Executes Rabbit celestial tile effect in current game scene. It then broadcasts the change
 * to opposing player.
 */
void MatchController::playRabbit(std::shared_ptr<TileSet::Tile>& celestialTile){
    auto& opponent = _network->getHostStatus()
                         ? clientPlayer->getHand()
                         : hostPlayer->getHand();
    
    opponent.rdHand.init();
    opponent.rdHand.shuffle(opponent._tiles);
    
    std::shared_ptr<cugl::JsonValue> changedTileJson;
    for (auto& tile : opponent._tiles) {
        // Make sure we only try to change rank of non-celestial and non-discarded tiles
        if (!tile->discarded && tile->_suit != TileSet::Tile::Suit::CELESTIAL && !tile->debuffed) {
            int oldRank = static_cast<int>(tile->_rank);
            int newRank = oldRank;
            
            while (newRank == oldRank) {
                newRank = 1 + rand() % 9;
            }
            tile->_scale = 0.325;
            tile->_rank = static_cast<TileSet::Tile::Rank>(newRank);
            tile->setFaceTexture(_assets->get<Texture>(tile->toString() + " new"));
            _tileSet->tilesToJson.push_back(tile);
            changedTileJson = _tileSet->toJson(_tileSet->tilesToJson);
            // Clear tilesToJson vector
            _tileSet->clearTilesToJson();
            break;
        }
    }
    
    // Transforming celestial tile to JSON
    _tileSet->tilesToJson.push_back(celestialTile);
    const std::shared_ptr<cugl::JsonValue> celestialTileJson = _tileSet->toJson(_tileSet->tilesToJson);
    // Clear tilesToJson vector
    _tileSet->clearTilesToJson();
    
    // Broadcast celestial tile
    _network->broadcastCelestialTile(_network->getLocalPid(), changedTileJson, celestialTileJson, "RABBIT");

    // Clear tilesToJson vector
    _tileSet->clearTilesToJson();
    
    hasPlayedCelestial = true;
    endTurn();
    
    return;
}

/**
 * Executes the Snake celestial tile effect  (change suit of random tile) in current game scene. It then broadcasts the change
 * to opposing player.
 */
void MatchController::playSnake(std::shared_ptr<TileSet::Tile>& celestialTile){
    auto& opponent = _network->getHostStatus()
                         ? clientPlayer->getHand()
                         : hostPlayer->getHand();
    
    opponent.rdHand.init();
    opponent.rdHand.shuffle(opponent._tiles);
    
    std::shared_ptr<cugl::JsonValue> changedTileJson;
    for (auto& tile : opponent._tiles) {
        // Make sure we only try to change rank of non-celestial and non-discarded tiles
        if (!tile->discarded && tile->_suit != TileSet::Tile::Suit::CELESTIAL && !tile->debuffed) {
            int oldSuit = static_cast<int>(tile->_suit);
            int newSuit = oldSuit;
            
            while (newSuit == oldSuit) {
                newSuit = 1 + rand() % 3;
            }
            tile->_scale = 0.325;
            tile->_suit = static_cast<TileSet::Tile::Suit>(newSuit);
            tile->setFaceTexture(_assets->get<Texture>(tile->toString() + " new"));
            _tileSet->tilesToJson.push_back(tile);
            changedTileJson = _tileSet->toJson(_tileSet->tilesToJson);
            // Clear tilesToJson vector
            _tileSet->clearTilesToJson();
            break;

        }
    }
    
    // Transforming celestial tile to JSON
    _tileSet->tilesToJson.push_back(celestialTile);
    const std::shared_ptr<cugl::JsonValue> celestialTileJson = _tileSet->toJson(_tileSet->tilesToJson);
    // Clear tilesToJson vector
    _tileSet->clearTilesToJson();
    
    // Broadcast celestial tile
    _network->broadcastCelestialTile(_network->getLocalPid(), changedTileJson, celestialTileJson, "SNAKE");
    // Clear tilesToJson vector
    _tileSet->clearTilesToJson();
    
    hasPlayedCelestial = true;
    endTurn();
    
    return;
}

/**
 * Executes the Monkey celestial tile effect (trade tiles) given the selected tile by the player. It will give the selected tile to the opponent and then take a random tile from them.
 */
void MatchController::playMonkey(std::shared_ptr<TileSet::Tile>& selectedTile) {
    auto& self = _network->getHostStatus()
                         ? hostPlayer->getHand()
                         : clientPlayer->getHand();
    
    auto& opponent = _network->getHostStatus()
                         ? clientPlayer->getHand()
                         : hostPlayer->getHand();
    
    // Remove selected tile from hand
    self.removeTile(selectedTile, _network->getHostStatus());
    
    // Get the random tile from the opponent and remove it from their hand (can be a debuffed or celestial tile)
    opponent.rdHand.init();
    opponent.rdHand.shuffle(opponent._tiles);
    std::shared_ptr<TileSet::Tile>& oppTile = opponent._tiles.front();
    opponent.removeTile(oppTile, _network->getHostStatus());
    
    // Add the selected tile to opponent hand
    opponent._tiles.push_back(selectedTile);
    selectedTile->inHostHand = !_network->getHostStatus();
    selectedTile->inClientHand = _network->getHostStatus();
    
    // Add the opponent tile to your own hand
    self._tiles.push_back(oppTile);
    oppTile->inHostHand = _network->getHostStatus();
    oppTile->inClientHand = !_network->getHostStatus();
    
    if (self.isWinningHand()) {
        _choice = Choice::WIN;
        _network->broadcastEnd(_network->getLocalPid());
        return;
    }

    // Add the swapped tiles to tileSet and turn into JSON
    _tileSet->tilesToJson.push_back(selectedTile);
    _tileSet->tilesToJson.push_back(oppTile);
    const std::shared_ptr<cugl::JsonValue> changedTilesJson = _tileSet->toJson(_tileSet->tilesToJson);
    // Clear tilesToJson vector
    _tileSet->clearTilesToJson();
    
    // Transforming celestial tile to JSON
    _tileSet->tilesToJson.push_back(_monkeyTile);
    const std::shared_ptr<cugl::JsonValue> celestialTileJson = _tileSet->toJson(_tileSet->tilesToJson);
    // Clear tilesToJson vector
    _tileSet->clearTilesToJson();
    
    // Broadcast celestial tile
    _network->broadcastCelestialTile(_network->getLocalPid(), changedTilesJson , celestialTileJson, "MONKEY");
    // Clear tilesToJson vector
    _tileSet->clearTilesToJson();
    
    hasPlayedCelestial = true;
    endTurn();
    
    return;
}

/**
 * Executes the Rat celestial tile effect (draw any tile from pile) given the selected tile by the player. It will remove the tile from the pile and add it to player's hand.
 */
void MatchController::playRat(std::shared_ptr<TileSet::Tile>& selectedTile) {
    auto& self = _network->getHostStatus()
                         ? hostPlayer->getHand()
                         : clientPlayer->getHand();
    
    _pile->removeTile(selectedTile);
    _pile->setTilePositions(false);

    self._tiles.push_back(selectedTile);
    selectedTile->inHostHand = _network->getHostStatus();
    selectedTile->inClientHand = !_network->getHostStatus();
    selectedTile->inPile = false;
    selectedTile->selected = false;
    selectedTile->_scale = 0.325;
    
    if (self.isWinningHand()) {
        _choice = Choice::WIN;
        _network->broadcastEnd(_network->getLocalPid());
        return;
    }
    
    // Clear tilesToJson vector
    _tileSet->clearTilesToJson();
    // Transforming celestial tile to JSON
    _tileSet->tilesToJson.push_back(_ratTile);
    const std::shared_ptr<cugl::JsonValue> celestialTileJson = _tileSet->toJson(_tileSet->tilesToJson);
    // Clear tilesToJson vector
    _tileSet->clearTilesToJson();
    
    _tileSet->tilesToJson.push_back(selectedTile);
    const std::shared_ptr<cugl::JsonValue> selectedTileJson = _tileSet->toJson(_tileSet->tilesToJson);
    // Clear tilesToJson vector
    _tileSet->clearTilesToJson();
    
    // Broadcast celestial tile
    _network->broadcastCelestialTile(_network->getLocalPid(), selectedTileJson, celestialTileJson, "RAT");
    // Clear tilesToJson vector
    _tileSet->clearTilesToJson();
    
    if (_pile->getVisibleSize() == 0) {
        _pile->createPile();
        _network->broadcastTileMap(_network->getLocalPid(), _tileSet->mapToJson(), "remake pile");
    }
    
    return;
}

void MatchController::playDragon() {
    // Clear tilesToJson vector
    _tileSet->clearTilesToJson();
    // Transforming celestial tile to JSON
    _tileSet->tilesToJson.push_back(_dragonTile);
    const std::shared_ptr<cugl::JsonValue> celestialTileJson = _tileSet->toJson(_tileSet->tilesToJson);
    // Clear tilesToJson vector
    _tileSet->clearTilesToJson();
    
    // Broadcast celestial tile
    _network->broadcastCelestialTile(_network->getLocalPid(), _tileSet->toJson(_pile->flattenedPile()), celestialTileJson, "DRAGON");
    // Clear tilesToJson vector
    _tileSet->clearTilesToJson();
    
    hasPlayedCelestial = true;
    endTurn();
    
    return;
}

void MatchController::playPig(std::pair<TileSet::Tile::Suit, TileSet::Tile::Rank> info) {
    auto& self = _network->getHostStatus()
                         ? hostPlayer->getHand()
                         : clientPlayer->getHand();
    // Find the tile that the player clicked on
    std::shared_ptr<TileSet::Tile> selectedTile = _discardPile->findTile(info);
    // Remove it from discard pile
    _discardPile->removeTile(selectedTile);
    // Add it to hand
    self._tiles.push_back(selectedTile);
    
    // Update information
    selectedTile->inHostHand = _network->getHostStatus();
    selectedTile->inClientHand = !_network->getHostStatus();
    selectedTile->inPile = false;
    selectedTile->selected = false;
    selectedTile->discarded = false;
    selectedTile->selectable = true;
    selectedTile->_scale = 0.325;
    
    if (self.isWinningHand()) {
        _choice = Choice::WIN;
        _network->broadcastEnd(_network->getLocalPid());
        return;
    }
    
    // Clear tilesToJson vector
    _tileSet->clearTilesToJson();
    // Transforming celestial tile to JSON
    _tileSet->tilesToJson.push_back(_pigTile);
    const std::shared_ptr<cugl::JsonValue> celestialTileJson = _tileSet->toJson(_tileSet->tilesToJson);
    // Clear tilesToJson vector
    _tileSet->clearTilesToJson();
    
    _tileSet->tilesToJson.push_back(selectedTile);
    const std::shared_ptr<cugl::JsonValue> selectedTileJson = _tileSet->toJson(_tileSet->tilesToJson);
    // Clear tilesToJson vector
    _tileSet->clearTilesToJson();
    
    // Broadcast celestial tile
    _network->broadcastCelestialTile(_network->getLocalPid(), selectedTileJson, celestialTileJson, "PIG");
    // Clear tilesToJson vector
    _tileSet->clearTilesToJson();
    
    return;
}


void MatchController::celestialEffect(){

    if (_network->getCelestialUpdateType() == NetworkController::ROOSTER
        || _network->getCelestialUpdateType() == NetworkController::DRAGON) {

        if (_network->getCelestialUpdateType() == NetworkController::ROOSTER) {
            AudioController::getInstance().playSound("Rooster");
            _celestialAnim = ROOSTER;

        }
        else if (_network->getCelestialUpdateType() == NetworkController::DRAGON) {
            AudioController::getInstance().playSound("Dragon");
            _celestialAnim = DRAGON;
        }

        //Updating tileset
        _tileSet->updateDeck(_network->getTileMapJson());
        bool reshuffle = _network->getCelestialUpdateType() == NetworkController::ROOSTER ? true : false;
        _pile->remakePile(reshuffle);
    } else if (_network->getCelestialUpdateType() == NetworkController::RAT) {
        AudioController::getInstance().playSound("Rat");
        bool isHost = _network->getHostStatus();
        
        // Add tile that was drawn into this match controller
        std::shared_ptr<TileSet::Tile> tileDrawn= _tileSet->processTileJson(_network->getTileDrawn())[0];
        std::string key = std::to_string(tileDrawn->_id);
        
        _pile->removeTile(_tileSet->tileMap[key]);
        _pile->setTilePositions(false);
        _tileSet->tileMap[key]->_scale = 0.325;
        
        // If this match controller is host's
        if(isHost) {clientPlayer->getHand()._tiles.push_back(_tileSet->tileMap[key]);}
        // Else is client's
        else {hostPlayer->getHand()._tiles.push_back(_tileSet->tileMap[key]);}
        _celestialAnim = RAT;
    } else if (_network->getCelestialUpdateType() == NetworkController::OX) { // these alter your hand, so must update textures
        AudioController::getInstance().playSound("Ox");
        _tileSet->updateDeck(_network->getTileMapJson());
        auto& hand = _network->getHostStatus()
                            ? hostPlayer->getHand()
                            : clientPlayer->getHand();
        
        hand.updateHandTextures(_assets);
        _celestialAnim = OX;
    } else if (_network->getCelestialUpdateType() == NetworkController::RABBIT ||
               _network->getCelestialUpdateType() == NetworkController::SNAKE) {
        if (_network->getCelestialUpdateType() == NetworkController::RABBIT) {
            AudioController::getInstance().playSound("Rabbit");
            _celestialAnim = RABBIT;
        }
        else if (_network->getCelestialUpdateType() == NetworkController::SNAKE) {
            AudioController::getInstance().playSound("Snake");
            _celestialAnim = SNAKE;
        }

        std::vector<std::shared_ptr<TileSet::Tile>> tilesToAnimate = _tileSet->processTileJson(_network->getTileMapJson());
        
        for(auto& it : tilesToAnimate) {
            std::shared_ptr<TileSet::Tile> tile = _tileSet->tileMap[std::to_string(it->_id)];
            std::shared_ptr<graphics::Texture> fromTexture = _assets->get<graphics::Texture>(tile->toString() + " sheet");
            std::shared_ptr<graphics::Texture> toTexture = _assets->get<graphics::Texture>(it->toString() + " sheet");
            std::shared_ptr<graphics::Texture> idle = _assets->get<graphics::Texture>(it->toString() + " new");
            AnimationController::getInstance().animateTileMorph(tile, fromTexture, toTexture, idle, 20.0f);
            AnimationController::getInstance().animateBounceEffect(tile, 0.2f);  
            tile->setTexture(_assets->get<graphics::Texture>(it->toString()));
            }
        
        _tileSet->updateDeck(_network->getTileMapJson());
        
    } else if (_network->getCelestialUpdateType() == NetworkController::MONKEY) {
        AudioController::getInstance().playSound("Monkey");
        _tileSet->updateDeck(_network->getTileMapJson());
        
        std::vector<std::shared_ptr<TileSet::Tile>> changedTiles = _tileSet->processTileJson(_network->getTileMapJson());
        for (auto& change : changedTiles) {
            auto tile = _tileSet->tileMap[std::to_string(change->_id)];
            auto& host = hostPlayer->getHand();
            auto& client = clientPlayer->getHand();

            // Add tile to hand based on flags
            if (tile->inHostHand) {
                host._tiles.push_back(tile);
                client.removeTile(tile, false);
            } else if (tile->inClientHand) {
                client._tiles.push_back(tile);
                host.removeTile(tile, true);
            }
            
            auto& hand = _network->getHostStatus()
                                ? hostPlayer->getHand()
                                : clientPlayer->getHand();
            
            hand.updateHandTextures(_assets);
            
        }
        _celestialAnim = MONKEY;
    } else if (_network->getCelestialUpdateType() == NetworkController::PIG) {
        bool isHost = _network->getHostStatus();
        
        // Add tile that was drawn into this match controller
        std::shared_ptr<TileSet::Tile> tileDrawn= _tileSet->processTileJson(_network->getTileDrawn())[0];
        std::string key = std::to_string(tileDrawn->_id);
        
        _discardPile->removeTile(_tileSet->tileMap[key]);
        _tileSet->tileMap[key]->_scale = 0.325;
        
        // If this match controller is host's
        if(isHost) {clientPlayer->getHand()._tiles.push_back(_tileSet->tileMap[key]);}
        // Else is client's
        else {hostPlayer->getHand()._tiles.push_back(_tileSet->tileMap[key]);}
        
        _choice = DISCARDUIUPDATE;
        _celestialAnim = PIG;
    }
    _network->setCelestialUpdateType(NetworkController::NONE);
}


/**
 * Call back for ending the turn for the current player. Must have drawn from the pile
 * and dicsarded/played a tile in order to end turn successfully. Resets the current turn
 * requirements with a sucessful end.
 */
void MatchController::endTurn() {
    // If it is this player's turn
    if(_network->getCurrentTurn() == _network->getLocalPid() && !inTutorial) {
        // If satisfied turn requirements n
        if((hasDrawn || hasTimedOut) && (hasPlayedCelestial || hasDiscarded || hasTimedOut)) {
            // If host
            if(_network->getHostStatus() && hostPlayer->getHand()._tiles.size() == hostPlayer->getHand()._size) {
                _network->endTurn();
            }
            // If client
            else if(!_network->getHostStatus() && clientPlayer->getHand()._tiles.size() == clientPlayer->getHand()._size) {
                _network->endTurn();
            }
        }
        resetTurn();
    }
}

/** Handles game win by broadcasting to opponent that they have a full mahjong hand  */
void MatchController::handleGameWin(){
    if (_network->getCurrentTurn() == _network->getLocalPid()){
        _network->broadcastEnd(_network->getHostStatus());
        _choice = WIN;
    }
}


/**
 * The method called to update the game mode
 *
 * @param timestep The amount of time (in seconds) since the last frame
 */
void MatchController::update(float timestep) {
    AnimationController::getInstance().update(timestep);
    // If we receieve end game status, current player loses
    if(_network->getStatus() == NetworkController::ENDGAME) {
        _choice = LOSE;
    }
    
    // Tile has been drawn from the pile 
    if(_network->getStatus() == NetworkController::TILEDRAWN) {
        setChoice(PILEDRAW);
        bool isHost = _network->getHostStatus();
        
        _pile->removePileTile(_network->getTileDrawn(), !isHost);
        
        // Add tile that was drawn into this match controller
        std::shared_ptr<TileSet::Tile> tileDrawn= _tileSet->processTileJson(_network->getTileDrawn())[0];
        std::string key = std::to_string(tileDrawn->_id);
        
        _tileSet->tileMap[key]->_scale = 0.325;
        
        // If this match controller is host's
        if(isHost) {clientPlayer->getHand()._tiles.push_back(_tileSet->tileMap[key]);}
        // Else is client's
        else {hostPlayer->getHand()._tiles.push_back(_tileSet->tileMap[key]);}
        
        // Reset network status
        _network->setStatus(NetworkController::INGAME);
    }
    
    // TileMap Update
    if(_network->getStatus() == NetworkController::TILEMAPUPDATE) {
        //Updating deck (all tile fields and removing used tiles from deck)
        _tileSet->updateDeck(_network->getTileMapJson());
        // Map update type
        NetworkController::MapUpdateType mapUpdateType = _network->getMapUpdateType();
        // Remake pile
        if(mapUpdateType == NetworkController::REMAKEPILE) {
            _pile->remakePile(false);
            _network->setMapUpdateType(NetworkController::NOUPDATE);
        }
        // Reset to default
        _network->setStatus(NetworkController::INGAME);
    }
    
    //Discard pile update
    if(_network->getStatus() == NetworkController::DISCARDUPDATE) {
        // Fetching discarded tile
        std::shared_ptr<TileSet::Tile> tile = _tileSet->processTileJson(_network->getDiscardTile())[0];
        _tileSet->updateDeck(_network->getDiscardTile());
        std::string key = std::to_string(tile->_id);
                
        tile = _tileSet->tileMap[key];
        
        //If host
        if(_network->getHostStatus()) {clientPlayer->getHand().discard(tile, true);}
        //If client
        else{hostPlayer->getHand().discard(tile, false);}
        
        _discardPile->addTile(tile);
        
        // Change state so gamescene can update discardUI scene
        _choice = DISCARDUIUPDATE;
        
        _network->setStatus(NetworkController::INGAME);
    }
    
    //Celestial tile played update
    if(_network->getStatus() == NetworkController::PLAYEDCELESTIAL) {
        //Retrieves celestial tile that was played
        std::shared_ptr<TileSet::Tile> celestialTile = _tileSet->processTileJson(_network->getCelestialTile())[0];
        std::string key = std::to_string(celestialTile->_id);
        
        celestialTile->inHostHand = false;
        celestialTile->inClientHand = false;
        celestialTile->discarded = true;
        
        //Apply the effect of the played celestial tile
        celestialEffect();
        
        //Updating player hand
        //If host
        if(_network->getLocalPid() == 0) {clientPlayer->getHand().discard(celestialTile, true);}
        //If client
        else{hostPlayer->getHand().discard(celestialTile, false);}
        _network->setStatus(NetworkController::INGAME);
    }
    
    // If set that has been played was successful
    if(_network->getStatus() == NetworkController::SUCCESSFULSET) {
        // Retrieving the opposing player
        std::shared_ptr<Player> opposingPlayer = _network->getHostStatus() ? clientPlayer : hostPlayer;
        std::shared_ptr<Player> currPlayer = _network->getHostStatus() ? hostPlayer : clientPlayer;
        // Fetching the top tile
        std::shared_ptr<TileSet::Tile> discardTile = _discardPile->drawTopTile();
        
         _tileSet->updateDeck(_network->getPlayedTiles());

         std::vector<std::shared_ptr<TileSet::Tile>> tiles = _tileSet->processTileJson(_network->getPlayedTiles());
       

        for(auto const& tile : tiles) {
            std::string id = std::to_string(tile->_id);
            tile->setTexture(_assets->get<Texture>(tile->toString()));
            opposingPlayer->getHand().removeTile(tile, _network->getHostStatus());
        }
        
        // Update opposing player's max hand size
        opposingPlayer->getHand().playSet(!_network->getHostStatus());
        currPlayer->getHand().opponentPlayedSets.push_back(tiles);
        
        
        // Reset network state
        _network->setStatus(NetworkController::INGAME);
        // Indicate to gamescene to deactivate button
        _choice = SUCCESS_SET;
    }
    
    // If played set was unsuccessful
    if(_network->getStatus() == NetworkController::UNSUCCESSFULSET) {
        //Reset network state
        _network->setStatus(NetworkController::INGAME);
        // Indicate to gamescene to deactivate button
        _choice = FAILED_SET;
    }
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void MatchController::dispose() {
    if(_active) {
        _tileSet = nullptr;
        _pile = nullptr;
        _discardPile = nullptr;
        hostPlayer = nullptr;
        clientPlayer = nullptr; 
        // end the background music for the matchscene.
        AudioController::getInstance().stopMusic();
        if(_network) {
            _network->disconnect();
        }
        _active = false;
    }
}
