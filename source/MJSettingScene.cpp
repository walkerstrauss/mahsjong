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

#define SCENE_HEIGHT  700

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
    _settingScene->doLayout();
    _settingScene->setPosition((Application::get()->getDisplayWidth() - _settingScene->getWidth()) / 8, _settingScene->getPosition().y);
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
        }
    });
    exitKey = exitBtn->addListener([this](const std::string& name, bool down){
        if (down) {
            switch (scene){
                case PrevScene::PAUSED:
                    choice = Choice::PAUSE;
                    break;
                case PrevScene::MAIN:
                    choice = Choice::MENU;
                    break;
                case PrevScene::NEITHER:
                    // Do nothing
                    break;
            }
        }
    });
//    exitBtn->setContentSize(Size(300, 150));
//    exitBtn->setAnchor(Vec2::ANCHOR_TOP_LEFT);
//    exitBtn->setPosition(900,600);
//    exitBtn->doLayout();

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
    } else {
        Scene2::setActive(false);
        choice = Choice::NONE;
        _settingScene->setVisible(false);
        exitBtn->deactivate();
        exitBtn->setDown(false);
    }
}
