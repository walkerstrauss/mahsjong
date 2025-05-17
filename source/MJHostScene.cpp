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
    } else if (!Scene2::initWithHint(0,SCENE_HEIGHT)){
        return false;
    }
    
    // Start up the input handler
    _assets = assets;
    _network = network;

    // Acquire the scene built by the asset loader and resize it the scene
    _hostScene1 = _assets->get<scene2::SceneNode>("host");
    _hostScene1->setContentSize(getSize());
    _hostScene1->getChild(0)->setContentSize(_hostScene1->getContentSize());
    
    cugl::Size screenSize = cugl::Application::get()->getDisplaySize();
    screenSize *= _hostScene1->getContentSize().height/screenSize.height;
    //cugl::Size screenSize = Size(0,SCENE_HEIGHT);
    
    float offset = (screenSize.width -_hostScene1->getWidth())/2;
    _hostScene1->setPosition(offset, _hostScene1->getPosition().y);
    
    if (!Scene2::initWithHint(screenSize)) {
        return false;
    }
    
    //scene->setContentSize(dimen);
//    scene->setContentSize(screenSize);

    _hostScene1->doLayout(); // Repositions the HUD
    
    //_startgame = std::dynamic_pointer_cast<scene2::Button>(
    //    _assets->get<scene2::SceneNode>("host.host1Scene.waitingRoom.menu.start")
    //);

    _backout = std::dynamic_pointer_cast<scene2::Button>(
        _assets->get<scene2::SceneNode>("host.host1Scene.waitingRoom.menu.exit")
    );

    //_waitOrStart1 = std::dynamic_pointer_cast<scene2::PolygonNode>(
    //    _assets->get<scene2::SceneNode>("host.host1Scene.waitingRoom.menu.start.up.host1-waiting-button")
    //);
    
    _tileOne = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("host.host1Scene.waitingRoom.roomid-tile.host1-roomid-tile"));
    _tileTwo = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("host.host1Scene.waitingRoom.roomid-tile.host1-roomid-tile_1"));
    _tileThree = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("host.host1Scene.waitingRoom.roomid-tile.host1-roomid-tile_2"));
    _tileFour = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("host.host1Scene.waitingRoom.roomid-tile.host1-roomid-tile_3"));
    _tileFive = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("host.host1Scene.waitingRoom.roomid-tile.host1-roomid-tile_4"));

    _tileOne3 = std::dynamic_pointer_cast<PolygonNode>(
        _assets->get<SceneNode>("host3.host3Scene.waitingRoom.roomid-tile.host1-roomid-tile")
    );
    _tileTwo3 = std::dynamic_pointer_cast<PolygonNode>(
        _assets->get<SceneNode>("host3.host3Scene.waitingRoom.roomid-tile.host1-roomid-tile_1")
    );
    _tileThree3 = std::dynamic_pointer_cast<PolygonNode>(
        _assets->get<SceneNode>("host3.host3Scene.waitingRoom.roomid-tile.host1-roomid-tile_2")
    );
    _tileFour3 = std::dynamic_pointer_cast<PolygonNode>(
        _assets->get<SceneNode>("host3.host3Scene.waitingRoom.roomid-tile.host1-roomid-tile_3")
    );
    _tileFive3 = std::dynamic_pointer_cast<PolygonNode>(
        _assets->get<SceneNode>("host3.host3Scene.waitingRoom.roomid-tile.host1-roomid-tile_4")
    );

    
    //_playerSingle = _assets->get<scene2::SceneNode>("host.host1Scene.waitingRoom.playerBoard");

    _hostScene3 = _assets->get<scene2::SceneNode>("host3");
    _hostScene3->setContentSize(getSize());
    _hostScene3->getChild(0)->setContentSize(_hostScene3->getContentSize());
    _hostScene3->setPosition(offset, _hostScene3->getPosition().y);
    _hostScene3->doLayout();


    addChild(_hostScene3);
    _hostScene3->setVisible(false);

    _startgame2 = std::dynamic_pointer_cast<scene2::Button>(
        _assets->get<scene2::SceneNode>("host3.host3Scene.waitingRoom.menu.start")
    );

    _backout2 = std::dynamic_pointer_cast<scene2::Button>(
        _assets->get<scene2::SceneNode>("host3.host3Scene.waitingRoom.menu.exit")
    );


    // Program the buttons
    _backout->addListener([this](const std::string& name, bool down) {
        if (down) {
            _backClicked = true;
            AudioController::getInstance().playSound("Exit");
            _network->disconnect();
        }
    });

    //_startgame->addListener([this](const std::string& name, bool down) {
    //    if (down) {
    //        AudioController::getInstance().playSound("Confirm");
    //        startGame();
    //    }
    //});

    _backout2->addListener([this](const std::string& name, bool down) {
        if (down) {
            _backClicked = true;
            AudioController::getInstance().playSound("Exit");
            _network->disconnect();
        }
        });

    _startgame2->addListener([this](const std::string& name, bool down) {
        if (down) {
            AudioController::getInstance().playSound("Confirm");
            startGame();
        }
        });

    //_gameid = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("host.hostscene.menu.gameid.gameid_textfield.gameid"));
    //_player = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("host.hostscene.menu.player.player_textfield.label"));  
    
    // Create the server configuration
    auto json = _assets->get<JsonValue>("server");
    _config.set(json);
    addChild(_hostScene1);
    setActive(false);
    
    return true;
}

/**
* Converts the hexademical server into specific tile IDs
*/
void HostScene::idSetup(const std::shared_ptr<cugl::scene2::PolygonNode>& tile, const char serverPart) {
    switch (serverPart) {
        case '1':
            tile->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile1"));
            break;
        case '2':
            tile->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile2"));
            break;
        case '3':
            tile->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile3"));
            break;
        case '4':
            tile->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile4"));
            break;
        case '5':
            tile->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile5"));
            break;
        case '6':
            tile->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile6"));
            break;
        case '7':
            tile->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile7"));
            break;
        case '8':
            tile->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile8"));
            break;
        case '9':
            tile->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile9"));
            break;
        case '0':
            tile->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile"));
            break;
        default:
            tile->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-blank"));
    }
        tile->setContentSize(50, 67);
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
            //_startgame->deactivate();
            _startgame2->deactivate();
            //updateText(_startgame, "INACTIVE");
            _backout->deactivate();
            _backout2->deactivate();
            //_startgame->setDown(false);
            _backout->setDown(false);
            _startgame2->setDown(false);
            _backout2->setDown(false);
            _startGameClicked = false;
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
 * The method called to update thawe scene.
 *
 * We need to update this method to constantly talk to the server
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void HostScene::update(float timestep) {
    std::string networkHex = "zzzz";
    if(_network->getStatus() == NetworkController::Status::CONNECTED){
        if (!_startGameClicked) {
            //updateText(_startgame, "Start Game");
            _startgame2->activate();
        }  
        if (_network != nullptr) {
            networkHex = _network->getRoomID();
        }
        if (_network->getNumPlayers() > 1) {
            if(!_playedSound){
                _playedSound = true;
                AudioController::getInstance().playSound("PlayerJoined");
            }
            _hostScene3->setVisible(true);
            _backout2->activate();
            //_startgame->activate();
            _hostScene1->setVisible(false);

            CULog("Testing");
        }
        else {
            _startgame2->deactivate();
            _hostScene3->setVisible(false);
            _hostScene1->setVisible(true);
        }
        _assets->get<scene2::SceneNode>("host.host1Scene.waitingRoom.menu.start.up.host1-waiting-button")->setContentSize(150, 100);
        _assets->get<scene2::SceneNode>("host3.host3Scene.waitingRoom.menu.start.up.start")->setContentSize(150, 150);

        std::string dec = hex2dec(networkHex);
        
        idSetup(_tileOne, dec[0]);
        idSetup(_tileTwo, dec[1]);
        idSetup(_tileThree, dec[2]);
        idSetup(_tileFour, dec[3]);
        idSetup(_tileFive, dec[4]);

        idSetup(_tileOne3, dec[0]);
        idSetup(_tileTwo3, dec[1]);
        idSetup(_tileThree3, dec[2]);
        idSetup(_tileFour3, dec[3]);
        idSetup(_tileFive3, dec[4]);
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
    _network->startGame();
    _startGameClicked = true;
}



