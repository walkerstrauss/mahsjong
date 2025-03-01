//
//  MJGameScene.h
//  Mahsjong
//
//  Created by Patrick Choo on 2/18/25.
//

#ifndef __MJ_GAME_SCENE_H__
#define __MJ_GAME_SCENE_H__

#include <cugl/cugl.h>
#include <vector>
#include <unordered_set>
#include "MJInputController.h"
#include "MJTileSet.h"
#include "MJPlayer.h"
#include "MJPile.h"
#include "MJScoreManager.h"

/**
 * This class is the primary gameplay controller for our game
 *
 * This world should contain all objects, assets, and input controller. Please start
 * including any and all objects and classes that will build our game */
class GameScene: public cugl::scene2::Scene2{
protected:
    /** Asset manager for this game mode */
    std::shared_ptr<cugl::AssetManager> _assets;
    /** Input controller for player input*/
    InputController _input;
    /** JSON with all of our constants*/
    std::shared_ptr<cugl::JsonValue> _constants;
    /** TileSet for the game */
    std::shared_ptr<TileSet> _tileSet;
    /** Reference to player */
    std::shared_ptr<Player> _player;
    /** Reference to tile pile */
    std::shared_ptr<Pile> _pile;
    /** Reference to texture for gma text*/
    std::shared_ptr<cugl::graphics::Texture> _gmaLabelTexture;
  
    std::shared_ptr<cugl::graphics::TextLayout> _text;
    
    Hand* _hand; // pointer to the hand.
    
    std::shared_ptr<cugl::graphics::TextLayout> _win;
    std::shared_ptr<cugl::graphics::TextLayout> _lose;
    bool _gameWin;
    bool _gameLose;
        
public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new game mode with the default values
     *
     * This constructor does not allocate any objects or start the game.
     * This allows us to use the object without a heap pointer
     */
    GameScene() : cugl::scene2::Scene2() {}
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose() override;
    
    /**
     * Initializes the controller contents, and starts the game
     *
     * The constructor does not allocate any objects or memory. This allows
     * us to have a non-pointer reference to this controller, reducing our memory
     * allocation. Instead, allocation happens in this method
     *
     * @param assets    the asset manager for the game
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets);
    
#pragma mark -
#pragma mark Gameplay Handling
    /**
     * Rests the status of the game so we can play again.
     */
    void reset() override;
    
    /**
     * The method called to update the game mode
     *
     * @param timestep The amount of time (in seconds) since the last frame
     */
    void update(float timestep) override;

    /**
     * Draws all this scene to the scene's SpriteBatch.
     */
    void render() override;
};

#endif /* __SG_GAME_SCENE_H__ */
