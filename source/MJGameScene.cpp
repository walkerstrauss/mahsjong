//
//  MJGameScene.cpp
//  Mahsjong
//
//  Created by Patrick Choo on 2/18/25.
//
#include <cugl/cugl.h>
#include <iostream>
#include <sstream>

#include "MJGameScene.h"

using namespace cugl;
using namespace cugl::graphics;
using namespace cugl::audio;
using namespace std;

#pragma mark -
#pragma mark Level Layout

// Lock the screen esize to a fixed heigh regardless of aspect ratio
// PLEASE ADJUST AS SEEN FIT
#define SCENE_HEIGHT 720


#pragma mark -
#pragma mark Constructors
/**
 * Initializes the controller contents, and starts the game
 */

bool GameScene::init(const std::shared_ptr<cugl::AssetManager>& assets){
    // Initialize the scene to a locked height
    if (assets == nullptr){
        return false;
    } else if (!Scene2::initWithHint(Size(0, SCENE_HEIGHT))){
        return false;
    }
    
    // Start up the input handler
    _assets = assets;
    
    /**
     * Setting up the background image and constant values
     * TODO: Please edit scene background and constants as seen fit
     */
    
    /**
     * Setting up objects and textures
     * TODO: Please intialize objects and their textures
     */
    
    return true;
}

void GameScene::dispose() {
    if(_active) {
        removeAllChildren();
        _active = false;
    }
}

#pragma mark -
#pragma mark Gameplay Handling
/**
 * Resets the status of the game so that we can play again
 * TODO: Please dispose or reset all of the objects you implement in this method when game is over
 */
void GameScene::reset(){
    return;
}

/**
 * The method to update the game mode
 *
 * @param timestep The amount of time (in seconds) since the last frame
 *
 * TODO: Please implement any gameplay associated with updates
 */
void GameScene::update(float timestep){
    //Reading input
    _input.readInput();
}

/**
 * Draws all this to the scene's SpriteBatch.
 */

void GameScene::render(){
    /**
     * This is the temporary view of our camera. Not supposed to do this.
     * TODO: Please edit camera view appropriately
     */
    _batch->begin(getCamera()->getCombined());
    
    /**
     * This is just a temporary texture to test we are in our game
     * TODO: Please fix with appropriate background texture
     */
    const std::shared_ptr<Texture> temp = Texture::getBlank();
    _batch->draw(temp, Color4("white"), Rect(Vec2::ZERO,getSize()));
    
    _batch->end(); 
}



