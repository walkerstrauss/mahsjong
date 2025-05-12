//
//  MJGameOverScene.h
//  Mahsjong
//
//  Created by Walker Strauss on 3/15/25.
//

#ifndef __MJ_GAME_OVER_SCENE_H__
#define __MJ_GAME_OVER_SCENE_H__

#include <cugl/cugl.h>
#include "MJAudioController.h"
#include "MJTileSet.h"
#include "MJAnimatedNode.h"

#include "MJAnimationController.h"

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
        LOSE,
        TIE
    };
    
protected:
    /** Reference to asset manager */
    std::shared_ptr<cugl::AssetManager> _assets;
    /** Reference to win scene node */
    std::shared_ptr<cugl::scene2::SceneNode> _winscene;
    /** Reference to lose scene node */
    std::shared_ptr<cugl::scene2::SceneNode> _losescene;
    /** Reference to tie scene node */
    std::shared_ptr<cugl::scene2::SceneNode> _tiescene;
    /** Reference to main button in win scene */
    std::shared_ptr<cugl::scene2::Button> _mainWinBtn;
    /** Reference to quit button in win scene*/
    std::shared_ptr<cugl::scene2::Button> _quitWinBtn;
    /** Reference to main button in lose scene */
    std::shared_ptr<cugl::scene2::Button> _mainLoseBtn;
    /** Reference to quit button in lose scene*/
    std::shared_ptr<cugl::scene2::Button> _quitLoseBtn;
    /** Reference to main button in tie scene*/
    std::shared_ptr<cugl::scene2::Button> _mainTieBtn;
    /** Reference to key for main button in win scene */
    Uint32 _mainWinKey;
    /**Reference to key for main button in tie scene **/
    Uint32 _mainTieKey;
    /** Reference to key for quit button in win scene */
    Uint32 _quitWinKey;
    /** Reference to key for main button in lose scene */
    Uint32 _mainLoseKey;
    /** Reference to key for quit button in lose scene */
    Uint32 _quitLoseKey;
    /** Reference to the animation for winning **/
    std::shared_ptr<cugl::graphics::SpriteSheet> _sheet;
    /** Node for sheet*/
    std::shared_ptr<cugl::scene2::SpriteNode> _sheetNode;
    float _frameTimer = 0.0f;  // Track time between frames
    float _frameDelay = 0.2f;  // Time in seconds before switching frames
    std::vector<std::shared_ptr<TileSet::Tile>> _winningHand;
    std::vector<std::shared_ptr<cugl::scene2::TexturedNode>> _winSlots;
    std::vector<std::shared_ptr<cugl::scene2::TexturedNode>> _loseSlots;
    std::vector<std::shared_ptr<cugl::scene2::TexturedNode>> _tieSlots;
    std::shared_ptr<cugl::scene2::SpriteNode> _grandmaWin;
    std::shared_ptr<cugl::scene2::SpriteNode> _grandmaCry;
    std::shared_ptr<cugl::scene2::SpriteNode> _grandmaTie;
    std::shared_ptr<cugl::scene2::SpriteNode> _rabbit;
    std::shared_ptr<cugl::scene2::SpriteNode> _rat;
    std::shared_ptr<cugl::scene2::SpriteNode> _rabbitLose;
    std::shared_ptr<cugl::scene2::SpriteNode> _ratLose;
    std::shared_ptr<cugl::scene2::SpriteNode> _rabbitTie;
    std::shared_ptr<cugl::scene2::SpriteNode> _ratTie;
    
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
    
    void setWinningHand(const std::vector<std::shared_ptr<TileSet::Tile>>& hand);
    
    void setLosingHand(const std::vector<std::shared_ptr<TileSet::Tile>>& hand);
};

#endif
