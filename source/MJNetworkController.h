//
//  MJNetworkController.h
//  Mahsjong
//
//  Created by Patrick Choo on 3/5/25.
//

#ifndef __MJ_NETWORK_CONTROLLER_H__
#define __MJ_NETWORK_CONTROLLER_H__
#include <cugl/cugl.h>
#include <vector>
#include "MJTileSet.h"
#include "MJPlayer.h"
#include "MJGameScene.h"

class NetworkController {
private:
    /**
     * The network connection
     */
    std::shared_ptr<cugl::netcode::NetcodeConnection> _network;
    
    /**
     * Processes data sent over the network.
     *
     * Once connection is established, all data sent over the network consistes of
     * byte vectors. This function is a call back function to process that data.
     * Note that this function may be called *multiple times* per animation frame,
     * as the messages can come from several sources.
     */
    void processData(const std::string source, const std::vector<std::byte>& data);
    
    /**
     * Checks that the network connection is still active.
     *
     * Even if you are not sending messages all that often, you need to be calling
     * this method regularly. This method is used to determine the current state
     * of the scene.
     */
    bool checkConnection();
    
    /**
     *  Observers for reading data enqueued data
     */
    
    GameScene observer;
    
public:
    
    NetworkController();
    
    ~NetworkController() { disconnect(); };
    
    std::shared_ptr<cugl::netcode::NetcodeSerializer> _serializer;
    std::shared_ptr<cugl::netcode::NetcodeDeserializer> _deserializer;

    /**
     * Returns the network connection
     */
    std::shared_ptr<cugl::netcode::NetcodeConnection> getConnection() const {
        return _network;
    }
    
    /**
     * Sets the connection for the network
     */
    void setConnection(const std::shared_ptr<cugl::netcode::NetcodeConnection>& network){
        _network = network;
    }
    
    /**
     * Disconnects the network associated w/ the network controller
     */
    void disconnect() { _network = nullptr; }
    
    /**
     * Processes a single tile update
     */
    
    void transmitSingleTile(TileSet::Tile& tile);
    
    /**
     * Updating observer list
     */
    void addObserver(GameScene& scene){ observer = scene; }
    
    /**
     * Notifying observers of changes
     */
    void notifyObservers(std::vector<std::string>& msg);
    
    void update(float timestep);
};

#endif /* __MJ_NETWORK_CONTROLLER_H__ */
