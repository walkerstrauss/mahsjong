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

    //AudioController::getInstance().init(_assets);
    
    
    // init the tie scene
    _tiescene = _assets->get<scene2::SceneNode>("tiescene");
    _tiescene->setContentSize(getSize());
    _tiescene->getChild(0)->setContentSize(_tiescene->getContentSize());
    _tiescene->doLayout();
    offset = (screenSize.width -_tiescene->getWidth())/2;
    _tiescene->setPosition(offset, _winscene->getPosition().y);
    
    
    _grandmaTie = SpriteNode::allocWithSheet(_assets->get<Texture>("grandmaMain"), 2, 3, 5);
    _grandmaTie->setAnchor(Vec2::ANCHOR_CENTER);
    _grandmaTie->setPosition(340, 410);
    _grandmaTie->setVisible(true);
    _grandmaTie->setFrame(0);
    _grandmaTie->setScale(1.9);
    _tiescene->addChild(_grandmaTie);
    AnimationController::getInstance().addSpriteSheetAnimation(_grandmaTie, 0, 4, true, 1, 5.0f, true);
    
    _rabbitTie = SpriteNode::allocWithSheet(_assets->get<Texture>("background_rabbit"), 3, 2, 5);
    _rabbitTie->setAnchor(Vec2::ANCHOR_CENTER);
    _rabbitTie->setPosition(580, 150);
    _rabbitTie->setVisible(true);
    _rabbitTie->setFrame(1);
    _rabbitTie->setScale(2.8);
    _tiescene->addChild(_rabbitTie);
    AnimationController::getInstance().addSpriteSheetAnimation(_rabbitTie, 1, 4, true, 1, 5.0f, true);
    
    
    _ratTie = SpriteNode::allocWithSheet(_assets->get<Texture>("background_rat"), 2, 2, 4);
    _ratTie ->setAnchor(Vec2::ANCHOR_CENTER);
    _ratTie ->setPosition(650, 195);
    _ratTie ->setVisible(true);
    _ratTie ->setFrame(0);
    _ratTie->setScale(2.8);
    _tiescene->addChild(_ratTie);
    AnimationController::getInstance().addSpriteSheetAnimation(_ratTie, 0, 3, true, 1, 5.0f, true);
    
    
    // init the lose scene
    _losescene = _assets->get<scene2::SceneNode>("losescene");
    _losescene->setContentSize(getSize());
    _losescene->getChild(0)->setContentSize(_losescene->getContentSize());
    offset = (screenSize.width -_losescene->getWidth())/2;
    _losescene->setPosition(offset, _losescene->getPosition().y);
    
    
    // Grandma animation

    _grandmaWin = SpriteNode::allocWithSheet(_assets->get<Texture>("grandmaWin"), 2, 3, 6);
    auto winBoard = _assets->get<scene2::SceneNode>("winscene.scorewinscene.win_board");
    
    _grandmaWin->setAnchor(Vec2::ANCHOR_CENTER);
    _grandmaWin->setPosition(340, 410);
    _grandmaWin->setVisible(true);
    _grandmaWin->setScale(1.4);
    _winscene->addChild(_grandmaWin);
    
    
    AnimationController::getInstance().addSpriteSheetAnimation(_grandmaWin, 0, 5, true, 1, 5.0f, true);
    
    _grandmaCry = SpriteNode::allocWithSheet(_assets->get<Texture>("grandmaCry"), 1, 3, 3);
    auto loseBoard = _assets->get<scene2::SceneNode>("losescene.scoredefeatscene.defeated_board");

    _grandmaCry->setAnchor(Vec2::ANCHOR_CENTER);
    _grandmaCry->setPosition(340, 410);
    _grandmaCry->setVisible(true);
    _grandmaCry->setScale(1.4);
    _losescene->addChild(_grandmaCry);
    
    AnimationController::getInstance().addSpriteSheetAnimation(_grandmaCry, 0, 2, true, 1, 5.0f, true);
    
    _rabbit = SpriteNode::allocWithSheet(_assets->get<Texture>("background_rabbit"), 3, 2, 5);
    _rabbit->setAnchor(Vec2::ANCHOR_CENTER);
    _rabbit->setPosition(580, 150);
    _rabbit->setVisible(true);
    _rabbit->setFrame(1);
    _rabbit->setScale(2.8);
    _winscene->addChild(_rabbit);
    
    AnimationController::getInstance().addSpriteSheetAnimation(_rabbit, 1, 4, true, 1, 5.0f, true);
    
    _rabbitLose = SpriteNode::allocWithSheet(_assets->get<Texture>("background_rabbit"), 3, 2, 5);
    _rabbitLose->setAnchor(Vec2::ANCHOR_CENTER);
    _rabbitLose->setPosition(580, 150);
    _rabbitLose->setVisible(true);
    _rabbitLose->setFrame(1);
    _rabbitLose->setScale(2.8);
    _losescene->addChild(_rabbitLose);
    
    AnimationController::getInstance().addSpriteSheetAnimation(_rabbitLose, 1, 4, true, 1, 5.0f, true);
        
    
    _rat = SpriteNode::allocWithSheet(_assets->get<Texture>("background_rat"), 2, 2, 4);
    _rat ->setAnchor(Vec2::ANCHOR_CENTER);
    _rat ->setPosition(650, 195);
    _rat ->setVisible(true);
    _rat ->setFrame(0);
    _rat->setScale(2.8);
    _winscene->addChild(_rat);

    AnimationController::getInstance().addSpriteSheetAnimation(_rat, 0, 3, true, 1, 5.0f, true);
    
    _ratLose = SpriteNode::allocWithSheet(_assets->get<Texture>("background_rat"), 2, 2, 4);
    _ratLose ->setAnchor(Vec2::ANCHOR_CENTER);
    _ratLose ->setPosition(650, 195);
    _ratLose ->setVisible(true);
    _ratLose ->setFrame(0);
    _ratLose->setScale(2.8);
    _losescene->addChild(_ratLose);

    AnimationController::getInstance().addSpriteSheetAnimation(_ratLose, 0, 3, true, 1, 5.0f, true);
    

    if (!Scene2::initWithHint(screenSize)) {
        std::cerr << "Scene2 initialization failed!" << std::endl;
        return false;
    }
    
    // init buttons
    _mainWinBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("winscene.scorewinscene.win_board.button_main"));
    _mainLoseBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("losescene.scoredefeatscene.defeated_board.button_main"));
    _mainTieBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("tiescene.scoretiescene.win_board.button_main"));
    
    // add button listeners
    _mainWinKey = _mainWinBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            choice = Choice::MENU;
            AudioController::getInstance().playSound("Confirm");
        }
       
    });
    _mainLoseKey = _mainLoseBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            choice = Choice::MENU;
            AudioController::getInstance().playSound("Confirm");
        }
    });
    _mainTieKey = _mainTieBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            choice = Choice::MENU;
            AudioController::getInstance().playSound("Confirm");
        }
    });
    
    addChild(_tiescene);
    addChild(_winscene);
    addChild(_losescene);
    setActive(false);
    
    // Winning tiles for the display.
    auto handNode  = _assets->get<scene2::SceneNode>("winscene.scorewinscene.win_board.hand");

    _winSlots.clear();
    _winSlots.reserve(14);
    for (int i = 1; i <= 14; ++i) {
        auto tileBtn = handNode->getChildByName("tile" + std::to_string(i));
        
        if (!tileBtn) {
            _winSlots.push_back(nullptr);
            continue;
        }
        auto upNode = tileBtn->getChildByName("up");
        if (!upNode) {
            _winSlots.push_back(nullptr);
            continue;
        }
        auto sample = std::dynamic_pointer_cast<cugl::scene2::TexturedNode>(
                          upNode->getChildByName("tilesample"));
        _winSlots.push_back(sample);
    }
    
    // Winning tiles for the display at the loosing side.
    auto handNodelose  = _assets->get<scene2::SceneNode>("losescene.scoredefeatscene.defeated_board.hand");
    auto handNodetie = _assets->get<scene2::SceneNode>("tiescene.scoretiescene.win_board.hand");

    _loseSlots.clear();
    _loseSlots.reserve(14);
    _tieSlots.clear();
    _tieSlots.reserve(14);
    
    for (int i = 1; i <= 14; ++i) {
        auto tileBtn = handNodelose->getChildByName("tile" + std::to_string(i));
        auto tileBtnTie = handNodetie->getChildByName("tile" + std::to_string(i));
        
        if (!tileBtn) {
            _loseSlots.push_back(nullptr);
            continue;
        }
        if (!tileBtnTie) {
            _tieSlots.push_back(nullptr);
            continue;
        }
        
        auto upNode = tileBtn->getChildByName("up");
        auto upNodeTie = tileBtnTie->getChildByName("up");
        
        if (!upNodeTie) {
            _tieSlots.push_back(nullptr);
            continue;
        }
        if (!upNode) {
            _loseSlots.push_back(nullptr);
            continue;
        }
        
        auto sample = std::dynamic_pointer_cast<cugl::scene2::TexturedNode>(
                          upNode->getChildByName("tilesample"));
        auto sampleTie = std::dynamic_pointer_cast<cugl::scene2::TexturedNode>(
                          upNodeTie->getChildByName("tilesample"));
        
        _loseSlots.push_back(sample);
        _tieSlots.push_back(sampleTie);
    }
    
    
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
    
    AnimationController::getInstance().update(timestep);

}

void GameOverScene::setActive(bool value){
    if (isActive() != value){
        //AudioController::getInstance().playMusic("menuMusic", true);
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
            case Type::TIE:
                if (value){
                    _tiescene->setVisible(true);
                    _mainTieBtn->activate();
                } else {
                    _tiescene->setVisible(false);
                    _mainTieBtn->deactivate();
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
//                    CULog("should not be active if no type");
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
        case Type::TIE:
            _tiescene->render(batch);
            break;
        case Type::NEITHER:
//            CULog("no render for neither");
            break;
    }
    batch->end();
}

void GameOverScene::setWinningHand(const std::vector<std::shared_ptr<TileSet::Tile>>& hand) {
    
    size_t count = std::min(hand.size(), _winSlots.size());
    
    for (size_t i = 0; i < count; ++i) {
        auto slot = _winSlots[i];
        auto slotTie = _tieSlots[i];
        
        if (!slot) continue;
        if (!slotTie) continue;
        auto tex = _assets->get<cugl::graphics::Texture>(hand[i]->toString());
        auto texTie = _assets->get<cugl::graphics::Texture>(hand[i]->toString());
          
        if(hand[i]->debuffed && !(hand[i]->getSuit() == TileSet::Tile::Suit::CELESTIAL)){
            tex = _assets->get<cugl::graphics::Texture>("blank normal hand");
            texTie = _assets->get<cugl::graphics::Texture>("blank normal hand");
            slot->setContentSize(40, 53);
            slotTie->setContentSize(40, 53);
            //slot->setScale(4.0f);
            //slotTie->setScale(4.0f);
        }else if(hand[i]->debuffed && (hand[i]->getSuit() == TileSet::Tile::Suit::CELESTIAL)){
            tex = _assets->get<cugl::graphics::Texture>("blank celestial hand");
            texTie = _assets->get<cugl::graphics::Texture>("blank celestial hand");
            slot->setContentSize(40, 53);
            slotTie->setContentSize(40, 53);
            
            //slotTie->setScale(4.0f);
            //slot->setScale(4.0f);

        }else{
            
            slot->setContentSize(40, 53);
            slotTie->setContentSize(40, 53);

        }
        
        slot->setTexture(tex);
        slotTie->setTexture(texTie);

        slot->setVisible(true);
        slotTie->setVisible(true);
    
    }
    
    _winscene->doLayout();
    _tiescene->doLayout();
}


void GameOverScene::setLosingHand(const std::vector<std::shared_ptr<TileSet::Tile>>& hand) {
    
    size_t count = std::min(hand.size(), _loseSlots.size());
    for (size_t i = 0; i < count; ++i) {
        auto slot = _loseSlots[i];
        
        if (!slot) continue;
        auto tex = _assets->get<cugl::graphics::Texture>(hand[i]->toString());
        
        if(hand[i]->debuffed && !(hand[i]->getSuit() == TileSet::Tile::Suit::CELESTIAL)){
            tex = _assets->get<cugl::graphics::Texture>("blank normal hand");
            slot->setContentSize(40, 53);
            //slot->setScale(4.0f);
            
        }else if(hand[i]->debuffed && (hand[i]->getSuit() == TileSet::Tile::Suit::CELESTIAL)){
            tex = _assets->get<cugl::graphics::Texture>("blank celestial hand");
            slot->setContentSize(40, 53);
            //slot->setScale(4.0f);
          
        }else{
            slot->setContentSize(40, 53);

        }
        slot->setContentSize(40, 53);
        slot->setTexture(tex);
        slot->setVisible(true);

    }
    
    _losescene->doLayout();
}
