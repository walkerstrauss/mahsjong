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
    _clientScene2->setPosition(offset, _clientScene2->getPosition().y);
    
    addChild(_clientScene2);
    _clientScene2->setVisible(false);
    _clientCheckbox = _assets->get<scene2::SceneNode>("client3.client3Scene.waitingRoom.playerBoard.player2.host1-checkbox");
    _clientCheckbox->setVisible(false);

    _id1 = std::dynamic_pointer_cast<scene2::PolygonNode>(
        _assets->get<scene2::SceneNode>(
            "client3.client3Scene.waitingRoom.roomid-tile.host1-roomid-tile"
        )
    );
    _id2 = std::dynamic_pointer_cast<scene2::PolygonNode>(
        _assets->get<scene2::SceneNode>(
            "client3.client3Scene.waitingRoom.roomid-tile.host1-roomid-tile_1"
        )
    );
    _id3 = std::dynamic_pointer_cast<scene2::PolygonNode>(
        _assets->get<scene2::SceneNode>(
            "client3.client3Scene.waitingRoom.roomid-tile.host1-roomid-tile_2"
        )
    );
    _id4 = std::dynamic_pointer_cast<scene2::PolygonNode>(
        _assets->get<scene2::SceneNode>(
            "client3.client3Scene.waitingRoom.roomid-tile.host1-roomid-tile_3"
        )
    );
    _id5 = std::dynamic_pointer_cast<scene2::PolygonNode>(
        _assets->get<scene2::SceneNode>(
            "client3.client3Scene.waitingRoom.roomid-tile.host1-roomid-tile_4"
        )
    );


    _startgame = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.client1Scene.buttons.confirm-button"));
    _resetGameID = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.client1Scene.buttons.cancel-button"));
    _backout = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.client1Scene.board.cancel-box"));
    _prepareOrStart = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("client3.client3Scene.waitingRoom.menu.start.up.start"));

    //AudioController::getInstance().init(_assets);

    _backout->addListener([this](const std::string& name, bool down) {
        if (down) {
            _backClicked = true;
            _network->disconnect();
            for (int i = 0; i < 5; ++i) {
                //Size orig = _gameIDNew[_idPos]->getContentSize();
                _gameIDNew[i]->setTexture(_assets->get<Texture>("client1-gameid-blank"));
                //_gameIDNew[i]->setContentSize(orig);
                _gameIDNew[i]->setContentSize(45, 60);
            }
            _idPos = 0;
            AudioController::getInstance().playSound("Exit", false);
        }
        });

    _resetGameID->addListener([this](const std::string& name, bool down) {
        if (!down) return;  // wait for the press event
        AudioController::getInstance().playSound("Clear", false);

        // Always clear all 5 slots, regardless of _idPos.
        auto blank = _assets->get<cugl::graphics::Texture>("client1-gameid-blank");
        for (int i = 0; i < 5; ++i) {
            //Size orig = _gameIDNew[_idPos]->getContentSize();
            _gameIDNew[i]->setTexture(blank);
            //_gameIDNew[i]->setContentSize(orig);
            _gameIDNew[i]->setContentSize(45, 60);
        }
        _idPos = 0;
        });


    _startgame->addListener([=, this](const std::string& name, bool down) {
        if (down) {
            if (_network->getStatus() == NetworkController::Status::IDLE) {
                _joinHex = tile2hex();
                _network->connectAsClient(_joinHex);
                // Reset all five blanks
                for (int i = 0; i < 5; ++i) {
                    auto blankTex = _assets->get<cugl::graphics::Texture>("client1-gameid-blank");
                    //Size orig = _gameIDNew[_idPos]->getContentSize();
                    _gameIDNew[i]->setTexture(blankTex);
                    //_gameIDNew[i]->setContentSize(orig);
                    _gameIDNew[i]->setContentSize(45, 60);
                }
                _idPos = 0;
            }
            AudioController::getInstance().playSound("Confirm", false);
        }
        });


    // Tile GameID
    _gameIDNew[0] = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("client.client1Scene.board.client1-gameid-blank.client1-gameid-blank"));
    _gameIDNew[1] = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("client.client1Scene.board.client1-gameid-blank.client1-gameid-blank_1"));
    _gameIDNew[2] = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("client.client1Scene.board.client1-gameid-blank.client1-gameid-blank_2"));
    _gameIDNew[3] = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("client.client1Scene.board.client1-gameid-blank.client1-gameid-blank_3"));
    _gameIDNew[4] = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("client.client1Scene.board.client1-gameid-blank.client1-gameid-blank_4"));
    _idPos = 0;

    // Tile Keypad
    _tile1 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.client1Scene.board.gameid-pad.pad-tile"));
    _tile2 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.client1Scene.board.gameid-pad.pad-tile1"));
    _tile3 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.client1Scene.board.gameid-pad.pad-tile2"));
    _tile4 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.client1Scene.board.gameid-pad.pad-tile3"));
    _tile5 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.client1Scene.board.gameid-pad.pad-tile4"));
    _tile6 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.client1Scene.board.gameid-pad.pad-tile5"));
    _tile7 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.client1Scene.board.gameid-pad.pad-tile6"));
    _tile8 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.client1Scene.board.gameid-pad.pad-tile7"));
    _tile9 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.client1Scene.board.gameid-pad.pad-tile8"));
    _tile10 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.client1Scene.board.gameid-pad.pad-tile9"));
    
    
   
    _tile1->addListener([this](const std::string& name, bool down) {
        if (down && _idPos < 5) {
            // “0” is texture client1-gameid-tile10
            auto tex = _assets->get<Texture>("client1-gameid-tile");
            _gameIDNew[_idPos]->setTexture(tex);
            //_gameIDNew[_idPos]->setContentSize(orig);
            _gameIDNew[_idPos]->setContentSize(45, 60);
            _idPos++;
            AudioController::getInstance().playSound("Click", false);
        }
        });

    _tile2->addListener([this](const std::string& name, bool down) {
        if (down && _idPos < 5) {
            auto tex = _assets->get<Texture>("client1-gameid-tile1");
            _gameIDNew[_idPos]->setTexture(tex);
            _gameIDNew[_idPos]->setContentSize(45, 60);
            _idPos++;
            AudioController::getInstance().playSound("Click", false);
        }
        });
    _tile3->addListener([this](const std::string& name, bool down) {
        if (down && _idPos < 5) {
            //Size orig = _gameIDNew[_idPos]->getContentSize();
            _gameIDNew[_idPos]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile2"));
            //_gameIDNew[_idPos]->setContentSize(orig);
            _gameIDNew[_idPos]->setContentSize(45, 60);
            _idPos++;
            AudioController::getInstance().playSound("Click", false);
        }
        });
    
    _tile4->addListener([this](const std::string& name, bool down) {
        if (down && _idPos < 5) {
            //Size orig = _gameIDNew[_idPos]->getContentSize();
            _gameIDNew[_idPos]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile3"));
            //_gameIDNew[_idPos]->setContentSize(orig);
            _gameIDNew[_idPos]->setContentSize(45, 60);
            _idPos++;
            AudioController::getInstance().playSound("Click", false);
        }
        });
    _tile5->addListener([this](const std::string& name, bool down) {
        if (down && _idPos < 5) {
            //Size orig = _gameIDNew[_idPos]->getContentSize();
            _gameIDNew[_idPos]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile4"));
            //_gameIDNew[_idPos]->setContentSize(orig);
            _gameIDNew[_idPos]->setContentSize(45, 60);
            _idPos++;
            AudioController::getInstance().playSound("Click", false);
        }
        });
    _tile6->addListener([this](const std::string& name, bool down) {
        if (down && _idPos < 5) {
            //Size orig = _gameIDNew[_idPos]->getContentSize();
            _gameIDNew[_idPos]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile5"));
            //_gameIDNew[_idPos]->setContentSize(orig);
            _gameIDNew[_idPos]->setContentSize(45, 60);
            _idPos++;
            AudioController::getInstance().playSound("Click", false);
        }
        });
    _tile7->addListener([this](const std::string& name, bool down) {
        if (down && _idPos < 5) {
            //Size orig = _gameIDNew[_idPos]->getContentSize();
            _gameIDNew[_idPos]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile6"));
            //_gameIDNew[_idPos]->setContentSize(orig);
            _gameIDNew[_idPos]->setContentSize(45, 60);
            _idPos++;
            AudioController::getInstance().playSound("Click", false);
        }
        });
    _tile8->addListener([this](const std::string& name, bool down) {
        if (down && _idPos < 5) {
            //Size orig = _gameIDNew[_idPos]->getContentSize();
            _gameIDNew[_idPos]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile7"));
            //_gameIDNew[_idPos]->setContentSize(orig);
            _gameIDNew[_idPos]->setContentSize(45, 60);
            _idPos++;
            AudioController::getInstance().playSound("Click", false);
        }
        });
    _tile9->addListener([this](const std::string& name, bool down) {
        if (down && _idPos < 5) {
            //Size orig = _gameIDNew[_idPos]->getContentSize();
            _gameIDNew[_idPos]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile8"));
            //_gameIDNew[_idPos]->setContentSize(orig);
            _gameIDNew[_idPos]->setContentSize(45, 60);
            _idPos++;
            AudioController::getInstance().playSound("Click");
        }
        });
    _tile10->addListener([this](const std::string& name, bool down) {
        if (down && _idPos < 5) {
            //Size orig = _gameIDNew[_idPos]->getContentSize();
            _gameIDNew[_idPos]->setTexture(_assets->get<cugl::graphics::Texture>("client1-gameid-tile9"));
            //_gameIDNew[_idPos]->setContentSize(orig);
            _gameIDNew[_idPos]->setContentSize(45, 60);
            _idPos++;
            AudioController::getInstance().playSound("Click", false);
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
 * Convert the 5 entered decimal digits into the 4-char hexadecimal room ID.
 */
std::string ClientScene::tile2hex() {
    // If we don’t yet have 5 digits, clear all five blanks and bail
    if (_idPos != 5) {
        for (int i = 0; i < 5; ++i) {
            auto blank = _assets->get<cugl::graphics::Texture>("client1-gameid-blank");
            //Size orig = _gameIDNew[_idPos]->getContentSize();
            _gameIDNew[i]->setTexture(blank);
            //_gameIDNew[i]->setContentSize(orig);
            _gameIDNew[i]->setContentSize(45, 60);
        }
        _idPos = 0;
        return "";
    }

    // Build the decimal string from the 5 tiles
    std::string dec;
    dec.reserve(5);
    for (int i = 0; i < 5; ++i) {
        // idCreation(...) returns a single-character string ("0"–"9")
        dec.push_back(idCreation(_gameIDNew[i])[0]);
    }

    // Convert that 5-digit decimal into a zero-padded 4-char hex string
    return dec2hex(dec);
}

std::string ClientScene::idCreation(const std::shared_ptr<scene2::PolygonNode>& tile) {
    auto texture = tile->getTexture();
    // Map only your 0–9 textures
    if (texture == _assets->get<Texture>("client1-gameid-tile1"))  return "1";
    else if (texture == _assets->get<Texture>("client1-gameid-tile2"))  return "2";
    else if (texture == _assets->get<Texture>("client1-gameid-tile3"))  return "3";
    else if (texture == _assets->get<Texture>("client1-gameid-tile4"))  return "4";
    else if (texture == _assets->get<Texture>("client1-gameid-tile5"))  return "5";
    else if (texture == _assets->get<Texture>("client1-gameid-tile6"))  return "6";
    else if (texture == _assets->get<Texture>("client1-gameid-tile7"))  return "7";
    else if (texture == _assets->get<Texture>("client1-gameid-tile8"))  return "8";
    else if (texture == _assets->get<Texture>("client1-gameid-tile9"))  return "9";
    else if (texture == _assets->get<Texture>("client1-gameid-tile10")) return "0";

    // If we ever hit this, something went wrong in the listener setup
    CUAssertLog(false, "Unexpected tile texture in idCreation");
    return "";
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
    configureStartButton();
    auto status = _network->getStatus();
    if (_network->getStatus() == NetworkController::Status::CONNECTING || status == NetworkController::Status::CONNECTED) {
        _clientScene1->setVisible(false);
        _clientScene2->setVisible(true);

        _id1->setTexture(_gameIDNew[0]->getTexture());
        _id2->setTexture(_gameIDNew[1]->getTexture());
        _id3->setTexture(_gameIDNew[2]->getTexture());
        _id4->setTexture(_gameIDNew[3]->getTexture());
        _id5->setTexture(_gameIDNew[4]->getTexture());

        if (status == NetworkController::Status::CONNECTING) {
            // Still waiting on the host to start
            _clientCheckbox->setVisible(false);
            _prepareOrStart->setTexture(
                _assets->get<Texture>("prepareButton")
            );
        }
        else {
            _clientCheckbox->setVisible(true);
            _prepareOrStart->setTexture(
                _assets->get<Texture>("host1-start-button")
            );
        }
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
