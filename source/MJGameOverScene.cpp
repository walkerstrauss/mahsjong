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
//    Size dimen = getSize();
    
    choice = Choice::NONE;
    type = Type::NEITHER;
    // init the win scene
    _winscene = _assets->get<scene2::SceneNode>("winscene");
    _winscene->setContentSize(getSize());
    _winscene->getChild(0)->setContentSize(_winscene->getContentSize());
    _winscene->doLayout();
    
    cugl::Size screenSize = cugl::Application::get()->getDisplaySize();
    
    screenSize *= _winscene->getContentSize().height/screenSize.height;
    
    float offset = (screenSize.width -_winscene->getWidth())/2;
    _winscene->setPosition(offset, _winscene->getPosition().y);

    AudioController::getInstance().init(_assets);
    
    // init the lose scene
    _losescene = _assets->get<scene2::SceneNode>("losescene");
    _losescene->setContentSize(getSize());
    _losescene->getChild(0)->setContentSize(_losescene->getContentSize());
    
    offset = (screenSize.width -_losescene->getWidth())/2;
    _losescene->setPosition(offset, _losescene->getPosition().y);

    if (!Scene2::initWithHint(screenSize)) {
        std::cerr << "Scene2 initialization failed!" << std::endl;
        return false;
    }
    
    // init buttons
    _mainWinBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("winscene.scorewinscene.win_board.button_main"));
    _mainLoseBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("losescene.scoredefeatscene.defeated_board.button_main"));
    
    // add button listeners
    _mainWinKey = _mainWinBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            choice = Choice::MENU;
            AudioController::getInstance().playSound("confirm");
        }
       
    });
    _mainLoseKey = _mainLoseBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            choice = Choice::MENU;
            AudioController::getInstance().playSound("confirm");
        }
    });
    
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
    std::shared_ptr<graphics::Texture> temp = Texture::getBlank();
    _batch->draw(temp, Color4(0,0,0,255), Rect(Vec2::ZERO, cugl::Application().get()->getDisplaySize()));
   
    switch (type){
        case Type::WIN:
            _winscene->render(batch);
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
