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
using namespace cugl::audio;
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
    } else if (!Scene2::initWithHint(0,SCENE_HEIGHT)){
        return false;
    }
    
    // Start up the input handler
    _assets = assets;
    _network = network;

    // Acquire the scene built by the asset loader and resize it the scene
    _clientScene1 = _assets->get<scene2::SceneNode>("client");
    _clientScene1->setContentSize(getSize());
    _clientScene1->getChild(0)->setContentSize(_clientScene1->getContentSize());
    cugl::Size screenSize = cugl::Application::get()->getDisplaySize();
    screenSize *= _clientScene1->getContentSize().height/screenSize.height;
    //cugl::Size screenSize = Size(0,SCENE_HEIGHT);
    
    float offset = (screenSize.width -_clientScene1->getWidth())/2;
    _clientScene1->setPosition(offset, _clientScene1->getPosition().y);
    
    if (!Scene2::initWithHint(screenSize)) {
        return false;
    }
    
    _clientScene2 = _assets->get<scene2::SceneNode>("client3");
    _clientScene2->setContentSize(getSize());
    _clientScene2->getChild(0)->setContentSize(_clientScene2->getContentSize());
    
    _clientScene1->setPosition(offset, _clientScene1->getPosition().y);
    addChild(_clientScene2);
    _clientScene2->setVisible(false);
    _clientCheckbox = _assets->get<scene2::SceneNode>("client3.client3Scene.waitingRoom.playerBoard.host1-checkbox");
    _clientCheckbox->setVisible(false);

    _startgame = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.clientscene.buttons.confirm-button"));
    _resetGameID = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.clientscene.buttons.clear-button"));
    _backout = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.clientscene.cancel-box"));
    _prepareOrStart = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("client3.client3Scene.menu.button1.up.start"));

    AudioController::getInstance().init(_assets);

    _backout->addListener([this](const std::string& name, bool down) {
        if (down) {
            _backClicked = true;
            _network->disconnect();
            _gameIDNew[0]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile19"));
            _gameIDNew[0]->setContentSize(75,75);
            _gameIDNew[1]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile19"));
            _gameIDNew[1]->setContentSize(75,75);
            _gameIDNew[2]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile19"));
            _gameIDNew[2]->setContentSize(75,75);
            _gameIDNew[3]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile19"));
            _gameIDNew[3]->setContentSize(75,75);
            _idPos = 0;
//            TODO: play back sound
            AudioController::getInstance().playSound("back",false);
        }
        });

    _resetGameID->addListener([this](const std::string& name, bool down) {
        if (down) {
            AudioController::getInstance().playSound("deselect", false);
            if(_idPos == 0) {
                return;
            }
            _gameIDNew[0]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile19"));
            _gameIDNew[0]->setContentSize(75,75);
            _gameIDNew[1]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile19"));
            _gameIDNew[1]->setContentSize(75,75);
            _gameIDNew[2]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile19"));
            _gameIDNew[2]->setContentSize(75,75);
            _gameIDNew[3]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile19"));
            _gameIDNew[3]->setContentSize(75,75);
            _idPos = 0;            
        }
        });

    _startgame->addListener([=, this](const std::string& name, bool down) {
        if (down) {
            if (_network->getStatus() == NetworkController::Status::IDLE) {
                _network->connectAsClient(tile2hex());
                
                _gameIDNew[0]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile19"));
                _gameIDNew[0]->setContentSize(75,75);
                _gameIDNew[1]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile19"));
                _gameIDNew[1]->setContentSize(75,75);
                _gameIDNew[2]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile19"));
                _gameIDNew[2]->setContentSize(75,75);
                _gameIDNew[3]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile19"));
                _gameIDNew[3]->setContentSize(75,75);
                _idPos = 0;
            }
            //_gameid->releaseFocus();
//          TODO: play confirm sound
            AudioController::getInstance().playSound("confirm",false);
        }
        });

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
    
    _tile1->addListener([this](const std::string& name, bool down) {
        if (down) {
            //_gameIDNew[_idPos]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile"));
            //_idPos++;
            _keySecPage = !_keySecPage;
            if (_keySecPage) {
                _tile9->deactivate();
                _tile10->deactivate();
                changeKeyPadTexture(_tile1, "client1-gameid-tile");
                changeKeyPadTexture(_tile2, "client1-gameid-tile10");
                changeKeyPadTexture(_tile3, "client1-gameid-tile11");
                changeKeyPadTexture(_tile4, "client1-gameid-tile12");
                changeKeyPadTexture(_tile5, "client1-gameid-tile13");
                changeKeyPadTexture(_tile6, "client1-gameid-tile14");
                changeKeyPadTexture(_tile7, "client1-gameid-tile15");
                changeKeyPadTexture(_tile8, "client1-gameid-tile16");

            }
            else {
                changeKeyPadTexture(_tile1, "client1-gameid-tile");
                changeKeyPadTexture(_tile2, "client1-gameid-tile1");
                changeKeyPadTexture(_tile3, "client1-gameid-tile2");
                changeKeyPadTexture(_tile4, "client1-gameid-tile3");
                changeKeyPadTexture(_tile5, "client1-gameid-tile4");
                changeKeyPadTexture(_tile6, "client1-gameid-tile5");
                changeKeyPadTexture(_tile7, "client1-gameid-tile6");
                changeKeyPadTexture(_tile8, "client1-gameid-tile7");
                
                _tile9->activate();
                _tile10->activate();
            }
            _tile9->setVisible(!_keySecPage);
            _tile10->setVisible(!_keySecPage);
//            AudioEngine::get()->play("swap",_assets->get<Sound>("swap"),false,1.0f);
        }
        });
    
    _tile2->addListener([this](const std::string& name, bool down) {
        if (down && _idPos != 4) {
            if(_keySecPage){
                _gameIDNew[_idPos]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile10"));
            }
            else{
                _gameIDNew[_idPos]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile1"));
            }
            _gameIDNew[_idPos]->setContentSize(75, 75);
            _idPos++;
// TODO: play select sound
            AudioController::getInstance().playSound("select", false);
        }
        });
    _tile3->addListener([this](const std::string& name, bool down) {
        if (down && _idPos != 4) {
            if(_keySecPage){
                _gameIDNew[_idPos]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile11"));
            }
            else{
                _gameIDNew[_idPos]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile2"));
            }
            _gameIDNew[_idPos]->setContentSize(75, 75);
            _idPos++;
// TODO: play select sound
            AudioController::getInstance().playSound("select", false);
        }
        });
    
    _tile4->addListener([this](const std::string& name, bool down) {
        if (down && _idPos != 4) {
            if(_keySecPage){
                _gameIDNew[_idPos]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile12"));
            }
            else{
                _gameIDNew[_idPos]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile3"));
            }
            _gameIDNew[_idPos]->setContentSize(75, 75);
            _idPos++;
// TODO: play select sound
            AudioController::getInstance().playSound("select", false);
        }
        });
    _tile5->addListener([this](const std::string& name, bool down) {
        if (down && _idPos != 4) {
            if(_keySecPage){
                _gameIDNew[_idPos]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile13"));
            }
            else{
                _gameIDNew[_idPos]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile4"));
            }
            _gameIDNew[_idPos]->setContentSize(75, 75);
            _idPos++;
// TODO: play select sound
            AudioController::getInstance().playSound("select", false);
        }
        });
    _tile6->addListener([this](const std::string& name, bool down) {
        if (down && _idPos != 4) {
            if(_keySecPage){
                _gameIDNew[_idPos]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile14"));
            }
            else{
                _gameIDNew[_idPos]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile5"));
            }
            _gameIDNew[_idPos]->setContentSize(75, 75);
            _idPos++;
// TODO: play select sound
            AudioController::getInstance().playSound("select", false);
        }
        });
    _tile7->addListener([this](const std::string& name, bool down) {
        if (down && _idPos != 4) {
            if(_keySecPage){
                _gameIDNew[_idPos]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile15"));
            }
            else{
                _gameIDNew[_idPos]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile6"));
            }
            _gameIDNew[_idPos]->setContentSize(75, 75);
            _idPos++;
// TODO: play select sound
            AudioController::getInstance().playSound("select", false);
        }
        });
    _tile8->addListener([this](const std::string& name, bool down) {
        if (down && _idPos != 4) {
            if(_keySecPage){
                _gameIDNew[_idPos]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile16"));
            }
            else{
                _gameIDNew[_idPos]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile7"));
            }
            _gameIDNew[_idPos]->setContentSize(75, 75);
            _idPos++;
// TODO: play select sound
            AudioController::getInstance().playSound("select", false);
        }
        });
    _tile9->addListener([this](const std::string& name, bool down) {
        if (down && _idPos != 4) {
            _gameIDNew[_idPos]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile8"));
            _gameIDNew[_idPos]->setContentSize(75, 75);
            _idPos++;
// TODO: play select sound
            AudioController::getInstance().playSound("select");
        }
        });
    _tile10->addListener([this](const std::string& name, bool down) {
        if (down && _idPos != 4) {
            _gameIDNew[_idPos]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile9"));
            _gameIDNew[_idPos]->setContentSize(75, 75);
            _idPos++;
// TODO: play select sound
            AudioController::getInstance().playSound("select", false);
        }
        });

    // Create the server configuration
    auto json = _assets->get<JsonValue>("server");
    _config.set(json);
    
    addChild(_clientScene1);
    setActive(false);
    return true;
}



/**
* Convert tile information into hexadecmial
*/
std::string ClientScene::tile2hex() {
    if (_idPos != 4) {
         _gameIDNew[0]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile19"));
         _gameIDNew[1]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile19"));
         _gameIDNew[2]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile19"));
         _gameIDNew[3]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile19"));
         _idPos = 0;
         return "";
    }
    std::string hex = "";
    for (int i = 0; i < 4; i++) {
        hex.append(idCreation(_gameIDNew[i]));
    }
    return hex;
}

std::string ClientScene::idCreation(const std::shared_ptr<cugl::scene2::PolygonNode>& tile) {
    auto texture = tile->getTexture();

    // Compare the texture to each asset
    if (texture == _assets->get<cugl::graphics::Texture>("client1-gameid-tile1")) {
        return "1";
    }
    else if (texture == _assets->get<cugl::graphics::Texture>("client1-gameid-tile2")) {
        return "2";
    }
    else if (texture == _assets->get<cugl::graphics::Texture>("client1-gameid-tile3")) {
        return "3";
    }
    else if (texture == _assets->get<cugl::graphics::Texture>("client1-gameid-tile4")) {
        return "4";
    }
    else if (texture == _assets->get<cugl::graphics::Texture>("client1-gameid-tile5")) {
        return "5";
    }
    else if (texture == _assets->get<cugl::graphics::Texture>("client1-gameid-tile6")) {
        return "6";
    }
    else if (texture == _assets->get<cugl::graphics::Texture>("client1-gameid-tile7")) {
        return "7";
    }
    else if (texture == _assets->get<cugl::graphics::Texture>("client1-gameid-tile8")) {
        return "8";
    }
    else if (texture == _assets->get<cugl::graphics::Texture>("client1-gameid-tile9")) {
        return "9";
    }
    else if (texture == _assets->get<cugl::graphics::Texture>("client1-gameid-tile10")) {
        return "0";
    }
    else if (texture == _assets->get<cugl::graphics::Texture>("client1-gameid-tile11")) {
        return "A";
    }
    else if (texture == _assets->get<cugl::graphics::Texture>("client1-gameid-tile12")) {
        return "B";
    }
    else if (texture == _assets->get<cugl::graphics::Texture>("client1-gameid-tile13")) {
        return "C";
    }
    else if (texture == _assets->get<cugl::graphics::Texture>("client1-gameid-tile14")) {
        return "D";
    }
    else if (texture == _assets->get<cugl::graphics::Texture>("client1-gameid-tile15")) {
        return "E";
    }
    else if (texture == _assets->get<cugl::graphics::Texture>("client1-gameid-tile16")) {
        return "F";
    }
    else {
        return ""; // Default case if no match found
    }
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
            _startgame->activate();
            // Don't reset the room id
        } else {
            //_gameid->deactivate();
            _startgame->deactivate();
            _backout->deactivate();
            _startgame->setDown(false);
            _backout->setDown(false);
            _tile1->deactivate();
            _tile2->deactivate();
            _tile3->deactivate();
            _tile4->deactivate();
            _tile5->deactivate();
            _tile6->deactivate();
            _tile7->deactivate();
            _tile8->deactivate();
            _tile9->deactivate();
            _tile10->deactivate();
            _resetGameID->deactivate();
            _startgame->deactivate();
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
    if(_network->getStatus() == NetworkController::Status::CONNECTING){
        _clientScene1->setVisible(false);
        _clientScene2->setVisible(true);

        std::shared_ptr<cugl::scene2::PolygonNode> id1 = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("client3.client3Scene.waitingRoom.roomid-tile.host1-roomid-tile"));
        std::shared_ptr<cugl::scene2::PolygonNode> id2 = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("client3.client3Scene.waitingRoom.roomid-tile.host1-roomid-tile_1"));
        std::shared_ptr<cugl::scene2::PolygonNode> id3 = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("client3.client3Scene.waitingRoom.roomid-tile.host1-roomid-tile_2"));
        std::shared_ptr<cugl::scene2::PolygonNode> id4 = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("client3.client3Scene.waitingRoom.roomid-tile.host1-roomid-tile_3"));
        
        id1->setTexture(_gameIDNew[0]->getTexture());
        id2->setTexture(_gameIDNew[1]->getTexture());
        id3->setTexture(_gameIDNew[2]->getTexture());
        id4->setTexture(_gameIDNew[3]->getTexture());

        _prepareOrStart->setTexture(_assets->get<cugl::graphics::Texture>("prepareButton"));

    } else if (_network->getStatus() == NetworkController::Status::CONNECTED){
        _clientCheckbox->setVisible(true);
        _prepareOrStart->setTexture(_assets->get<cugl::graphics::Texture>("host1-start-button"));
    }
    else {
        _clientScene1->setVisible(true);
        _clientScene2->setVisible(false);
        _clientCheckbox->setVisible(false);
    }
    _prepareOrStart->setContentSize(200, 181);
}

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
        
    } else if(_network->getStatus() == NetworkController::Status::CONNECTING){
        _startgame->setDown(false);
        _startgame->deactivate();
    }
    else if(_network->getStatus() == NetworkController::Status::CONNECTED){
        _startgame->setDown(false);
        _startgame->deactivate();
    }
}

void ClientScene::changeKeyPadTexture(std::shared_ptr<cugl::scene2::Button>& tile, std::string texture){
    auto up = tile->getChildByName("up");
    auto image = up->getChild(0);
    auto textureNode = std::dynamic_pointer_cast<cugl::scene2::TexturedNode>(image);
    textureNode->setTexture(_assets->get<cugl::graphics::Texture>(texture));
    
    textureNode->setContentSize(120, 120);
    textureNode->setAnchor(0, 1);
}
