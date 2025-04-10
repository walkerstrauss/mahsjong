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
        INGAME,
        /** Create new pile layer */
        LAYER,
        /** Deck changed, please update */
        DECK,
        /** Pile updated  */
        PILETILEUPDATE,
        /** Remove discard tile*/
        REMOVEDISCARD,
        /** Adding new discard tile*/
        NEWDISCARD,
        /** Error in connection */
        NETERROR,
        /** Preemptive discard for reciever */
        PREEMPTIVEDISCARD,
        
        /** **BEGINNING OF MATCHMODEL STATES ** */
        /** Tile has been drawn from pile */
        TILEDRAWN,
        /** TileMap has been updated */
        TILEMAPUPDATE,
        /** Discard pile has been updated */
        DISCARDUPDATE,
        /** Celestial tile has been played */
        PLAYEDCELESTIAL,
        /** Game has concluded  */
        ENDGAME
        /** **END OF MATCHMODEL STATES ** */
        
    };
    
    enum MapUpdateType {
        /** Idle Update*/
        NOUPDATE,
        /** Update to remake pile */
        REMAKEPILE
    };
    
    enum CelestialUpdateType {
        /** Idle Update */
        NONE,
        /** Chaos played */
        CHAOS
    };
    
protected:
    /** The asset manager for the controller. */
    std::shared_ptr<cugl::AssetManager> _assets;
    
    /** The network configuration */
    cugl::netcode::NetcodeConfig _config;
    
    /** The network connection */
    std::shared_ptr<cugl::netcode::NetcodeConnection> _network;
    
    Status _status;
    
    /** Enumerator type for map updates */
    MapUpdateType _mapUpdateType;
    
    /** Enumerator type for celestial tile updates */
    CelestialUpdateType _celestialUpdateType;
    
    std::string _roomid;
    
    bool _isHost;
    
    Uint32 _localPid;
    
    Uint32 _localReady;
    
    Uint32 _currentTurn;
    
    Uint32 _pileIndex;
    
    std::shared_ptr<cugl::JsonValue> _deckJson;
    
    std::shared_ptr<cugl::JsonValue> _startingDeckJson;
    
    std::shared_ptr<cugl::JsonValue> _nextTileJson;
    
    std::shared_ptr<cugl::JsonValue> _pileTileJson;
    
    std::shared_ptr<cugl::JsonValue> _discardTile;
    
    std::shared_ptr<cugl::JsonValue> _tileMapJson;
    
    bool _isHostDraw;
    
    std::tuple<int, bool> _numDiscard;
    
    /** ** START OF MATCH CONTROLLER BRANCH FIELDS**  */
    
    /** JSON representing the initial game reprsentation */
    std::shared_ptr<cugl::JsonValue> _clientStart;
    /** JSON representing the tile that was drawn */
    std::shared_ptr<cugl::JsonValue> _tileDrawn;
    /** JSON representing the celestial tile that was played */
    std::shared_ptr<cugl::JsonValue> _celestialTile; 
    /** ** END OF MATCH CONTROLLER BRANCH FIELDS**  */
    
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
    
    void initGame(const std::shared_ptr<cugl::JsonValue>& deckJson);
    
    void broadcastDeck(const std::shared_ptr<cugl::JsonValue>& deckJson);
    
    void broadcastNextTile(const std::shared_ptr<cugl::JsonValue>& tileJson);
    
    void broadcastPileIndex(const int index);
    
    void broadcastPileLayer();
    
    void broadcastRemoveDiscard();
    
    void broadcastNewDiscard(const std::shared_ptr<cugl::JsonValue>& tileJson);
    
    void broadcastStartingDeck(const std::shared_ptr<cugl::JsonValue>& deckJson);
    
    void broadcastDeckMap(const std::shared_ptr<cugl::JsonValue>& tileMapJson);
    
    
    Uint32 getLocalPid() const {
        return _localPid;
    }
    
    Uint32 getCurrentTurn() const {
        return _currentTurn;
    }
    
    Status getStatus() const {
        return _status;
    }
    
    void setStatus(Status status) {
        _status = status;
    }
    
    /** Sets the status of pile map updates */
    void setMapUpdateType(MapUpdateType mapUpdateType) {
        _mapUpdateType = mapUpdateType;
    }
    
    /** Sets celestial tile update type */
    void setCelestialUpdateType(CelestialUpdateType celestialUpdateType) {
        _celestialUpdateType = celestialUpdateType;
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
    
    std::shared_ptr<cugl::JsonValue> getDiscardTile(){
        return _discardTile;
    }
    
    std::shared_ptr<cugl::JsonValue> getNextTileJson(){
        return _nextTileJson;
    }
    
    std::shared_ptr<cugl::JsonValue> getPileTile() {
        return _pileTileJson;
    }
    
    std::shared_ptr<cugl::JsonValue> getStartingDeck() {
        return _startingDeckJson;
    }
    
    std::tuple<int, bool> getNumDiscard() {
        return _numDiscard;
    }
    
    bool isNewPileTile() {
        return _pileTileJson != nullptr;
    }
    
    void resetPileTile() {
        _pileTileJson = nullptr;
    }
    
    bool getIsHostDraw() {
        return _isHostDraw;
    }
    
    void broadcastUpdating();
    
    void broadcastReady();
    
    void broadcastPreDraw(int numDraw, bool isHost);
    
    /** **START OF MATCH CONTROLLER GETTERS** */
    
    /** Retrieves tileMap JSON as received by the network */
    std::shared_ptr<cugl::JsonValue> getTileMapJson() {
        return _tileMapJson;
    }
    
    /** Retrieves the current map update type */
    MapUpdateType getMapUpdateType() {
        return _mapUpdateType; 
    }
    
    /** Retrieves the current celestial update type */
    CelestialUpdateType getCelestialUpdateType() {
        return _celestialUpdateType;
    }
    
    /** Retrieves the starting representation of game (for client) */
    std::shared_ptr<cugl::JsonValue> getClientStart() {
        return _clientStart;
    }
    
    /** Retrieves the tile that was drawn as received by the network*/
    std::shared_ptr<cugl::JsonValue> getTileDrawn() {
        return _tileDrawn; 
    }
    
    /** Retrieves the celestial tile that was played as received by the network*/
    std::shared_ptr<cugl::JsonValue> getCelestialTile() {
        return _celestialTile;
    }
    
    /** **END OF MATCH CONTROLLER GETTERS ** */
    
    /** ** START OF MATCH CONTROLLER BRANCH FUNCTIONS**  */
    
    /**
     * Called during initialization of GameScene and MatchController. Broadcasts the initial representation
     * and state of game to client. When received, it sets status to INGAME for the client to join the game as
     * initialized by host.
     *
     * @param clientStart   The JsonValue representing the initial representation of all tiles
     */
    void broadcastClientStart(const std::shared_ptr<cugl::JsonValue>& clientStart);
    
    /**
     * Broadcasts the JSON representation of the tile that has been drawn. When received, it sets status to
     * TILEDRAWN, indicating a tile has been drawn to the match controller.
     *
     * @param isHost    If current network is the host network or not
     * @param drawnTileJson     The JsonValue representing the drawn tile
     */
    void broadcastTileDrawn(int isHost, const std::shared_ptr<cugl::JsonValue>& tileJson);
    
    /**
     * Broadcasts the JSON representation of all tiles in the tileset (not only deck). Currently used for:
     * remaking pile, updating deck (deleting and adjusting fields for tiles), etc.
     *
     * @param isHost    If current network is the host network or not
     * @param tileMapJson       The JsonValue of the tileMap
     * @param mapUpdateType     The type of map update
     */
    void broadcastTileMap(int isHost, const std::shared_ptr<cugl::JsonValue>& tileMapJson, std::string mapUpdateType);
    
    /**
     * Broadcasts the JSON representation of the discarded tiles.
     *
     * @param isHost    If current network is the host network or not
     * @param discardedTileJson     The JsonValue of the discarded tile
     */
    void broadcastDiscard(int isHost, const std::shared_ptr<cugl::JsonValue>& discardedTileJson);
    
    /**
     * Broadcasts the JSON representation of the celestial tile that has been played
     *
     * @param isHost        if the current network is the host network or not
     * @param tileMapJson       the JSON representation of the current tileMap 
     * @param celestialTile     The JSON representation of the celestial tile
     * @param celestialType     The type of celestial tile that was played
     */
    void broadcastCelestialTile(int isHost, const std::shared_ptr<cugl::JsonValue>& tileMapJson, const std::shared_ptr<cugl::JsonValue>& celestialTile, std::string celestialType);
    /**
     * Broadcasts a message that the game has concluded
     *
     * @param isHost    If current network is the host network or not
     */
    void broadcastEnd(int isHost);
    
    /** ** START OF MATCH CONTROLLER BRANCH FUNCTIONS**  */

    
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
