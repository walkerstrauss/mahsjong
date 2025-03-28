//
//  MJClientScene.h
//  Mahsjong
//
//  Created by Patrick Choo on 3/3/25.
//

#ifndef __MJ_CLIENT_SCENE_H__
#define __MJ_CLIENT_SCENE_H__
#include <cugl/cugl.h>
#include <vector>
#include "MJNetworkController.h"

/**
 * This class provides the interface to join an existing game.
 *
 * Most games have a since "matching" scene whose purpose is to initialize the
 * network controller.  We have separate the host from the client to make the
 * code a little more clear.
 */
class ClientScene : public cugl::scene2::Scene2 {
public:
//    /**
//     * The configuration status
//     *
//     * This is how the application knows to switch to the next scene.
//     */
//    enum Status {
//        /** Client has not yet entered a room */
//        IDLE,
//        /** Client is connecting to the host */
//        JOIN,
//        /** Client is waiting on host to start game */
//        WAIT,
//        /** Time to start the game */
//        START,
//        /** Game was aborted; back to main menu */
//        ABORT
//    };
    
protected:
    /** The asset manager for this scene. */
    std::shared_ptr<cugl::AssetManager> _assets;
    /** The network controller */
    std::shared_ptr<NetworkController> _network;

//    /** The network connection (as made by this scene) */
//    std::shared_ptr<cugl::netcode::NetcodeConnection> _clientNetwork;

    /** The menu button for starting a game */
    std::shared_ptr<cugl::scene2::Button> _startgame;
    /** The back button for the menu scene */
    std::shared_ptr<cugl::scene2::Button> _backout;
    /** The back button for the lobby scene */
    std::shared_ptr<cugl::scene2::Button> _backout2;
    /** The game id label (for updating) */
    std::shared_ptr<cugl::scene2::TextField> _gameid;
    /** The players label (for updating) */
    std::shared_ptr<cugl::scene2::Label> _player;
    /** Reference to beginning keyboard client scene*/
    std::shared_ptr<cugl::scene2::SceneNode> _clientScene1;
    /** Reference to final client scene*/
    std::shared_ptr<cugl::scene2::SceneNode> _clientScene2;
    /**Checkbox for client**/
    std::shared_ptr<cugl::scene2::SceneNode> _clientCheckbox;
    /** texture for waiting or start button */
    std::shared_ptr<cugl::scene2::PolygonNode> _prepareOrStart;
    /** The network configuration */
    cugl::netcode::NetcodeConfig _config;

    /** Keypad */
    std::shared_ptr<cugl::scene2::Button> _tile1;
    std::shared_ptr<cugl::scene2::Button> _tile2;
    std::shared_ptr<cugl::scene2::Button> _tile3;
    std::shared_ptr<cugl::scene2::Button> _tile4;
    std::shared_ptr<cugl::scene2::Button> _tile5;
    std::shared_ptr<cugl::scene2::Button> _tile6;
    std::shared_ptr<cugl::scene2::Button> _tile7;
    std::shared_ptr<cugl::scene2::Button> _tile8;
    std::shared_ptr<cugl::scene2::Button> _tile9;
    std::shared_ptr<cugl::scene2::Button> _tile10;

    std::shared_ptr<cugl::scene2::Button> _resetGameID;

    /** GameID Tiles */
    //std::shared_ptr<cugl::scene2::PolygonNode> _tileID1;
    //std::shared_ptr<cugl::scene2::PolygonNode> _tileID2;
    //std::shared_ptr<cugl::scene2::PolygonNode> _tileID3;
    //std::shared_ptr<cugl::scene2::PolygonNode> _tileID4;

    /** GameID list */
    std::shared_ptr<cugl::scene2::PolygonNode> _gameIDNew[4];
    int _idPos;
    
    /** Whether the back button had been clicked. */
    bool _backClicked = false;

    /** Whether we are using the second page of the keypad for client/host */
    bool _keySecPage = false;
//    /** The current status */
//    Status _status;

public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new client scene with the default values.
     *
     * This constructor does not allocate any objects or start the game.
     * This allows us to use the object without a heap pointer.
     */
    ClientScene() : cugl::scene2::Scene2() {}
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~ClientScene() { dispose(); }
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose() override;
    
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

    std::string tile2hex();
    std::string idCreation(const std::shared_ptr<cugl::scene2::PolygonNode>& tile);

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
//     *
//     * @return the network connection (as made by this scene)
//     */
//    std::shared_ptr<cugl::netcode::NetcodeConnection> getConnection() const {
//        return _network;
//    }

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
    
    /**
     * Returns whether the back button has been clicked
     */
    bool getBackClicked() { return _backClicked; }

//    /**
//     * Disconnects this scene from the network controller.
//     *
//     * Technically, this method does not actually disconnect the network controller.
//     * Since the network controller is a smart pointer, it is only fully disconnected
//     * when ALL scenes have been disconnected.
//     */
//    void disconnect() { _network = nullptr; }

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

    /**
     * Reconfigures the start button for this scene
     *
     * This is necessary because what the buttons do depends on the state of the
     * networking.
     */
    void configureStartButton();
    
//    /**
//     * Connects to the game server as specified in the assets file
//     *
//     * The {@link #init} method set the configuration data. This method simply uses
//     * this to create a new {@Link NetworkConnection}. It also immediately calls
//     * {@link #checkConnection} to determine the scene state.
//     *
//     * @param room  The room ID to use
//     *
//     * @return true if the connection was successful
//     */
//    bool connect(const std::string room);
//
//    /**
//     * Processes data sent over the network.
//     *
//     * Once connection is established, all data sent over the network consistes of
//     * byte vectors. This function is a call back function to process that data.
//     * Note that this function may be called *multiple times* per animation frame,
//     * as the messages can come from several sources.
//     *
//     * Typically this is where players would communicate their names after being
//     * connected. In this lab, we only need it to do one thing: communicate that
//     * the host has started the game.
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

};

#endif /* __MJ_CLIENT_SCENE_H__ */
