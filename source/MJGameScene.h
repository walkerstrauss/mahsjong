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
    
    /** Controllers are attatched directly to the scene (no pointers) */
    /** TODO: Please further implement input controller; see MJInputController*/
    InputController _input;
    
    /** JSON with all of our constants*/
    std::shared_ptr<cugl::JsonValue> _constants;
    
    /** TODO: Initialize new class objects such as tile, pile, etc.
     * Remember, please use shared pointers for disposable objects to make our lives easier!
     */
    std::shared_ptr<TileSet> _tileSet;
    
    std::shared_ptr<Player> _player;
    
    Hand* _hand; // pointer to the hand.
        
    
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
     */
    
    bool init(const std::shared_ptr<cugl::AssetManager>& assets);
    
#pragma mark -
#pragma mark Gameplay Handling
    /**
     * The method called to update the game mode
     */
    void update(float timestep) override;
    
    /**
     * Draws all this scene to the scene's SpriteBatch.
     *
     * The default implementation of this method simply draws the scene
     * graph to the sprite batch. By overriding it, you can do custom drawing in its place.
     *
     */
    void render() override;
    
    /**
     * Rests the status of the game so we can play again.
     */
    void reset() override;
    
    
    void testScore();
    
};

#endif /* __SG_GAME_SCENE_H__ */
