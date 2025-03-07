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
    } else if (!Scene2::init()) {
        std::cerr << "Scene2 initialization failed!" << std::endl;
        return false;
    }
    _assets = assets;
    _settingScene = _assets->get<scene2::SceneNode>("settings");
    _settingScene->doLayout();
    std::shared_ptr<scene2::SceneNode> scene = _assets->get<scene2::SceneNode>("settings");
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
        }
    });
    _notifOffKey = _notifOffBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            // TODO: handle turning notifications off
        }
    });
    _musicOnKey = _musicOnBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            // TODO: handle turning music on
        }
    });
    _musicOffKey = _musicOffBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            // TODO: handle turning music off
        }
    });
    _soundOnKey = _soundOnBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            // TODO: handle turning sound on
        }
    });
    _soundOffKey = _soundOffBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            // TODO: handle turning sound on
        }
    });
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this scene.
 */
void SettingScene::dispose(){
    _notifOnBtn->removeListener(_notifOnKey);
    _notifOffBtn->removeListener(_notifOffKey);
    _musicOnBtn->removeListener(_musicOnKey);
    _musicOffBtn->removeListener(_musicOffKey);
    _soundOnBtn->removeListener(_soundOnKey);
    _soundOffBtn->removeListener(_soundOffKey);
    exitBtn->removeListener(exitKey);
    _settingScene = nullptr;
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

/**
 * Draws this scene to the scene's SpriteBatch.
 */
void SettingScene::render(){
    _settingScene->render(_batch);
}
