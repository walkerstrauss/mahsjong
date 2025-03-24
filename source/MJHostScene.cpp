//
//  MJHostScene.cpp
//  Mahsjong
//
//  Created by Patrick Choo on 3/3/25.
//
#include <cugl/cugl.h>
#include <iostream>
#include <sstream>

#include "MJHostScene.h"

using namespace cugl;
using namespace cugl::scene2;
using namespace cugl::netcode;
using namespace std;

#pragma mark -
#pragma mark Level Layout

/** Regardless of logo, lock the height to this */
#define SCENE_HEIGHT  720

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
    Uint32 value = strtool::stou32(hex,0,16);
    std::string result = strtool::to_string(value);
    if (result.size() < 5) {
        size_t diff = 5-result.size();
        std::string alt(5,'0');
        for(size_t ii = 0; ii < result.size(); ii++) {
            alt[diff+ii] = result[ii];
        }
        result = alt;
    }
    return result;
}

#pragma mark -
#pragma mark Provided Methods
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
bool HostScene::init(const std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<NetworkController> network) {
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
    std::shared_ptr<scene2::SceneNode> scene = _assets->get<scene2::SceneNode>("host");
    scene->setContentSize(dimen);
    scene->doLayout(); // Repositions the HUD

    _startgame = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("host.hostscene.menu.button1"));
    _backout = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("host.hostscene.menu.button2"));
    
    _tileOne = std::dynamic_pointer_cast<scene2::TexturedNode>(_assets->get<scene2::SceneNode>("host.hostscene.waitingRoom.roomid-tile.host1-roomid-tile"));
    _tileTwo = std::dynamic_pointer_cast<scene2::TexturedNode>(_assets->get<scene2::SceneNode>("host.hostscene.waitingRoom.roomid-tile.host1-roomid-tile_1"));
    _tileThree = std::dynamic_pointer_cast<scene2::TexturedNode>(_assets->get<scene2::SceneNode>("host.hostscene.waitingRoom.roomid-tile.host1-roomid-tile_2"));
    _tileFour = std::dynamic_pointer_cast<scene2::TexturedNode>(_assets->get<scene2::SceneNode>("host.hostscene.waitingRoom.roomid-tile.host1-roomid-tile_3"));

    

    // Program the buttons
    _backout->addListener([this](const std::string& name, bool down) {
        if (down) {
            CULog("YOU HIT ME BITCH");
            _backClicked = true;
            _network->disconnect();
        }
    });

    _startgame->addListener([this](const std::string& name, bool down) {
        if (down) {
            startGame();
        }
    });

    //_gameid = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("host.hostscene.menu.gameid.gameid_textfield.gameid"));
    //_player = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("host.hostscene.menu.player.player_textfield.label"));  
    
    // Create the server configuration
    auto json = _assets->get<JsonValue>("server");
    _config.set(json);
    
    addChild(scene);
    //setActive(true);
    _backout->activate();
    return true;
}

/**
* Converts the hexademical server into specific tile IDs
*/
void HostScene::idSetup(const std::shared_ptr<cugl::scene2::TexturedNode>& tile, const char serverPart) {

    switch (serverPart) {
        case '1':
            tile->setTexture(_assets->get<cugl::graphics::Texture>("one_of_bamboo"));
            break;
        case '2':
            tile->setTexture(_assets->get<cugl::graphics::Texture>("two_of_bamboo"));
            break;
        case '3':
            tile->setTexture(_assets->get<cugl::graphics::Texture>("three_of_bamboo"));
            break;
        case '4':
            tile->setTexture(_assets->get<cugl::graphics::Texture>("four_of_bamboo"));
            break;
        case '5':
            tile->setTexture(_assets->get<cugl::graphics::Texture>("five_of_bamboo"));
            break;
        case '6':
            tile->setTexture(_assets->get<cugl::graphics::Texture>("six_of_bamboo"));
            break;
        case '7':
            tile->setTexture(_assets->get<cugl::graphics::Texture>("seven_of_bamboo"));
            break;
        case '8':
            tile->setTexture(_assets->get<cugl::graphics::Texture>("eight_of_bamboo"));
            break;
        case '9':
            tile->setTexture(_assets->get<cugl::graphics::Texture>("nine_of_bamboo"));
            break;
        case '0':
            tile->setTexture(_assets->get<cugl::graphics::Texture>("one_of_dot"));
            break;
        case 'A':
            tile->setTexture(_assets->get<cugl::graphics::Texture>("two_of_dot"));
            break;
        case 'B':
            tile->setTexture(_assets->get<cugl::graphics::Texture>("three_of_dot"));
            break;
        case 'C':
            tile->setTexture(_assets->get<cugl::graphics::Texture>("four_of_dot"));
            break;
        case 'D':
            tile->setTexture(_assets->get<cugl::graphics::Texture>("five_of_dot"));
            break;
        case 'E':
            tile->setTexture(_assets->get<cugl::graphics::Texture>("six_of_dot"));
            break;
        case 'F':
            tile->setTexture(_assets->get<cugl::graphics::Texture>("seven_of_dot"));
            break;
        default:
            tile->setTexture(_assets->get<cugl::graphics::Texture>("facedown"));
    }
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void HostScene::dispose() {
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
void HostScene::setActive(bool value) {
    if (isActive() != value) {
        Scene2::setActive(value);
        if (value) {
            _backout->activate();
            _network->disconnect();
            _network->connectAsHost();
            _backClicked = false;
        } else {
            _startgame->deactivate();
            //updateText(_startgame, "INACTIVE");
            _backout->deactivate();
            _startgame->setDown(false);
            _backout->setDown(false);
        }
    }
}

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
void HostScene::updateText(const std::shared_ptr<scene2::Button>& button, const std::string text) {
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
void HostScene::update(float timestep) {
    if(_network->getStatus() == NetworkController::Status::CONNECTED){
        if (!_startGameClicked) {
            //updateText(_startgame, "Start Game");
            _startgame->activate();
        }
        else {
            //updateText(_startgame, "Starting");
            _startgame->deactivate();
        }
        //_gameid->setText(hex2dec(_network->getRoomID()));
        //_player->setText(std::to_string(_network->getNumPlayers()));
        std::string networkHex = "zzzz";
        if (_network != nullptr) {
            networkHex = _network->getRoomID();
        }
        
        
        idSetup(_tileOne, networkHex[0]);
        idSetup(_tileTwo, networkHex[1]);
        idSetup(_tileThree, networkHex[2]);
        idSetup(_tileFour, networkHex[3]);
        
    }
}

/**
 * Starts the game.
 *
 * This method is called once the requisite number of players have connected.
 * It locks down the room and sends a "start game" message to all other
 * players.
 */
void HostScene::startGame() {
    CULog("host start game");
    
    _network->startGame();
    _startGameClicked = true;
}


