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
    _serializer = cugl::netcode::NetcodeSerializer::alloc();
    _deserializer = cugl::netcode::NetcodeDeserializer::alloc();
};

void NetworkController::update(float timestep){
    if(_network) {
        _network->receive([this](const std::string source,
                                 const std::vector<std::byte>& data) {
            processData(source, data);
        });
        checkConnection();
    }
}

void NetworkController::processData(const std::string source,
                                    const std::vector<std::byte>& data){
    std::vector<std::string> msg;
    if(_network && data.size() > 0){
        _deserializer->receive(data);
        msg.emplace_back(std::get<std::string>(_deserializer->read()));
        msg.emplace_back(std::get<std::string>(_deserializer->read()));
        msg.emplace_back(std::get<std::string>(_deserializer->read()));
        
        notifyObservers(msg);
    }
}

void NetworkController::transmitSingleTile(TileSet::Tile& tile){
    std::vector<std::byte> msg;
    
    //Process tile name and id
    _serializer->writeString(tile.toString());
    _serializer->writeString(std::to_string(tile._id));
    if(tile.selected){
        _serializer->writeString("true");
    }
    else{
        _serializer->writeString("false");
    }

    _network->broadcast(_serializer->serialize());
}

bool NetworkController::checkConnection() {
    int connectState = static_cast<int>(_network->getState());

    if(connectState == 5 || connectState == 9){
        disconnect();
        return false;
    }
    return true;
}

void NetworkController::notifyObservers(std::vector<std::string>& msg){
    observer.processData(msg);
}
