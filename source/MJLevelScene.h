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
    
    void dispose() override;
    
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, const std::shared_ptr<Player> player);

#pragma mark -
#pragma mark Gameplay Handling
    /**
     * Method to update the level scene
     */
    void update(float timestep) override;
    
    void render() override;
    
    void discard();
    
    void play();
};

#endif /* __SG_LEVEL_SCENE_H__ */
