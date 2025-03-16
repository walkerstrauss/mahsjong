//
//  MJGameOverScene.cpp
//  Mahsjong
//
//  Created by Walker Strauss on 3/15/25.
//

#include <cugl/cugl.h>
#include <iostream>
#include <sstream>
#include "MJGameOverScene.h"

using namespace cugl;
using namespace cugl::graphics;
using namespace cugl::audio;
using namespace std;

#define SCENE_HEIGHT  720

#pragma mark -
#pragma mark Constructors

bool GameOverScene::init(const std::shared_ptr<cugl::AssetManager>& assets){
    if (!assets){
        return false;
    } else if (!Scene2::initWithHint(0, SCENE_HEIGHT)){
        return false;
    }
    
    _assets = assets;
    
    // init the win scene
    _winscene = _assets->get<scene2::SceneNode>("winscene");
    // init the lose scene
    _losescene = _assets->get<scene2::SceneNode>("losescene");
    
    // init buttons
    _mainWinBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("winscene.scorewinscene.menu.button_main"));
    _quitWinBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("winscene.scorewinscene.menu.button_quit"));
    _mainLoseBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("losescene.scoredefeatscene.menu.button_main"));
    _quitLoseBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("losescene.scoredefeatscene.menu.button_quit"));
    choice = Choice::NONE;
    type = Type::NEITHER;
    return true;
}

void GameOverScene::dispose(){
    return;
}

void GameOverScene::reset(){
    return;
}

void GameOverScene::update(float timestep){
    return;
}

void GameOverScene::setActive(bool value){
    if (isActive() != value){
        Scene2::setActive(value);
        if (value){
            
        } else {
            
        }
    }
}

#pragma mark -
#pragma mark Gameplay Handling
