//
//  MJGameOverScene.h
//  Mahsjong
//
//  Created by Walker Strauss on 3/15/25.
//

#ifndef __MJ_GAME_OVER_SCENE_H__
#define __MJ_GAME_OVER_SCENE_H__

#include <cugl/cugl.h>

using namespace cugl;
using namespace cugl::scene2;

/**
 * Class representing the game over scene (win and lose) for the game
 */
class GameOverScene : public cugl::scene2::Scene2 {
public:
    /**
     * Enum representing the player's choice in the game over scene
     */
    enum Choice {
        NONE,
        MENU,
        QUIT
    };
    
    enum Type {
        NEITHER,
        WIN,
        LOSE
    };
    
protected:
    /** Reference to asset manager */
    std::shared_ptr<cugl::AssetManager> _assets;
    /** Reference to win scene node */
    std::shared_ptr<cugl::scene2::SceneNode> _winscene;
    /** Reference to lose scene node */
    std::shared_ptr<cugl::scene2::SceneNode> _losescene;
    /** Reference to main button in win scene */
    std::shared_ptr<cugl::scene2::Button> _mainWinBtn;
    /** Reference to quit button in win scene*/
    std::shared_ptr<cugl::scene2::Button> _quitWinBtn;
    /** Reference to main button in lose scene */
    std::shared_ptr<cugl::scene2::Button> _mainLoseBtn;
    /** Reference to quit button in lose scene*/
    std::shared_ptr<cugl::scene2::Button> _quitLoseBtn;
    /** Reference to key for main button in win scene */
    Uint32 _mainWinKey;
    /** Reference to key for quit button in win scene */
    Uint32 _quitWinKey;
    /** Reference to key for main button in lose scene */
    Uint32 _mainLoseKey;
    /** Reference to key for quit button in lose scene */
    Uint32 _quitLoseKey;
    
public:
    /** Represents the choice for this scene */
    Choice choice;
    /** Represents the type of this scene - win vs. lose scene */
    Type type;
    
#pragma mark -
#pragma mark Constructors
    
    GameOverScene() : cugl::scene2::Scene2() {}
    
    bool init(const std::shared_ptr<cugl::AssetManager>& assets);
    
    void dispose() override;
    
#pragma mark -
#pragma mark Gameplay Handling
    
    virtual void setActive(bool value) override;
    
    void update(float timestep) override;
    
    void reset() override;
    
    void render(const std::shared_ptr<cugl::graphics::SpriteBatch>& batch);
};

#endif
