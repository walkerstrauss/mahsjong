//
//  MJPauseScene.h
//  Mahsjong
//
//  Created by Walker Strauss on 3/14/25.
//

#ifndef __MJ_PAUSE_SCENE_H__
#define __MJ_PAUSE_SCENE_H__

#include <cugl/cugl.h>

using namespace cugl;
using namespace cugl::scene2;

class PauseScene : public cugl::scene2::Scene2{
public:
    enum Choice {
        NONE,
        SETTINGS,
        MENU,
        CONTINUE
    };
    
protected:
    /** The asset manager for this scene. */
    std::shared_ptr<cugl::AssetManager> _assets;
    /** Scene Node for pause scene **/
    std::shared_ptr<cugl::scene2::SceneNode> _pauseScene;
    /** Button for continuing (in the pause scene) */
    std::shared_ptr<cugl::scene2::Button> _continueBtn;
    /** Close button for the pause scene*/
    std::shared_ptr<cugl::scene2::Button> _closePauseBtn;
    /** Settings button for pause scene*/
    std::shared_ptr<cugl::scene2::Button> _settingPauseBtn;
    /** Main menu button for pause scene*/
    std::shared_ptr<cugl::scene2::Button> _menuPauseBtn;
    
    /** Key for continue button */
    Uint32 _continueBtnKey;
    /** Close pause button listener*/
    Uint32 _closePauseBtnKey;
    /** Settings button pause scene listener */
    Uint32 _settingPauseBtnKey;
    /** Menu button for pause scene listener */
    Uint32 _menuPauseBtnKey;
    /** Replay button for pause scene listener*/
public:
    
    Choice choice;
    
    PauseScene() : cugl::scene2::Scene2() {}
    
    bool init(const std::shared_ptr<cugl::AssetManager>& assets);
    
    void dispose() override;
    
    virtual void setActive(bool value) override;
    
    void reset() override;
    
    void update(float timestep) override;
};

#endif
