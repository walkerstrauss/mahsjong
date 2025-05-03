//
//  MJDiscardUIScene.cpp
//  Mahsjong
//
//  Created by Walker Strauss on 3/8/25.
//

#include <cugl/cugl.h>
#include <iostream>
#include <sstream>
#include "MJDiscardUIScene.h"

using namespace cugl;
using namespace cugl::graphics;
using namespace cugl::audio;
using namespace std;

#pragma mark -
#pragma mark Constructors
/**
 * Method to initialize the discard UI scene
 */
bool DiscardUIScene::init(const std::shared_ptr<cugl::AssetManager>& assets){
    if (!assets){
        return false;
    } else if (!Scene::initWithHint(0,720)){
        return false;
    }
    _assets = assets;
    _tilesetui = _assets->get<scene2::SceneNode>("tilesetui");
    _tilesetui->setContentSize(1280,720);
    cugl::Size screenSize = cugl::Application::get()->getDisplaySize();
    //cugl::Size screenSize = Size(0,SCENE_HEIGHT);
    
    screenSize *= _tilesetui->getContentSize().height/screenSize.height;

    //AudioController::getInstance().init(_assets);
    
    float offset = (screenSize.width -_tilesetui->getWidth())/2;
    _tilesetui->setPosition(offset, _tilesetui->getPosition().y);

    
    if (!Scene2::initWithHint(screenSize)) {
        std::cerr << "Scene2 initialization failed!" << std::endl;
        return false;
    }
//    _tilesetui->setContentSize(getSize());
//    _tilesetui->doLayout();
//    _tilesetui->setPosition((Application::get()->getDisplayWidth() - _tilesetui->getWidth()) / 8, _tilesetui->getPosition().y);
    _labels.resize(27);
    std::shared_ptr<scene2::SceneNode> node = _assets->get<scene2::SceneNode>("tilesetui.tilesetscene.board.number");
    
    for (int i = 0; i < 27; i++){
//        std::string s = "tilesetui.tilesetscene.board.number.label";
//        if (i != 0){
//            s += "_" + std::to_string(i);
//        }
//        std::shared_ptr<scene2::Label> label = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>(s + std::to_string(i + 1)));
        std::shared_ptr<scene2::Label> label = std::dynamic_pointer_cast<scene2::Label>(node->getChild(i));
        label->setText("0");
        _labels[i] = label;
    }
    backBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("tilesetui.tilesetscene.board.buttonClose"));
    backBtnKey = backBtn->addListener([this](const std::string& name, bool down) {
        if (!down) {
            AudioController::getInstance().playSound("Done");
            setActive(false);
        }
    });
    
    backBtn->activate();
    
    addChild(_tilesetui);
    setActive(false);
    return true;
}

void DiscardUIScene::dispose(){
//    _labels.clear();
//    _tilesetui = nullptr;
//    _assets = nullptr;
}

#pragma mark -
#pragma mark Gameplay Handling

void DiscardUIScene::reset(){
    return;
}

void DiscardUIScene::update(float timestep){
    return;
}

void DiscardUIScene::setActive(bool value){
    if (isActive() != value){
        Scene2::setActive(value);
        if (value){
            choice = Choice::NONE;
            _tilesetui->setVisible(true);
            backBtn->activate();
        } else {
            _tilesetui->setVisible(false);
            backBtn->deactivate();
        }
    }
}
/**
 * Method to get the index of this tile's associated label in the discard UI vector of labels
 *
 * @param tile  the tile whose label we need in _labels
 * @return an int representing the index of this tile's discard UI label
 */
int DiscardUIScene::getLabelIndex(std::shared_ptr<TileSet::Tile> tile){
    int rowIndex = 0;
    switch (tile->getSuit()){
        case TileSet::Tile::Suit::BAMBOO:
            break;
        case TileSet::Tile::Suit::CRAK:
            rowIndex = 9;
            break;
        case TileSet::Tile::Suit::DOT:
            rowIndex = 18;
            break;
        default:
            break;
    }
    return rowIndex + (int)tile->getRank() - 1;
}

/**
 * Method to update discard UI label corresponding to tile passed as argument
 *
 * @param tile  the tile to increment in the discard UI
 * @return true if update was successful, and false otherwise
 */
bool DiscardUIScene::incrementLabel(std::shared_ptr<TileSet::Tile> tile){
    // Get index of label in _labels
    int i = getLabelIndex(tile);
    
    // Check if we already discarded 4 (or more) of this tile
    if (std::stoi(_labels[i]->getText()) > 3){
        CULog("already discarded all copies of this tile");
        return false;
    }
    
    // Increment discard UI number and update label text
    std::string text = std::to_string(std::stoi(_labels[i]->getText()) + 1);
    _labels[i]->setText(text);
    return true;
}

/**
 * Method to update discard UI label corresponding to tile passed as argument
 *
 * @param tile  the tile to decrement in the discard UI
 * @return true if update was successful, and false otherwise
 */
bool DiscardUIScene::decrementLabel(std::shared_ptr<TileSet::Tile> tile){
    // Get index of label in _labels
    int i = getLabelIndex(tile);
    
    // Check if we already discarded 4 (or more) of this tile
    if (std::stoi(_labels[i]->getText()) < 0){
        CULog("All copies of this tile are in play");
        return false;
    }
    
    // Increment discard UI number and update label text
    std::string text = std::to_string(std::stoi(_labels[i]->getText()) - 1);
    _labels[i]->setText(text);
    return true;
}

void DiscardUIScene::render(){
    _batch->begin(_camera->getCombined());
    _tilesetui->render(_batch);
    _batch->end();
}
