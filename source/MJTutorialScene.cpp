//
//  MJTutorialScene.cpp
//  Mahsjong
//
//  Created by Walker Strauss on 4/22/25.
//

#include <cugl/cugl.h>
#include <iostream>
#include <sstream>
#include "MJTutorialScene.h"

#pragma mark -
#pragma mark Level Layout

// Lock the screen size to a fixed height regardless of aspect ratio
#define SCENE_HEIGHT 720

bool TutorialScene::init(const std::shared_ptr<AssetManager>& assets, std::shared_ptr<InputController>& input){
    if (assets == nullptr){
        return false;
    } else if (input == nullptr){
        return false;
    } else if (!Scene2::initWithHint(0, SCENE_HEIGHT)){
        return false;
    }
    
    _assets = assets;
    _input = input;
    _choice = Choice::NONE;
    
    _tutorialScene = assets->get<SceneNode>("matchscene");
    _tutorialScene->setContentSize(getSize());
    _tutorialScene->getChild(0)->setContentSize(_tutorialScene->getContentSize());
    _tutorialScene->doLayout();
    
    _discardUINode = std::make_shared<DiscardUINode>();
    _discardUINode->init(_assets);
    _discardUINode->setContentSize(getSize());
    _discardUINode->_root->getChild(0)->setContentSize(_tutorialScene->getContentSize());
    _discardUINode->doLayout();
    
    _playArea = _assets->get<SceneNode>("matchscene.gameplayscene.play-area");
    _playArea->setVisible(false);
    
    _tradeArea = _assets->get<SceneNode>("matchscene.gameplayscene.drag-to-trade");
    _tradeArea->setVisible(false);

    std::shared_ptr<scene2::SceneNode> activeRegionNode = _assets->get<scene2::SceneNode>("matchscene.gameplayscene.drag-to-discard-tile");
    std::shared_ptr<scene2::SceneNode> discardedTileRegionNode = _assets->get<scene2::SceneNode>("matchscene.gameplayscene.discarded-tile.discarded-rec");
    std::shared_ptr<scene2::SceneNode> playerHandRegionNode = _assets->get<scene2::SceneNode>("matchscene.gameplayscene.playerhand");
    std::shared_ptr<scene2::SceneNode> pileRegionNode = _assets->get<scene2::SceneNode>("matchscene.gameplayscene.pile");
    
    cugl::Vec2 activeRegionWorldOrigin = activeRegionNode->nodeToWorldCoords(Vec2::ZERO);
    cugl::Vec2 discardedTileRegionWorldOrigin = discardedTileRegionNode->nodeToWorldCoords(Vec2::ZERO);
    cugl::Vec2 playerHandRegionWorldOrigin = playerHandRegionNode->nodeToWorldCoords(Vec2::ZERO);
    cugl::Vec2 pileRegionNodeOrigin = pileRegionNode->nodeToWorldCoords(Vec2::ZERO);
    
    _activeRegion = cugl::Rect(activeRegionWorldOrigin, activeRegionNode->getContentSize());
    _discardedTileRegion = cugl::Rect(discardedTileRegionWorldOrigin, discardedTileRegionNode->getContentSize());
    _playerHandRegion = cugl::Rect(playerHandRegionWorldOrigin.x, playerHandRegionWorldOrigin.y - 300, playerHandRegionNode->getContentSize().width, playerHandRegionNode->getContentSize().height);
    _pileBox = cugl::Rect(pileRegionNodeOrigin, pileRegionNode->getContentSize());
    
    // Initializing tileset UI button
    _tilesetUIBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("matchscene.gameplayscene.discarded-tile.discard-can"));
    _tilesetUIBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            setActive(false);
//            setTutorialActive(false);
            _backBtn->activate();
            _discardUINode->_root->setVisible(true);
            AnimationController::getInstance().pause();
            AudioController::getInstance().playSound("Select");
        }
    });
    
    
    // Initializing back button
    _backBtn = std::dynamic_pointer_cast<scene2::Button>(
        _discardUINode->_root->getChildByName("tilesetscene")->getChildByName("board")->getChildByName("buttonClose"));
    _backBtn->addListener([this](const std::string& name, bool down) {
        if (!down) {
            setActive(true);
//            setTutorialActive(true);
            _discardUINode->_root->setVisible(false);
            AudioController::getInstance().playSound("Done");
        }
    });
    
    _settingBtn = std::dynamic_pointer_cast<Button>(_assets->get<SceneNode>("matchscene.gameplayscene.setting-icon"));
    _settingBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            _choice = SETTING;
            AudioController::getInstance().playSound("Select");
        }
    });
    
    // Initializing settings button
    _infoBtn = std::dynamic_pointer_cast<Button>(_assets->get<SceneNode>("matchscene.gameplayscene.information-icon"));
    _infoBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            _choice = INFO;
            AudioController::getInstance().playSound("Select",false);
        }
    });
    
    //     Initializing opponent hand
    _opponentHandRec = _assets->get<SceneNode>("matchscene.gameplayscene.opponent-hand-rec");
    _opponentHandBtn = std::dynamic_pointer_cast<Button>(_assets->get<SceneNode>("matchscene.gameplayscene.opponent-hand"));
    _opponentHandBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            AudioController::getInstance().playSound("Select");
            _opponentHandRec->setVisible(!_opponentHandRec->isVisible());
            for (int i = 0; i < _opponentHandTiles.size(); i++){
                _opponentHandTiles[i]->setVisible(!_opponentHandTiles[i]->isVisible());
            }
        }
    });
    for (int i = 0; i < 14; i++){
        std::shared_ptr<TexturedNode> tile = std::dynamic_pointer_cast<TexturedNode>(_assets->get<SceneNode>("matchscene.gameplayscene.opponent-hand-tile.tile-back_" + std::to_string(i)));
        if (tile != nullptr){
            _opponentHandTiles.push_back(tile);
        }
    }
    _opponentHandBtn->activate();
    
    _discardedTileImage = std::dynamic_pointer_cast<scene2::TexturedNode>(_assets->get<scene2::SceneNode>("matchscene.gameplayscene.discarded-tile.discarded-tile-recent.up.discarded-tile-recent"));
    
    _dragToDiscardNode = std::dynamic_pointer_cast<cugl::scene2::TexturedNode>(
        _assets->get<cugl::scene2::SceneNode>(
            "matchscene.gameplayscene.drag-to-discard-tile"
        )
    );
    _dragToDiscardNode->setVisible(false);
    
    _dragToHandNode = std::dynamic_pointer_cast<cugl::scene2::TexturedNode>(
        _assets->get<cugl::scene2::SceneNode>(
            "matchscene.gameplayscene.drag-to-hand-area"
        )
    );
    _dragToHandNode->setVisible(false);
    addChild(_tutorialScene);
    addChild(_discardUINode->_root);
    
    _player1 = std::make_shared<Player>();
    _player2 = std::make_shared<Player>();
    _tileSet = std::make_shared<TileSet>();
    _pile = std::make_shared<Pile>();
    _discardPile = std::make_shared<DiscardPile>();
    _discardPile->init(_assets);
    
    
    return true;
}
