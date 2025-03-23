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

#define SCENE_HEIGHT  700

#pragma mark -
#pragma mark Constructors

bool GameOverScene::init(const std::shared_ptr<cugl::AssetManager>& assets){
    if (!assets){
        return false;
    } else if (!Scene2::initWithHint(0, SCENE_HEIGHT)){
        return false;
    }
    
    _assets = assets;
    Size dimen = Application::get()->getDisplaySize();
    
    choice = Choice::NONE;
    type = Type::NEITHER;
    // init the win scene
    _winscene = _assets->get<scene2::SceneNode>("winscene");
    _winscene->setContentSize(dimen);
    // init the lose scene
    _losescene = _assets->get<scene2::SceneNode>("losescene");
    _losescene->setContentSize(dimen);
    
    // init buttons
    _mainWinBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("winscene.scorewinscene.win_board.button_main"));
    _mainLoseBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("losescene.scoredefeatscene.defeated_board.button_main"));
    
    
    // add button listeners
    _mainWinKey = _mainWinBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            choice = Choice::MENU;
        }
       
    });
    _mainLoseKey = _mainLoseBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            choice = Choice::MENU;
        }
    });
    
    _sheetNode = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>("win"),2,3,5);
    _sheetNode->setAnchor(Vec2::ANCHOR_CENTER);
    _sheetNode->setPosition(dimen.width / 2, dimen.height / 2);
    _sheetNode->setScale(0.2);
    _sheetNode->setFrame(0);
    
    addChild(_winscene);
    addChild(_losescene);
    setActive(false);
    return true;
}

void GameOverScene::dispose(){
    return;
}

#pragma mark -
#pragma mark Gameplay Handling

void GameOverScene::reset(){
    return;
}

void GameOverScene::update(float timestep){
    if (!isActive()) return;
    _frameTimer += timestep;  // Accumulate time
        if (_frameTimer >= _frameDelay) {
            _frameTimer = 0; // Reset timer
            if (_sheetNode->getFrame() > 3){
                _sheetNode->setFrame(0);
            } else {
                _sheetNode->setFrame(_sheetNode->getFrame() + 1);
            }
        }
    return;
}

void GameOverScene::setActive(bool value){
    if (isActive() != value){
        Scene2::setActive(value);
        switch (type){
            case Type::WIN:
                if (value){
                    _winscene->setVisible(true);
                    _mainWinBtn->activate();
                } else {
                    _winscene->setVisible(false);
                    _mainWinBtn->deactivate();
                }
                break;
            case Type::LOSE:
                if (value){
                    _losescene->setVisible(true);
                    _mainLoseBtn->activate();
                } else {
                    _losescene->setVisible(false);
                    _mainLoseBtn->deactivate();
                }
                break;
            case Type::NEITHER:
                // Do nothing
                if (value){
                    CULog("should not be active if no type");
                    _winscene->setVisible(true);
                    _losescene->setVisible(true);
                    _mainWinBtn->activate();
                    _mainLoseBtn->activate();
                } else {
                    _winscene->setVisible(false);
                    _losescene->setVisible(false);
                    _mainWinBtn->deactivate();
                    _mainLoseBtn->deactivate();
                }
                break;
        }
    }
}

void GameOverScene::render(const std::shared_ptr<graphics::SpriteBatch>& batch){
    batch->begin(_camera->getCombined());
    switch (type){
        case Type::WIN:
            _winscene->render(batch);
            _sheetNode->render(batch);
            break;
        case Type::LOSE:
            _losescene->render(batch);
            break;
        case Type::NEITHER:
            CULog("no render for neither");
            break;
    }
    batch->end();
}
