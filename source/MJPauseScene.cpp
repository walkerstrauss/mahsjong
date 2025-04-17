//
//  MJPauseScene.cpp
//  Mahsjong
//
//  Created by Walker Strauss on 3/15/25.
//

#include <cugl/cugl.h>
#include <iostream>
#include <sstream>
#include "MJPauseScene.h"

using namespace cugl;
using namespace cugl::scene2;
using namespace cugl::graphics;
using namespace cugl::audio;

bool PauseScene::init(const std::shared_ptr<cugl::AssetManager>& assets){
    if (!assets){
        return false;
    } else if (!Scene2::initWithHint(0,720)){
        return false;
    }
    
    _assets = assets;
//    Size dimen = getSize();
    _pauseScene = _assets->get<scene2::SceneNode>("pause");
    _pauseScene->setContentSize(1280,720);
    cugl::Size screenSize = cugl::Application::get()->getDisplaySize();
    //cugl::Size screenSize = Size(0,SCENE_HEIGHT);
    
    screenSize *= _pauseScene->getContentSize().height/screenSize.height;
    
    float offset = (screenSize.width -_pauseScene->getWidth())/2;
    _pauseScene->setPosition(offset, _pauseScene->getPosition().y);

    
    if (!Scene2::initWithHint(screenSize)) {
        std::cerr << "Scene2 initialization failed!" << std::endl;
        return false;
    }
//    _pauseScene->setContentSize(dimen);
//    _pauseScene->doLayout();
//    
//    _pauseScene->setPosition((Application::get()->getDisplayWidth() - _pauseScene->getWidth()) / 8, _pauseScene->getPosition().y);
//    _pauseScene->setPosition(125, _pauseScene->getPositionY());
    // Set initial choice to none
    choice = Choice::NONE;
    
    // Initialize buttons and add listeners
    _continueBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("pause.pausescene.pauseSection.menu.button4"));
    _closePauseBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("pause.pausescene.pauseSection.menu.button1"));
    _settingPauseBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("pause.pausescene.pauseSection.menu.button3"));
    _menuPauseBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("pause.pausescene.pauseSection.menu.button2"));
    _continueBtn->setPosition(_continueBtn->getPositionX(), _continueBtn->getPositionY() + 50);
    _menuPauseBtn->setPosition(_menuPauseBtn->getPositionX(), _menuPauseBtn->getPositionY() + 50);
    _settingPauseBtn->setPosition(_settingPauseBtn->getPositionX(), _settingPauseBtn->getPositionY() + 50);

    AudioController::getInstance().init(_assets);
    
    _sound = _assets->get<audio::Sound>("confirm");
    
    _continueBtnKey = _continueBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            choice = Choice::CONTINUE;
//            AudioEngine::get()->play("confirm", _sound, false, 1.0f);
            AudioController::getInstance().playSound("confirm");
        }
    });
    _closePauseBtnKey = _closePauseBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            choice = Choice::CONTINUE;
//            AudioEngine::get()->play("confirm", _sound, false, 1.0f);
            AudioController::getInstance().playSound("confirm");
        }
    });
    _settingPauseBtnKey = _settingPauseBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            choice = Choice::SETTINGS;
//            AudioEngine::get()->play("confirm", _sound, false, 1.0f);
            AudioController::getInstance().playSound("confirm");
        }
    });
    _menuPauseBtnKey = _menuPauseBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            choice = Choice::MENU;
//            AudioEngine::get()->play("confirm", _sound, false, 1.0f);
            AudioController::getInstance().playSound("confirm");
        }
    });
    _closePauseBtn->removeFromParent();
    addChild(_pauseScene);
    setActive(false);
    return true;
}

void PauseScene::dispose(){
    return;
}

void PauseScene::setActive(bool value){
    if (isActive() != value){
        Scene2::setActive(value);
        if (value){
            _continueBtn->activate();
            _closePauseBtn->activate();
            _settingPauseBtn->activate();
            _menuPauseBtn->activate();
            _pauseScene->setVisible(true);
        } else {
            _continueBtn->deactivate();
            _closePauseBtn->deactivate();
            _settingPauseBtn->deactivate();
            _menuPauseBtn->deactivate();
            _pauseScene->setVisible(false);
            choice = Choice::NONE;
        }
    }
}

void PauseScene::reset(){
    return;
}

void PauseScene::update(float timestep){
    return;
}

