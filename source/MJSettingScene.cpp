//
//  MJSettingScene.cpp
//  Mahsjong
//
//  Created by Walker Strauss on 3/7/25.
//

#include <cugl/cugl.h>
#include <iostream>
#include <sstream>
#include "MJSettingScene.h"

using namespace cugl;
using namespace cugl::graphics;
using namespace cugl::audio;
using namespace std;

#define SCENE_HEIGHT  720

#pragma mark -
#pragma mark Constructors
/**
 * Initializes the setting scene contents
 *
 * @param assets    the asset manager for initializing the setting scene
 */
bool SettingScene::init(const std::shared_ptr<cugl::AssetManager>& assets){
    if (assets == nullptr) {
        return false;
    } else if (!Scene2::initWithHint(0,SCENE_HEIGHT)) {
        return false;
    }
    _assets = assets;
    _settingScene = _assets->get<scene2::SceneNode>("settings");
    _settingScene->setContentSize(getSize());
    _settingScene->getChild(0)->setContentSize(_settingScene->getContentSize());
    cugl::Size screenSize = cugl::Application::get()->getDisplaySize();
    //cugl::Size screenSize = Size(0,SCENE_HEIGHT);
    
    screenSize *= _settingScene->getContentSize().height/screenSize.height;
    
    float offset = (screenSize.width -_settingScene->getWidth())/2;
    _settingScene->setPosition(offset, _settingScene->getPosition().y);

    AudioController::getInstance().init(_assets);
    
    if (!Scene2::initWithHint(screenSize)) {
        std::cerr << "Scene2 initialization failed!" << std::endl;
        return false;
    }
    
    choice = Choice::NONE;
    scene = PrevScene::NEITHER;
    
    // Initialize all buttons
    
    _soundBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("settings.settingscene.settingSection.menu.button2"));
    exitBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("settings.settingscene.settingSection.menu.button1"));
    
    // Set button listeners
    _soundKey = _soundBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            // TODO: handle turning sound on
            CULog("Turning sound on");
            AudioController::getInstance().playSound("confirm");
            AudioController::getInstance().toggleSound();
        }
    });
    exitKey = exitBtn->addListener([this](const std::string& name, bool down){
        if (!down) {
            switch (scene){
                case PrevScene::PAUSED:
                    choice = Choice::PAUSE;
//                    AudioEngine::get()->play("back", _assets->get<Sound>("back"), false, 1.0f);
                    AudioController::getInstance().playSound("back");
                    break;
                case PrevScene::MAIN:
                    choice = Choice::MENU;
//                    AudioEngine::get()->play("back", _assets->get<Sound>("back"), false, 1.0f);
                    AudioController::getInstance().playSound("back");
                    break;
                case PrevScene::NEITHER:
                    // Do nothing
                    break;
            }
        }
    });

    addChild(_settingScene);
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this scene.
 */
void SettingScene::dispose(){
}

#pragma mark -
#pragma mark Gameplay Handling
/**
 * Resets the setting scene
 */
void SettingScene::reset(){
    init(_assets);
    return;
}

/**
 * Updates the setting scene
 *
 * @param timestep The amount of time (in seconds) since the last frame
 */
void SettingScene::update(float timestep){
    // TODO: add logic for updating setting scene
    return;
}

void SettingScene::setActive(bool value){
    if (value){
        Scene2::setActive(true);
        _settingScene->setVisible(true);
        exitBtn->activate();
        _soundBtn->activate();
    } else {
        Scene2::setActive(false);
        choice = Choice::NONE;
        _settingScene->setVisible(false);
        exitBtn->deactivate();
        _soundBtn->deactivate();
    }
}
