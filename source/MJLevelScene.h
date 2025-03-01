//
//  MJLevelScene.h
//  Mahsjong
//
//  Created by Walker Strauss on 2/24/25.
//

#ifndef __MJ_LEVEL_SCENE_H
#define __MJ_LEVEL_SCENE_H
#include "MJPlayer.h"
#include <cugl/cugl.h>

/**
 * This class represents a single level in the game
 */
class LevelScene : public cugl::scene2::Scene2 {
protected:
    /** Asset manager for this game mode */
    std::shared_ptr<cugl::AssetManager> _assets;
    /** Sprite batch to draw level to */
    std::shared_ptr<cugl::graphics::SpriteBatch> _batch;
    /** Reference to the player */
    std::shared_ptr<Player> _player;
    
public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new level with default values
     */
    LevelScene() : cugl::scene2::Scene2() {}
    
    /**
     * Disposes of all resources for the level
     */
    void dispose() override;
    
    /**
     * Initializes a new level with given asset manager and player
     *
     * @param assets    the asset manager for the level
     * @param player    the player for the game
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, const std::shared_ptr<Player> player);

#pragma mark -
#pragma mark Gameplay Handling
    /**
     * Method to update the level scene
     */
    void update(float timestep) override;
    
    /**
     * Method to draw the level to the screen
     */
    void render() override;
};

#endif /* __SG_LEVEL_SCENE_H__ */
