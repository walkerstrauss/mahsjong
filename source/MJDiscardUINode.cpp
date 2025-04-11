//
//  MJDiscardUINode.cpp
//  Mahsjong
//
//  Created by Walker Strauss on 4/11/25.
//

#include "MJDiscardUINode.h"

using namespace cugl;
using namespace cugl::scene2;
using namespace std;

#pragma mark -
#pragma mark Initialization

bool DiscardUINode::init(const std::shared_ptr<cugl::AssetManager>& assets) {
    if (!assets) {
        return false;
    }

    _assets = assets;

    std::shared_ptr<SceneNode> root = _assets->get<SceneNode>("tilesetui");
    
    setContentSize(1280, 720);
    cugl::Size screenSize = cugl::Application::get()->getDisplaySize();
    screenSize *= getContentSize().height/screenSize.height;
    
    float offset = (screenSize.width -getWidth())/2;
    setPosition(offset, getPosition().y);


    _labels.resize(27);
    std::shared_ptr<SceneNode> labelParent = root->getChildByName("tilesetscene")->getChildByName("board")->getChildByName("number");

    for (int i = 0; i < 27; ++i) {
        std::shared_ptr<Label> label = std::dynamic_pointer_cast<Label>(labelParent->getChild(i));
        if (label) {
            label->setText("0");
            _labels[i] = label;
        }
    }

    setVisible(false);
    return true;
}

void DiscardUINode::dispose() {
    _labels.clear();
    _assets = nullptr;
    backBtn = nullptr;
}

#pragma mark -
#pragma mark UI Handling

void DiscardUINode::reset() {
    for (auto& label : _labels) {
        if (label) {
            label->setText("0");
        }
    }
}

void DiscardUINode::update(float timestep) {
}

int DiscardUINode::getLabelIndex(std::shared_ptr<TileSet::Tile> tile) {
    int rowIndex = 0;
    switch (tile->getSuit()) {
        case TileSet::Tile::Suit::CRAK:
            rowIndex = 9;
            break;
        case TileSet::Tile::Suit::DOT:
            rowIndex = 18;
            break;
        case TileSet::Tile::Suit::BAMBOO:
        default:
            rowIndex = 0;
            break;
    }
    return rowIndex + (int)tile->getRank() - 1;
}

bool DiscardUINode::incrementLabel(std::shared_ptr<TileSet::Tile> tile) {
    int i = getLabelIndex(tile);
    int count = std::stoi(_labels[i]->getText());

    if (count > 3) {
        CULog("already discarded all copies of this tile");
        return false;
    }

    _labels[i]->setText(std::to_string(count + 1));
    return true;
}

bool DiscardUINode::decrementLabel(std::shared_ptr<TileSet::Tile> tile) {
    int i = getLabelIndex(tile);
    int count = std::stoi(_labels[i]->getText());

    if (count <= 0) {
        CULog("All copies of this tile are in play");
        return false;
    }

    _labels[i]->setText(std::to_string(count - 1));
    return true;
}
