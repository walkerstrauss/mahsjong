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

#pragma mark -
#pragma mark Constructors

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

void LevelScene::dispose(){
    _player = nullptr;
    _assets = nullptr;
}

#pragma mark -
#pragma mark Gameplay Handling

void LevelScene::update(float timestep){
    _player->getHand().updateTilePositions();
}

void LevelScene::render(){
    if (!_batch){
        _batch = graphics::SpriteBatch::alloc();
    }
    _batch->begin(getCamera()->getCombined());
    _player->getHand().draw(_batch);
    _batch->end();
    
}

void LevelScene::discard(){
    return;
}

void LevelScene::play(){
    return;
}

