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
    _tileSet = std::make_shared<TileSet>();
    
    // from the MJPlayer.
    _player = std::make_shared<Player>();
    _hand = &_player->getHand();
    _hand->init(_tileSet);
    _hand->updateTilePositions();
    
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
    
    _player->getHand().updateTilePositions();
    
    
    testScore();
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
    
    _player->getHand().draw(_batch);
    
    _batch->end(); 
}


void GameScene::testScore(){
    
    // Create some sample tile sets to simulate played hands
    std::vector<std::shared_ptr<TileSet::Tile>> set1 = {
        
        std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::FIVE, TileSet::Tile::Suit::BAMBOO),
        std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::THREE, TileSet::Tile::Suit::BAMBOO),
        std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::FOUR, TileSet::Tile::Suit::BAMBOO)
        
    }; // Valid consecutive set
    
    
    std::vector<std::shared_ptr<TileSet::Tile>> set2 = {
        std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::SEVEN, TileSet::Tile::Suit::DOT),
        std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::SEVEN, TileSet::Tile::Suit::DOT),
        std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::SEVEN, TileSet::Tile::Suit::DOT),
        std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::SEVEN, TileSet::Tile::Suit::DOT)
    }; // A valid set of a kind
    
    
    std::vector<std::shared_ptr<TileSet::Tile>> duplicateSet = {
        
        std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::SEVEN, TileSet::Tile::Suit::BAMBOO),
        std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::SIX, TileSet::Tile::Suit::BAMBOO),
        std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::EIGHT, TileSet::Tile::Suit::BAMBOO)
    }; // Duplicate of set1

    
    
    std::vector<std::vector<std::shared_ptr<TileSet::Tile>>> played_sets;
    
    played_sets.push_back(set1);
    played_sets.push_back(set2);
    played_sets.push_back(duplicateSet);
    
    
    ScoreManager scoreManager(played_sets);
    
    
    int result = scoreManager.calculateScore();
    
    CULog("the score %d", result);
    assert(result==20);
    
    
    
    
    
    
    

    
}

