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
        std::cerr << "Scene2 initialization failed!" << std::endl;
        return false;
    }
    _assets = assets;
    _settingScene = _assets->get<scene2::SceneNode>("settings");
    _settingScene->setContentSize(Application::get()->getDisplaySize());
    _settingScene->doLayout();
    choice = Choice::NONE;
    scene = PrevScene::NEITHER;
    
    // Initialize all buttons
    _notifOnBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("settings.settingscene.on_notification"));
    _notifOffBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("settings.settingscene.off_notification"));
    _musicOnBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("settings.settingscene.on_music"));
    _musicOffBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("settings.settingscene.off_music"));
    _soundOnBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("settings.settingscene.on_sound"));
    _soundOffBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("settings.settingscene.off_sound"));
    exitBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("settings.settingscene.button_cancel"));
    
    // Set button listeners
    _notifOnKey = _notifOnBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            // TODO: handle turning notifications on
            CULog("Turning notifications on");
        }
    });
    _notifOffKey = _notifOffBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            // TODO: handle turning notifications off
            CULog("Turning notifications off");
        }
    });
    _musicOnKey = _musicOnBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            // TODO: handle turning music on
            CULog("Turning music on");
        }
    });
    _musicOffKey = _musicOffBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            // TODO: handle turning music off
            CULog("Turning music off");
        }
    });
    _soundOnKey = _soundOnBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            // TODO: handle turning sound on
            CULog("Turning sound on");
        }
    });
    _soundOffKey = _soundOffBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            // TODO: handle turning sound on
            CULog("Turning sound off");
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
    exitBtn->setContentSize(Size(300, 150));
    exitBtn->setAnchor(Vec2::ANCHOR_TOP_LEFT);
    exitBtn->setPosition(900,600);
    exitBtn->doLayout();

    addChild(_settingScene);
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this scene.
 */
void SettingScene::dispose(){
//    _notifOnBtn->removeListener(_notifOnKey);
//    _notifOffBtn->removeListener(_notifOffKey);
//    _musicOnBtn->removeListener(_musicOnKey);
//    _musicOffBtn->removeListener(_musicOffKey);
//    _soundOnBtn->removeListener(_soundOnKey);
//    _soundOffBtn->removeListener(_soundOffKey);
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
