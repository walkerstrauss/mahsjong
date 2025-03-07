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
//    _serializer = cugl::netcode::NetcodeSerializer::alloc();
//    _deserializer = cugl::netcode::NetcodeDeserializer::alloc();
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
    _network = cugl::netcode::NetcodeConnection::alloc(_config);
    _network->open();
    checkConnection();
    
    return _network->getState() == NetcodeConnection::State::CONNECTED;
}

//void NetworkController::update(float timestep){
//    if(_network) {
//        _network->receive([this](const std::string source,
//                                 const std::vector<std::byte>& data) {
//            processData(source, data);
//        });
//        checkConnection();
//    }
//}

bool NetworkController::connectAsHost(std::shared_ptr<cugl::netcode::NetcodeConnection>& _hostNetwork) {
    _isHost = true;
    _status = Status::CONNECTING;
    
    _hostNetwork = NetcodeConnection::alloc(_config, _network->getRoom());
    
    if (!_hostNetwork) {
        _status = Status::NETERROR;
        CULog("ERROR: Failed to allocate network connection!");
        return false;
    }
    
    _hostNetwork->open();
    
    return checkConnection();

}

bool NetworkController::connectAsClient(std::shared_ptr<cugl::netcode::NetcodeConnection>& _clientNetwork) {
    _isHost = false;
    _status = Status::CONNECTING;
    
    _clientNetwork = NetcodeConnection::alloc(_config, _network->getRoom());
    _clientNetwork->open();
    
    return checkConnection();
    
}

/**
 * Disconnects from the network.
 */
void NetworkController::disconnect() {
    _network = nullptr;
    _status = IDLE;
}

//void NetworkController::processData(const std::string source,
//                                    const std::vector<std::byte>& data){
//    std::vector<std::string> msg;
//    if(_network && data.size() > 0){
//        _deserializer->receive(data);
//        msg.emplace_back(std::get<std::string>(_deserializer->read()));
//        msg.emplace_back(std::get<std::string>(_deserializer->read()));
//        msg.emplace_back(std::get<std::string>(_deserializer->read()));
//        
//        notifyObservers(msg);
//    }
//}

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

    switch (state) {
        case NetcodeConnection::State::CONNECTED:
            if (_isHost && _roomid.empty()) {
                _roomid = _network->getRoom();
            }
            break;
        case NetcodeConnection::State::NEGOTIATING:
            _status = CONNECTING;
            break;
        case NetcodeConnection::State::DISCONNECTED:
        case NetcodeConnection::State::DENIED:
        case NetcodeConnection::State::MISMATCHED:
        case NetcodeConnection::State::INVALID:
        case NetcodeConnection::State::FAILED:
            disconnect();
            _status = NETERROR;
            return false;
        default:
            return false;
            
    };
    
    return true;
}

void NetworkController::broadcast(const std::vector<std::byte>& data) {
    if (_network && _network->getState() == NetcodeConnection::State::CONNECTED) {
        _network->broadcast(data);
    }
}

void NetworkController::startGame() {
    _status = Status::START;
    NetcodeSerializer serializer;
    serializer.writeString("start game");
    broadcast(serializer.serialize());
}


//void NetworkController::notifyObservers(std::vector<std::string>& msg){
//    observer.processData(msg);
//}
