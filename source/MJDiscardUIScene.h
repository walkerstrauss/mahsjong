//
//  MJDiscardUIScene.h
//  Mahsjong
//
//  Created by Walker Strauss on 3/8/25.
//

#ifndef __MJ_DISCARD_UI_SCENE_H__
#define __MJ_DISCARD_UI_SCENE_H__

#include <cugl/cugl.h>
#include "MJTileSet.h"
#include "MJPlayer.h"

using namespace cugl;
using namespace cugl::scene2;
using namespace cugl::netcode;
using namespace std;

/**
 * This is the class responsible for drawing the discard UI to the game scene
 */
class DiscardUIScene : public cugl::scene2::Scene2{
protected:
    /** Asset manager for this game mode */
    std::shared_ptr<cugl::AssetManager> _assets;
    
public:
#pragma mark -
#pragma mark Constructors
    /**
     * Method to initialize the discard UI scene
     */
    bool init(const std::shared_ptr<cugl::AssetManager> assets);
    
    /**
     * Method to dispose of all (non-static) resources allocated to this mode.
     */
    void dispose() override;
    
#pragma mark -
#pragma mark Gameplay Handling
    /**
     * Method to reset the discard UI scene
     */
    void reset() override;
    
    /**
     * The method called to update the discard UI scene
     *
     * @param timestep The amount of time (in seconds) since the last frame
     */
    void update(float timestep) override;
    
    /**
     * Draws this scene and its children to the scene's SpriteBatch.
     */
    void render() override;
}
