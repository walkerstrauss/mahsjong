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
    _isHost = false;
    _roomid = "";
    _currentTurn = 0;
    _serializer = cugl::netcode::NetcodeSerializer::alloc();
    _deserializer = cugl::netcode::NetcodeDeserializer::alloc();
};


/**
 * Disposes of all (non-static) resources allocated to this controller.
 */
void NetworkController::dispose() {
    disconnect();
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
}

void NetworkController::processData(const std::string source,
                                    const std::vector<std::byte>& data){
    static bool first = true;
    if (_network->getHost() == source && first) {
        CULog("received message from host");
        _status = START;
        first = false;
    }
    
}

//void NetworkController::transmitSingleTile(TileSet::Tile& tile){
//    std::vector<std::byte> msg;
//    
//    //Process tile name and id
//    _serializer->writeString(tile.toString());
//    _serializer->writeString(std::to_string(tile._id));
//    if(tile.selected){
//        _serializer->writeString("true");
//    }
//    else{
//        _serializer->writeString("false");
//    }
//
//    _network->broadcast(_serializer->serialize());
//}

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
    CULog("network starting game");
    _status = Status::START;
    _serializer->writeString("start game");
    broadcast(_serializer->serialize());
}

//
//void NetworkController::notifyObservers(std::vector<std::string>& msg){
//    observer.processData(msg);
//}
