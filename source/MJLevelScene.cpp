//
//  MJLevelScene.cpp
//  Mahsjong
//
//  Created by Walker Strauss on 2/24/25.
//
#include <cugl/cugl.h>
#include <iostream>
#include <sstream>

#include "MJLevelScene.h"
#include "MJScoreManager.h"

using namespace cugl;
using namespace cugl::graphics;
using namespace cugl::audio;
using namespace std;

#pragma mark -
#pragma mark Level Layout

#define SCENE_HEIGHT 720

/**
 * This class represents a single level in the game
 */
#pragma mark -
#pragma mark Constructors

/**
 * Initializes a new level with given asset manager and player
 *
 * @param assets    the asset manager for the level
 * @param player    the player for the game
 */
bool LevelScene::init(const std::shared_ptr<AssetManager>& assets, const std::shared_ptr<Player> player){
    if (assets == nullptr){
        return false;
    } else if (player == nullptr){
        return false;
    } else if (!Scene2::initWithHint(Size(0, SCENE_HEIGHT))){
        return false;
    }
    // Set assets and player fields
    _assets = assets;
    _player = player;
    
    // Create btns
    return true;
}

/**
 * Disposes of all resources for the level
 */
void LevelScene::dispose(){
    _player = nullptr;
    _assets = nullptr;
}

#pragma mark -
#pragma mark Gameplay Handling

/**
 * Method to update the level scene
 */
void LevelScene::update(float timestep){
    _player->getHand().updateTilePositions();
}

/**
 * Method to draw the level to the screen
 */
void LevelScene::render(){
    if (!_batch){
        _batch = graphics::SpriteBatch::alloc();
    }
    _batch->begin(getCamera()->getCombined());
    _batch->end();
}
