//
//  MJSettingScene.h
//  Mahsjong
//
//  Created by Walker Strauss on 3/7/25.
//

#ifndef __MJ_SETTING_SCENE_H__
#define __MJ_SETTING_SCENE_H__

#include <cugl/cugl.h>
#include "MJAudioController.h"

using namespace cugl;
using namespace cugl::scene2;

/**
 * The class representing the scene for settings. All settings logic will be handled by this class.
 */
class SettingScene: public cugl::scene2::Scene2{
public:
    enum Choice {
        /** We are in setting scene */
        NONE,
        /** We chose back and we came from menu */
        MENU,
        /** We chose back and we came from pause*/
        PAUSE,
        /** We chose music on */
        MUSICON,
        /** We chose music off */
        MUSICOFF,
        /** We chose sound on */
        SOUNDON,
        /** We chose sound off */
        SOUNDOFF
    };
    
    enum PrevScene {
        NEITHER,
        MAIN,
        PAUSED
    };
        
protected:
    /** A reference to the asset manager */
    std::shared_ptr<cugl::AssetManager> _assets;
    /** A reference to the scene node representing the setting scene */
    std::shared_ptr<cugl::scene2::SceneNode> _settingScene;
    /** The button to turn on and off sound */
    std::shared_ptr<cugl::scene2::Button> _soundBtn;
    /** The button to go back to main menu (if we are in the gamescene) */
    std::shared_ptr<cugl::scene2::Button> _mainBtn;
    /** The listener for the notificiations on button*/
    Uint32 _soundKey;

public:
    /** The button to exit the scene */
    std::shared_ptr<cugl::scene2::Button> exitBtn;
    /** The listener for the exit button*/
    Uint32 exitKey;
    /** Choice of settings scene */
    Choice choice;
    /** Last scene that we came from */
    PrevScene scene;
    
#pragma mark -
#pragma mark Constructors
    /**
     * Initializes the setting scene contents
     *
     * @param assets    the asset manager for initializing the setting scene
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets);
    
    /**
     * Disposes of all (non-static) resources allocated to this scene.
     */
    void dispose() override;

#pragma mark -
#pragma mark Gameplay Handling
    
    virtual void setActive(bool value) override;
    
    /**
     * Resets the setting scene
     */
    void reset() override;
    
    /**
     * Updates the setting scene
     *
     * @param timestep The amount of time (in seconds) since the last frame
     */
    void update(float timestep) override;
};

#endif /* __MJ_SETTING_SCENE_H__ */
