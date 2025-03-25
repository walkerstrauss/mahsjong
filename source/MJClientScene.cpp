//
//  MJClientScene.cpp
//  Mahsjong
//
//  Created by Patrick Choo on 3/3/25.
//
#include <cugl/cugl.h>
#include <iostream>
#include <sstream>

#include "MJClientScene.h"

using namespace cugl;
using namespace cugl::netcode;
using namespace std;

#pragma mark -
#pragma mark Level Layout

/** Regardless of logo, lock the height to this */
#define SCENE_HEIGHT  720

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
    Uint32 value = strtool::stou32(dec);
    if (value >= 655366) {
        value = 0;
    }
    return strtool::to_hexstring(value,4);
}


#pragma mark -
#pragma mark Provided Methods
/**
 * Initializes the controller contents, and starts the game
 *
 * The constructor does not allocate any objects or memory.  This allows
 * us to have a non-pointer reference to this controller, reducing our
 * memory allocation.  Instead, allocation happens in this method.
 *
 * @param assets    The (loaded) assets for this game mode
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool ClientScene::init(const std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<NetworkController> network) {
    // Initialize the scene to a locked width
    if (assets == nullptr) {
        return false;
    } else if (!Scene2::initWithHint(Size(0,SCENE_HEIGHT))) {
        return false;
    }
    
    // Start up the input handler
    _assets = assets;
    _network = network;
    
    Size dimen = getSize();
    // Acquire the scene built by the asset loader and resize it the scene
    std::shared_ptr<scene2::SceneNode> scene = _assets->get<scene2::SceneNode>("client");
    scene->setContentSize(dimen);
    scene->doLayout(); // Repositions the HUD

    _startgame = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.clientscene.buttons.confirm-button"));
    _backout = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.clientscene.buttons.cancel-button"));

    // Tile GameID
    _gameIDNew[0] = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("client.clientscene.client1-gameid-blank.client1-gameid-blank"));
    _gameIDNew[1] = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("client.clientscene.client1-gameid-blank.client1-gameid-blank_1"));
    _gameIDNew[2] = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("client.clientscene.client1-gameid-blank.client1-gameid-blank_2"));
    _gameIDNew[3] = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("client.clientscene.client1-gameid-blank.client1-gameid-blank_3"));
    _idPos = 0;

    // Tile Keypad
    _tile1 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.clientscene.gameid-pad.pad-tile"));
    _tile2 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.clientscene.gameid-pad.pad-tile1"));
    _tile3 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.clientscene.gameid-pad.pad-tile2"));
    _tile4 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.clientscene.gameid-pad.pad-tile3"));
    _tile5 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.clientscene.gameid-pad.pad-tile4"));
    _tile6 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.clientscene.gameid-pad.pad-tile5"));
    _tile7 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.clientscene.gameid-pad.pad-tile6"));
    _tile8 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.clientscene.gameid-pad.pad-tile7"));
    _tile9 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.clientscene.gameid-pad.pad-tile8"));
    _tile10 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.clientscene.gameid-pad.pad-tile9"));

    _resetGameID = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.clientscene.cancel-box"));
    
    _backout->addListener([this](const std::string& name, bool down) {
        if (down) {
            _backClicked = true;
            _network->disconnect();
            _gameIDNew[0]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-blank"));
            _gameIDNew[1]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-blank"));
            _gameIDNew[2]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-blank"));
            _gameIDNew[3]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-blank"));
            _idPos = 0;
        }
        });

    _resetGameID->addListener([this](const std::string& name, bool down) {
        if (down) {
            _gameIDNew[0]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-blank"));
            _gameIDNew[1]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-blank"));
            _gameIDNew[2]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-blank"));
            _gameIDNew[3]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-blank"));
            _idPos = 0;
        }
        });

    _startgame->addListener([=, this](const std::string& name, bool down) {
        if (down) {
            // This will call the _gameid listener
            //_gameid->releaseFocus();
        }
        });

    _tile1->addListener([this](const std::string& name, bool down) {
        if (down && _idPos != 4) {
            _gameIDNew[_idPos]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile"));
            _idPos++;
        }
        });
    _tile2->addListener([this](const std::string& name, bool down) {
        if (down && _idPos != 4) {
            _gameIDNew[_idPos]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile1"));
            _idPos++;
        }
        });
    _tile3->addListener([this](const std::string& name, bool down) {
        if (down && _idPos != 4) {
            _gameIDNew[_idPos]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile2"));
            _idPos++;
        }
        });
    _tile4->addListener([this](const std::string& name, bool down) {
        if (down && _idPos != 4) {
            _gameIDNew[_idPos]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile3"));
            _idPos++;
        }
        });
    _tile5->addListener([this](const std::string& name, bool down) {
        if (down && _idPos != 4) {
            _gameIDNew[_idPos]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile4"));
            _idPos++;
        }
        });
    _tile6->addListener([this](const std::string& name, bool down) {
        if (down && _idPos != 4) {
            _gameIDNew[_idPos]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile5"));
            _idPos++;
        }
        });
    _tile7->addListener([this](const std::string& name, bool down) {
        if (down && _idPos != 4) {
            _gameIDNew[_idPos]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile6"));
            _idPos++;
        }
        });
    _tile8->addListener([this](const std::string& name, bool down) {
        if (down && _idPos != 4) {
            _gameIDNew[_idPos]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile7"));
            _idPos++;
        }
        });
    _tile9->addListener([this](const std::string& name, bool down) {
        if (down && _idPos != 4) {
            _gameIDNew[_idPos]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile8"));
            _idPos++;
        }
        });
    _tile10->addListener([this](const std::string& name, bool down) {
        if (down && _idPos != 4) {
            _gameIDNew[_idPos]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile9"));
            _idPos++;
        }
        });

    //TEMP REMOPVE LATER
    _backout->activate();
    _tile1->activate();
    _tile2->activate();
    _tile3->activate();
    _tile4->activate();
    _tile5->activate();
    _tile6->activate();
    _tile7->activate();
    _tile8->activate();
    _tile9->activate();
    _tile10->activate();
    _resetGameID->activate();
    addChild(scene);
    return true;
    //TEMP REMOPVE LATER
    _gameid = std::dynamic_pointer_cast<scene2::TextField>(_assets->get<scene2::SceneNode>("client.clientscene.menu.gameid.gameid_textfield.gameid"));
    _player = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("client.clientscene.menu.player.player_textfield.label"));
//    _status = Status::IDLE;
    
    
    
    _gameid->addExitListener([this](const std::string& name, const std::string& value) {
        _network->connectAsClient(dec2hex(value));
    });

    // Create the server configuration
    auto json = _assets->get<JsonValue>("server");
    _config.set(json);
    
    addChild(scene);
    setActive(false);
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void ClientScene::dispose() {
    if (_active) {
        removeAllChildren();
        _active = false;
    }
}

/**
 * Sets whether the scene is currently active
 *
 * This method should be used to toggle all the UI elements.  Buttons
 * should be activated when it is made active and deactivated when
 * it is not.
 *
 * @param value whether the scene is currently active
 */
void ClientScene::setActive(bool value) {
    if (isActive() != value) {
        Scene2::setActive(value);
        if (value) {
            //_gameid->activate();
            _backout->activate();
            //_player->setText("1");
            configureStartButton();
            _backClicked = false;
            // Don't reset the room id
        } else {
            //_gameid->deactivate();
            _startgame->deactivate();
            _backout->deactivate();
            _startgame->setDown(false);
            _backout->setDown(false);
        }
    }
}

/**
 * Checks that the network connection is still active.
 *
 * Even if you are not sending messages all that often, you need to be calling
 * this method regularly. This method is used to determine the current state
 * of the scene.
 *
 * @return true if the network connection is still active.
 */
void ClientScene::updateText(const std::shared_ptr<scene2::Button>& button, const std::string text) {
    auto label = std::dynamic_pointer_cast<scene2::Label>(button->getChildByName("up")->getChildByName("label"));
    label->setText(text);

}

#pragma mark -
#pragma mark Student Methods
/**
 * The method called to update the scene.
 *
 * We need to update this method to constantly talk to the server
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void ClientScene::update(float timestep) {
//    if (_network) {
//        _network->receive([this](const std::string source,
//                                 const std::vector<std::byte>& data) {
//            processData(source,data);
//        });
//        checkConnection();
    
    configureStartButton();
    if(_network->getStatus() == NetworkController::Status::CONNECTED){
        _player->setText(std::to_string(_network->getNumPlayers()));
    }
}


///**
// * Connects to the game server as specified in the assets file
// *
// * The {@link #init} method set the configuration data. This method simply uses
// * this to create a new {@Link NetworkConnection}. It also immediately calls
// * {@link #checkConnection} to determine the scene state.
// *
// * @param room  The room ID to use
// *
// * @return true if the connection was successful
// */
//bool ClientScene::connect(const std::string room) {
//    // THIS IS WRONG. FIX ME
//    if(!room.empty()){
//        std::string hexRoomCode = dec2hex(room);
//        _network = cugl::netcode::NetcodeConnection::alloc(_config, hexRoomCode);
//        _network->open();
//        checkConnection();
//        return _network->getState() == NetcodeConnection::State::CONNECTED;
//    }
//    return false;
//}

///**
// * Processes data sent over the network.
// *
// * Once connection is established, all data sent over the network consistes of
// * byte vectors. This function is a call back function to process that data.
// * Note that this function may be called *multiple times* per animation frame,
// * as the messages can come from several sources.
// *
// * Typically this is where players would communicate their names after being
// * connected. In this lab, we only need it to do one thing: communicate that
// * the host has started the game.
// *
// * @param source    The UUID of the sender
// * @param data      The data received
// */
//void ClientScene::processData(const std::string source,
//                              const std::vector<std::byte>& data) {
//    std::vector<std::byte> message{255};
//    if(data == message){
//        _status = START;
//    }
//}

///**
// * Checks that the network connection is still active.
// *
// * Even if you are not sending messages all that often, you need to be calling
// * this method regularly. This method is used to determine the current state
// * of the scene.
// *
// * @return true if the network connection is still active.
// */
//bool ClientScene::checkConnection() {
//    int connectState = static_cast<int>(_network->getState());
//    
//    if(connectState == 1){
//        _status = JOIN;
//    }
//    else if(connectState == 2){
//        if(_status != START){
//            _status = WAIT;
//        }
//    }
//    else if(connectState == 5 || connectState == 6 || connectState == 7 || connectState == 8 || connectState == 9){
//        _status = IDLE;
//        return false;
//    }
//    
//    _player->setText(std::to_string(_network->getNumPlayers()));
//    return true;
//}

/**
 * Reconfigures the start button for this scene
 *
 * This is necessary because what the buttons do depends on the state of the
 * networking.
 */
void ClientScene::configureStartButton() {
    if(_network->getStatus() == NetworkController::Status::IDLE){
        _startgame->setDown(false);
        _startgame->activate();
        //updateText(_startgame,"Start Game");
    }
    else if(_network->getStatus() == NetworkController::Status::CONNECTING){
        _startgame->setDown(false);
        _startgame->deactivate();
        //updateText(_startgame, "Connecting");
    }
    else if(_network->getStatus() == NetworkController::Status::CONNECTED){
        _startgame->setDown(false);
        _startgame->deactivate();
        //updateText(_startgame, "Waiting");
    }
}
