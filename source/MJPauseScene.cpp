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

bool PauseScene::init(const std::shared_ptr<cugl::AssetManager>& assets){
    if (!assets){
        return false;
    } else if (!Scene2::initWithHint(0,700)){
        return false;
    }
    
    _assets = assets;
    Size dimen = getSize();
    _pauseScene = _assets->get<scene2::SceneNode>("pause");
    _pauseScene->setContentSize(dimen);
    _pauseScene->doLayout();
    
    // Set initial choice to none
    choice = Choice::NONE;
    
    // Initialize buttons and add listeners
    _continueBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("pause.pausescene.button_continue"));
    _closePauseBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("pause.pausescene.button_cancel"));
    _settingPauseBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("pause.pausescene.button_setting"));
    _menuPauseBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("pause.pausescene.button_main"));
    
    _continueBtnKey = _continueBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            choice = Choice::CONTINUE;
        }
    });
    _closePauseBtnKey = _closePauseBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            choice = Choice::CONTINUE;
        }
    });
    _settingPauseBtnKey = _settingPauseBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            choice = Choice::SETTINGS;
        }
    });
    _menuPauseBtnKey = _menuPauseBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            choice = Choice::MENU;
        }
    });
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

