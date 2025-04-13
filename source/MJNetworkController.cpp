//
//  MJNetworkController.cpp
//  Mahsjong
//
//  Created by Patrick Choo on 3/5/25.
//

#include <cugl/cugl.h>
#include <vector>

#include "MJNetworkController.h"

using namespace cugl;
using namespace cugl::netcode;

NetworkController::NetworkController() {
    _status = IDLE;
    _mapUpdateType = NOUPDATE;
    _celestialUpdateType = NONE; 
    _isHost = false;
    _roomid = "";
    _currentTurn = 0;
    _localPid = -1;
    _serializer = cugl::netcode::NetcodeSerializer::alloc();
    _deserializer = cugl::netcode::NetcodeDeserializer::alloc();
};


/**
 * Disposes of all (non-static) resources allocated to this controller.
 */
void NetworkController::dispose() {
    disconnect();
    _serializer->~NetcodeSerializer();
    _deserializer->~NetcodeDeserializer();
    _assets = nullptr;
}

bool NetworkController::init(const std::shared_ptr<cugl::AssetManager>& assets) {
    _assets = assets;
    
    auto json = assets->get<JsonValue>("server");
    _config.set(json);
    _status = Status::IDLE;
    
    return true;
}

void NetworkController::update(float timestep){
    if(_network) {
        _network->receive([this](const std::string source,
                                 const std::vector<std::byte>& data) {
            processData(source, data);
        });
        checkConnection();
    }
}

bool NetworkController::connectAsHost() {
    if (_status == Status::NETERROR) {
        disconnect();
    }

    _isHost = true;
    _localPid = 0;
    if (_status == Status::IDLE) {
        _status = Status::CONNECTING;
        _network = cugl::netcode::NetcodeConnection::alloc(_config);
        _network->open();
    }
    return checkConnection();
}

bool NetworkController::connectAsClient(std::string room) {
    if (_status == Status::NETERROR) {
        disconnect();
    }

    _isHost = false;
    _localPid = 1;
    if (_status == Status::IDLE) {
        _status = Status::CONNECTING;
        _network = cugl::netcode::NetcodeConnection::alloc(_config, room);
        _network->open();
    }
    _roomid = room;
    return checkConnection();
    
}

/**
 * Disconnects from the network.
 */
void NetworkController::disconnect() {
    if(_network && _network->isOpen()) {
        _network->close();
    }
    _network = nullptr;
    _status = Status::IDLE;
    
}

void NetworkController::processData(const std::string source,
                                    const std::vector<std::byte>& data){
    _deserializer->reset();
    _deserializer->receive(data);
    std::string msgType = _deserializer->readString();
    
    // Game start for host
    if (msgType == "start game") {
        _status = START;
    }
    // End turn for player
    else if (msgType == "end turn") {
        _currentTurn = _deserializer->readUint32();
    }
    // Start client's game
    else if (msgType == "client start") {
        _clientStart = _deserializer->readJson();
        _status = INGAME;
    }
    int isHost = _deserializer->readUint32();
    // Messages only for the opposing player
    if (_localPid != isHost) {
        // Opponent drew a tile
        if (msgType == "tile drawn") {
            _tileDrawn = _deserializer->readJson();
            _status = TILEDRAWN;
        }
        
        // Update to tile map
        if (msgType == "tile map update") {
            _tileMapJson = _deserializer->readJson();
            std::string mapUpdateType = _deserializer->readString();
            // Remaking pile
            if(mapUpdateType == "remake pile"){
                _mapUpdateType = REMAKEPILE;
            }
            _status = TILEMAPUPDATE;
        }
        // Update to discard pile
        if (msgType == "discard update") {
            _discardTile = _deserializer->readJson();
            _status = DISCARDUPDATE; 
        }
        
        // Celestial tile has been played
        if (msgType == "celestial tile played") {
            std::string celestialType = _deserializer->readString();
            if(celestialType == "CHAOS") {
                _celestialTile = _deserializer->readJson();
                _celestialUpdateType = CHAOS;
            }
            _status = PLAYEDCELESTIAL;
        }
        // Opponent won game
        if(msgType == "game concluded") {
            _status = ENDGAME;
        }
        //
    }
//    else if (msgType == "initialize game") {
//        _startingDeckJson = _deserializer->readJson();
//    }
//    else if (msgType == "starting client deck") {
//        _deckJson = _deserializer->readJson();
//        _status = INGAME;
//    }
//    else if (msgType == "update deck") {
//        _deckJson = _deserializer->readJson();
//        _status = DECK;
//    }
//    else if (msgType == "pile tile update") {
//        _pileTileJson = _deserializer->readJson();
//        _isHostDraw = _isHost;
//        _status = PILETILEUPDATE;
//    }
//    else if (msgType == "update layer") {
//        _status = LAYER;
//    }
//    else if (msgType == "remove discard tile"){
//        _status = REMOVEDISCARD;
//    }
//    else if (msgType == "new discard tile") {
//        _discardTile = _deserializer->readJson();   
//        _status = NEWDISCARD;
//    }
//    else if (msgType == "tile map update") {
//        _tileMapJson = _deserializer->readJson();
//    }
//    else if (msgType == "preemptive draw") {
//        int numToDraw = _deserializer->readUint32();
//        bool isHost = _deserializer->readBool();
//        _numDiscard = std::tuple<int, bool>(numToDraw, isHost);
//        _status = PREEMPTIVEDISCARD;
//    }
}

void NetworkController::endTurn() {
    _currentTurn = (_currentTurn == 0) ? 1 : 0;  // Toggle between 0 and 1
    
    _serializer->reset();
    _serializer->writeString("end turn");
    _serializer->writeUint32(_currentTurn);
    broadcast(_serializer->serialize());
}

void NetworkController::transmitSingleTile(TileSet::Tile& tile){
    
    //Process tile name and id
    _serializer->writeString(tile.toString());
    _serializer->writeString(std::to_string(tile._id));
    if(tile.selected){
        _serializer->writeString("true");
    }
    else{
        _serializer->writeString("false");
    }

    broadcast(_serializer->serialize());
    
}

bool NetworkController::checkConnection() {
    NetcodeConnection::State state = _network->getState();

    if (state == NetcodeConnection::State::CONNECTED) {
        if(_status == Status::CONNECTING || _status == Status::IDLE) {
            _status = Status::CONNECTED;
        }
        if (_isHost) {
            _roomid = _network->getRoom();
        }
        return true;
    } else if (state == cugl::netcode::NetcodeConnection::State::NEGOTIATING) {
        _status = Status::CONNECTING;
        return true;
    } else if (state == cugl::netcode::NetcodeConnection::State::DENIED ||
               state == cugl::netcode::NetcodeConnection::State::DISCONNECTED ||
               state == cugl::netcode::NetcodeConnection::State::FAILED ||
               state == cugl::netcode::NetcodeConnection::State::INVALID ||
               state == cugl::netcode::NetcodeConnection::State::MISMATCHED) {
        _status = Status::NETERROR;
        return false;
    }
    return true;
}

void NetworkController::broadcast(const std::vector<std::byte>& data) {
    if (_network && _network->getState() == NetcodeConnection::State::CONNECTED) {
        _network->broadcast(data);
    }
}

void NetworkController::startGame() {
    _serializer->reset();
    
    _status = Status::START;
    _serializer->writeString("start game");
    broadcast(_serializer->serialize());
}

void NetworkController::initGame(const std::shared_ptr<cugl::JsonValue>& deckJson) {
    _serializer->reset();
    
    _serializer->writeString("initialize game");
    _serializer->writeJson(deckJson);
    
    broadcast(_serializer->serialize());
}

void NetworkController::broadcastDeck(const std::shared_ptr<cugl::JsonValue>& deckJson) {
    _serializer->reset();
    
    _serializer->writeString("update deck");
    _serializer->writeJson(deckJson);
    
    broadcast(_serializer->serialize());
}

void NetworkController::broadcastNextTile(const std::shared_ptr<cugl::JsonValue>& tileJson) {
    _serializer->reset();
    
    _serializer->writeString("next tile update");
    _serializer->writeJson(tileJson);
    
    broadcast(_serializer->serialize());
}

void NetworkController::broadcastPileIndex(const int index){
    _serializer->reset();
    
    _serializer->writeString("pile index update");
    _serializer->writeUint32(index);
    
    broadcast(_serializer->serialize());
}

void NetworkController::broadcastDeckMap(const std::shared_ptr<cugl::JsonValue>& tileMapJson) {
    _serializer->reset();
    
    _serializer->writeString("tile map update");
    _serializer->writeJson(tileMapJson);
    
    broadcast(_serializer->serialize());
}

void NetworkController::broadcastPileLayer() {
    _serializer->reset();
    
    _serializer->writeString("update layer");
    
    broadcast(_serializer->serialize());
}

void NetworkController::broadcastUpdating(){
    _serializer->reset();
    
    _serializer->writeString("updating");
    
    broadcast(_serializer->serialize());
}

void NetworkController::broadcastRemoveDiscard(){
    _serializer->reset();
    
    _serializer->writeString("remove discard tile");
    
    broadcast(_serializer->serialize());
}

void NetworkController::broadcastNewDiscard(const std::shared_ptr<cugl::JsonValue>& tileJson){
    _serializer->reset();
    
    _serializer->writeString("new discard tile");
    _serializer->writeJson(tileJson);
    
    broadcast(_serializer->serialize());
}

void NetworkController::broadcastStartingDeck(const std::shared_ptr<cugl::JsonValue>& deckJson){
    _serializer->reset();
    
    _serializer->writeString("starting client deck");
    _serializer->writeJson(deckJson);
    
    broadcast(_serializer->serialize());
}

void NetworkController::broadcastPreDraw(int numDraw, bool isHost) {
    _serializer->reset();
    
    _serializer->writeString("preemptive draw");
    _serializer->writeUint32(numDraw);
    _serializer->writeBool(isHost);
    
    broadcast(_serializer->serialize());
}

/** ** BEGINNING OF MATCHCONTROLLER BROADCASTS** */

/**
 * Called during initialization of GameScene and MatchController. Broadcasts the initial representation
 * and state of game to client. When received, it sets status to INGAME for the client to join the game as
 * initialized by host.
 *
 * @param clientStart   The JsonValue representing the initial representation of all tiles
 */
void NetworkController::broadcastClientStart(const std::shared_ptr<cugl::JsonValue>& clientStart) {
    _serializer->reset();
    
    _serializer->writeString("client start");
    _serializer->writeJson(clientStart);
    
    broadcast(_serializer->serialize());
}

/**
 * Broadcasts the JSON representation of the tile that has been drawn. When received, it sets status to
 * TILEDRAWN, indicating a tile has been drawn to the match controller.
 *
 * @param drawnTileJson     The JsonValue representing the drawn tile
 */
void NetworkController::broadcastTileDrawn(int isHost, const std::shared_ptr<cugl::JsonValue>& drawnTileJson) {
    _serializer->reset();
    
    _serializer->writeString("tile drawn");
    _serializer->writeUint32(isHost);
    _serializer->writeJson(drawnTileJson);
    
    broadcast(_serializer->serialize());
}

/**
 * Broadcasts the JSON representation of all tiles in the tileset (not only deck). Currently used for:
 * remaking pile, updating deck (deleting and adjusting fields for tiles), etc.
 *
 * @param tileMapJson       The JsonValue of the tileMap 
 */
void NetworkController::broadcastTileMap(int isHost, const std::shared_ptr<cugl::JsonValue>& tileMapJson, std::string mapUpdateType) {
    _serializer->reset();
    
    _serializer->writeString("tile map update");
    _serializer->writeUint32(isHost);
    _serializer->writeJson(tileMapJson);
    _serializer->writeString(mapUpdateType);
    
    broadcast(_serializer->serialize());
}

/**
 * Broadcasts the JSON representation of the discarded tiles.
 *
 * @param discardedTileJson     The JsonValue of the discarded tile
 */
void NetworkController::broadcastDiscard(int isHost, const std::shared_ptr<cugl::JsonValue>& discardedTileJson) {
    _serializer->reset();
    
    _serializer->writeString("discard update");
    _serializer->writeUint32(isHost);
    _serializer->writeJson(discardedTileJson);
    
    broadcast(_serializer->serialize());
}

/**
 * Broadcasts the JSON representation of the celestial tile that has been played
 *
 * @param isHost        if the current network is the host network or not
 * @param tileMapJson       the JSON representation of the current tileMap
 * @param celestialTile     The JSON representation of the celestial tile
 * @param celestialType     The type of celestial tile that was played
 */
void NetworkController::broadcastCelestialTile(int isHost, const std::shared_ptr<cugl::JsonValue>& celestialTile, std::string celestialType) {
    _serializer->reset();
    
    _serializer->writeString("celestial tile played");
    _serializer->writeUint32(isHost);
    _serializer->writeString(celestialType);
    // If celestial that was played is CHAOS
    if(celestialType == "CHAOS") {
        //Writing tile to remove from opposing player's hand
        _serializer->writeJson(celestialTile);
    }
    
    broadcast(_serializer->serialize());
}

           
/** Broadcasts a message that the game has concluded */
void NetworkController::broadcastEnd(int isHost) {
    _serializer->reset();
    
    _serializer->writeString("game concluded");
    _serializer->writeUint32(isHost);

    broadcast(_serializer->serialize());
}

/** ** END OF MATCHCONTROLLER BROADCASTS** */

                                                                                                 
//
//void NetworkController::notifyObservers(std::vector<std::string>& msg){
//    observer.processData(msg);
//}
