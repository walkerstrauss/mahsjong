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

class NetworkController {
public:
    /**
     * The configuration status
     *
     * This is how the application knows to switch to the next scene.
     */
    enum Status {
        /** No connection requested */
        IDLE,
        /** Connecting to server */
        CONNECTING,
        /** Connected to server */
        CONNECTED,
        /** Host can start the game */
        START,
        /** Ready for client to start */
        READY,
        /** Error in connection */
        NETERROR
    };

protected:
    /** The asset manager for the controller. */
    std::shared_ptr<cugl::AssetManager> _assets;

    /** The network configuration */
    cugl::netcode::NetcodeConfig _config;

    /** The network connection */
    std::shared_ptr<cugl::netcode::NetcodeConnection> _network;

    Status _status;

    std::string _roomid;

    bool _isHost;
    
    Uint32 _localPid;
    
    Uint32 _localReady; 
    
    Uint32 _currentTurn;
    
    std::shared_ptr<cugl::JsonValue> _deckJson;
    

public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new NetworkController with the default values.
     *
     * This constructor does not allocate any objects or start the game.
     * This allows us to use the object without a heap pointer.
     */
    NetworkController();

    /**
     * Disposes of all (non-static) resources allocated to this controller.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~NetworkController () { dispose(); }
    
    std::shared_ptr<cugl::netcode::NetcodeSerializer> _serializer;
    std::shared_ptr<cugl::netcode::NetcodeDeserializer> _deserializer;

    /**
     * Disposes of all (non-static) resources allocated to this controller.
     */
    void dispose();

    bool init(const std::shared_ptr<cugl::AssetManager>& assets);
    
    int getNumPlayers() const {
        if (_network) {
            return (int)(_network->getNumPlayers());
        }
        return 1;
    }
    void update(float timestep);

    bool connectAsHost();

    bool connectAsClient(std::string room);

    std::string getRoomID() const {
        return _roomid;
    }

    void disconnect();
    
    void notifyEndTurn();
    
    void endTurn();
    
    void transmitSingleTile(TileSet::Tile& tile);

    bool checkConnection();

    void broadcast(const std::vector<std::byte>& data);

    void processData(const std::string source, const std::vector<std::byte>& data);

    void startGame();
        
    void broadcastDeck(const std::shared_ptr<cugl::JsonValue>& deckJson);

    
    Uint32 getLocalPid() const {
        return _localPid;
    }

    Uint32 getCurrentTurn() const {
        return _currentTurn;
    }

    Status getStatus() const {
        return _status;
    }
    
    std::shared_ptr<cugl::JsonValue> getDeckJson(){
        return _deckJson;
    }
    
    bool loadedDeck(){
        return _deckJson != nullptr;
    }
    bool getHostStatus(){
        return _isHost;
    }
    
    /**
     * Converts a decimal string to a hexadecimal string
     *
     * This function assumes that the string is a decimal number less
     * than 65535, and therefore converts to a hexadecimal string of four
     * characters or less (as is the case with the lobby server). We
     * pad the hexadecimal string with leading 0s to bring it to four
     * characters exactly.
     *
     * @param dec the decimal string to convert
     *
     * @return the hexadecimal equivalent to dec
     */
    static std::string dec2hex(const std::string dec) {
        Uint32 value = cugl::strtool::stou32(dec);
        if (value >= 655366) {
            value = 0;
        }
        return cugl::strtool::to_hexstring(value, 4);
    }

    /**
     * Converts a hexadecimal string to a decimal string
     *
     * This function assumes that the string is 4 hexadecimal characters
     * or less, and therefore it converts to a decimal string of five
     * characters or less (as is the case with the lobby server). We
     * pad the decimal string with leading 0s to bring it to 5 characters
     * exactly.
     *
     * @param hex the hexadecimal string to convert
     *
     * @return the decimal equivalent to hex
     */
    static std::string hex2dec(const std::string hex) {
        Uint32 value = cugl::strtool::stou32(hex, 0, 16);
        std::string result = cugl::strtool::to_string(value);
        if (result.size() < 5) {
            size_t diff = 5 - result.size();
            std::string alt(5, '0');
            for (size_t ii = 0; ii < result.size(); ii++) {
                alt[diff + ii] = result[ii];
            }
            result = alt;
        }
        return result;
    }
};

#endif /* __MJ_NETWORK_CONTROLLER_H__ */
