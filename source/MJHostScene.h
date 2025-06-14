//
//  MJHostScene.h
//  Mahsjong
//
//  Created by Patrick Choo on 3/3/25.
//
#ifndef __MJ_HOST_SCENE_H__
#define __MJ_HOST_SCENE_H__
#include <cugl/cugl.h>
#include <vector>
#include "MJNetworkController.h"
#include "MJAudioController.h"


/**
 * This class provides the interface to make a new game.
 *
 * Most games have a since "matching" scene whose purpose is to initialize the
 * network controller.  We have separate the host from the client to make the
 * code a little more clear.
 */
class HostScene : public cugl::scene2::Scene2 {
public:
//    /**
//     * The configuration status
//     *
//     * This is how the application knows to switch to the next scene.
//     */
//    enum Status {
//        /** Host is waiting on a connection */
//        WAIT,
//        /** Host is waiting on all players to join */
//        IDLE,
//        /** Time to start the game */
//        START,
//        /** Game was aborted; back to main menu */
//        ABORT
//    };
    /** Scene node for player board in scene 1*/
    std::shared_ptr<cugl::scene2::SceneNode> playerBoard;
    /** Scene node for player board in scene 2*/
    std::shared_ptr<cugl::scene2::SceneNode> playerBoard2;
protected:
    /** The asset manager for this scene. */
    std::shared_ptr<cugl::AssetManager> _assets;
    /** The network controller */
    std::shared_ptr<NetworkController> _network;
    /** The network connection for this scene **/
//    std::shared_ptr<cugl::netcode::NetcodeConnection> _hostNetwork;
    /** The menu button for starting a game */
    //std::shared_ptr<cugl::scene2::Button> _startgame;
    /** The back button for the menu scene */
    std::shared_ptr<cugl::scene2::Button> _backout;

    std::shared_ptr<cugl::scene2::Button> _startgame2;
    /** The back button for the menu scene */
    std::shared_ptr<cugl::scene2::Button> _backout2;

    /** The game id label (for updating) */
    std::shared_ptr<cugl::scene2::Label> _gameid;
    /** The players label (for updating) */
    std::shared_ptr<cugl::scene2::Label> _player;
    /** texture for waiting or start button */
    //std::shared_ptr<cugl::scene2::PolygonNode> _waitOrStart1;
    //std::shared_ptr<cugl::scene2::PolygonNode> _waitOrStart3;

    /** Reference to init host scene*/
    std::shared_ptr<cugl::scene2::SceneNode> _hostScene1;

    std::shared_ptr<cugl::scene2::SceneNode> _hostScene3;

    /** Reference to Gameboard when players are connecting or alone */
    //std::shared_ptr<cugl::scene2::SceneNode> _playerSingle;
    //std::shared_ptr<cugl::scene2::SceneNode> _playerMulti;
    
    /** Tiles for gameID */
    std::shared_ptr<cugl::scene2::PolygonNode> _tileOne;
    std::shared_ptr<cugl::scene2::PolygonNode> _tileTwo;
    std::shared_ptr<cugl::scene2::PolygonNode> _tileThree;
    std::shared_ptr<cugl::scene2::PolygonNode> _tileFour;
    std::shared_ptr<cugl::scene2::PolygonNode> _tileFive;

    std::shared_ptr<cugl::scene2::PolygonNode> _tileOne3;
    std::shared_ptr<cugl::scene2::PolygonNode> _tileTwo3;
    std::shared_ptr<cugl::scene2::PolygonNode> _tileThree3;
    std::shared_ptr<cugl::scene2::PolygonNode> _tileFour3;
    std::shared_ptr<cugl::scene2::PolygonNode> _tileFive3;
    
    /** Whether the startGame button had been pressed. */
    bool _startGameClicked = false;
    /** Whether the back button had been pressed. */
    bool _backClicked = false;

    bool _playedSound = false;
    
    /** The network configuration */
    cugl::netcode::NetcodeConfig _config;
    
//    /** The current status */
//    Status _status;

public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new host scene with the default values.
     *
     * This constructor does not allocate any objects or start the game.
     * This allows us to use the object without a heap pointer.
     */
    HostScene() : cugl::scene2::Scene2() {}
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~HostScene() { dispose(); }
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose() override;

    void idSetup(const std::shared_ptr<cugl::scene2::PolygonNode>& tile, const char serverPart);

    
    /**
     * Initializes the controller contents, and starts the game
     *
     * In previous labs, this method "started" the scene.  But in this
     * case, we only use to initialize the scene user interface.  We
     * do not activate the user interface yet, as an active user
     * interface will still receive input EVEN WHEN IT IS HIDDEN.
     *
     * That is why we have the method {@link #setActive}.
     *
     * @param assets    The (loaded) assets for this game mode
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<NetworkController> network);
    
    /**
     * Sets whether the scene is currently active
     *
     * This method should be used to toggle all the UI elements.  Buttons
     * should be activated when it is made active and deactivated when
     * it is not.
     *
     * @param value whether the scene is currently active
     */
    virtual void setActive(bool value) override;
    
//    /**
//     * Returns the network connection (as made by this scene)
//     *
//     * This value will be reset every time the scene is made active.
//     * In addition, this method will return nullptr if {@link #disconnect}
//     * has been called.
//     *
//     * @return the network connection (as made by this scene)
//     */
//    std::shared_ptr<cugl::netcode::NetcodeConnection> getConnection() const {
//        return _network;
//    }
//
//    /**
//     * Returns the scene status.
//     *
//     * Any value other than WAIT will transition to a new scene.
//     *
//     * @return the scene status
//     *
//     */
//    Status getStatus() const { return _status; }

    /**
     * The method called to update the scene.
     *
     * We need to update this method to constantly talk to the server
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void update(float timestep) override;
    
//    /**
//     * Disconnects this scene from the network controller.
//     *
//     * Technically, this method does not actually disconnect the network controller.
//     * Since the network controller is a smart pointer, it is only fully disconnected
//     * when ALL scenes have been disconnected.
//     */
//    void disconnect() { _network = nullptr; }
    
    void makeBlackBackground() {
        _batch->begin(getCamera()->getCombined());
        _batch->setColor(cugl::Color4::BLACK);
        _batch->fill(cugl::Rect(cugl::Vec2::ZERO, cugl::Application().get()->getDisplaySize()));
        _batch->end();
    }
    
    /**
     * Returns whether the back button is pressed
     */
    bool getBackClicked() { return _backClicked; };
    
private:
    /**
     * Updates the text in the given button.
     *
     * Techincally a button does not contain text. A button is simply a scene graph
     * node with one child for the up state and another for the down state. So to
     * change the text in one of our buttons, we have to descend the scene graph.
     * This method simplifies this process for you.
     *
     * @param button    The button to modify
     * @param text      The new text value
     */
    void updateText(const std::shared_ptr<cugl::scene2::Button>& button, const std::string text);
    
//    /**
//     * Reconfigures the start button for this scene
//     *
//     * This is necessary because what the buttons do depends on the state of the
//     * networking.
//     */
//    void configureStartButton();
//    
//    /**
//     * Connects to the game server as specified in the assets file
//     *
//     * The {@link #init} method set the configuration data. This method simply uses
//     * this to create a new {@Link NetworkConnection}. It also immediately calls
//     * {@link #checkConnection} to determine the scene state.
//     *
//     * @return true if the connection was successful
//     */
//    bool connect();
//
//    /**
//     * Processes data sent over the network.
//     *
//     * Once connection is established, all data sent over the network consistes of
//     * byte vectors. This function is a call back function to process that data.
//     * Note that this function may be called *multiple times* per animation frame,
//     * as the messages can come from several sources.
//     *
//     * In this lab, this method does not do all that much. Typically this is where
//     * players would communicate their names after being connected.
//     *
//     * @param source    The UUID of the sender
//     * @param data      The data received
//     */
//    void processData(const std::string source, const std::vector<std::byte>& data);
//
//    /**
//     * Checks that the network connection is still active.
//     *
//     * Even if you are not sending messages all that often, you need to be calling
//     * this method regularly. This method is used to determine the current state
//     * of the scene.
//     *
//     * @return true if the network connection is still active.
//     */
//    bool checkConnection();
    
    /**
     * Starts the game.
     *
     * This method is called once the requisite number of players have connected.
     * It locks down the room and sends a "start game" message to all other
     * players.
     */
    void startGame();
    
    
    
};

#endif /* __MJ_HOST_SCENE_H__ */
