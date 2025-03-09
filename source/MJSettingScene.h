//
//  MJSettingScene.h
//  Mahsjong
//
//  Created by Walker Strauss on 3/7/25.
//

#ifndef __MJ_SETTING_SCENE_H__
#define __MJ_SETTING_SCENE_H__

#include <cugl/cugl.h>

using namespace cugl;
using namespace cugl::scene2;

/**
 * The class representing the scene for settings. All settings logic will be handled by this class.
 */
class SettingScene: public cugl::scene2::Scene2{
protected:
    /** A reference to the asset manager */
    std::shared_ptr<cugl::AssetManager> _assets;
    /** A reference to the scene node representing the setting scene */
    std::shared_ptr<cugl::scene2::SceneNode> _settingScene;
    /** The button to turn on notifications */
    std::shared_ptr<cugl::scene2::Button> _notifOnBtn;
    /** The button to turn off notificatons */
    std::shared_ptr<cugl::scene2::Button> _notifOffBtn;
    /** The button to turn on music */
    std::shared_ptr<cugl::scene2::Button> _musicOnBtn;
    /** The button to turn off music */
    std::shared_ptr<cugl::scene2::Button> _musicOffBtn;
    /** The button to turn on sound */
    std::shared_ptr<cugl::scene2::Button> _soundOnBtn;
    /** The button to turn off sound */
    std::shared_ptr<cugl::scene2::Button> _soundOffBtn;
    /** The listener for the notificiations on button*/
    Uint32 _notifOnKey;
    /** The listener for the notifications off button*/
    Uint32 _notifOffKey;
    /** The listener for the music on button*/
    Uint32 _musicOnKey;
    /** The listener for the music off button*/
    Uint32 _musicOffKey;
    /** The listener for the sound on button*/
    Uint32 _soundOnKey;
    /** The listener for the sound off button*/
    Uint32 _soundOffKey;

public:
    /** The button to exit the scene */
    std::shared_ptr<scene2::Button> exitBtn;
    /** The listener for the exit button*/
    Uint32 exitKey;
    
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
    
    /**
     * Draws this scene to the scene's SpriteBatch.
     */
    void render() override;
};

#endif /* __MJ_SETTING_SCENE_H__ */

