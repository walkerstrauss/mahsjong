//
//  MJTutorialScene.h
//  Mahsjong
//
//  Created by Walker Strauss on 4/22/25.
//

#ifndef __MJ_TUTORIAL_SCENE_H__
#define __MJ_TUTORIAL_SCENE_H__
#include <cugl/cugl.h>
#include "MJInputController.h"

using namespace cugl;
using namespace cugl::scene2;
using namespace cugl::graphics;

/** Class representing the tutorial scene for the game */
class TutorialScene : public Scene2 {
protected:
    std::shared_ptr<AssetManager> _assets;
    std::shared_ptr<cugl::scene2::SceneNode> _tutorialScene;
    std::shared_ptr<cugl::scene2::Button> _exit;
    std::shared_ptr<cugl::scene2::Button> _back;

    std::shared_ptr<cugl::scene2::PolygonNode> _presentation;
    std::vector<std::string> _slides;
    int _slide;

    InputController _input;
    
public:
#pragma mark Constructors
    TutorialScene() : Scene2(){}
    
    bool init(const std::shared_ptr<AssetManager>& assets);

    enum Choice {
        NONE,
        BACK
    };
    Choice _choice;
    Choice getChoice() const { return _choice; }
    
#pragma mark Gameplay Handling
    void update(float timestep) override;
    
    virtual void setActive(bool value) override;
    void dispose() override;
};
    
#endif
