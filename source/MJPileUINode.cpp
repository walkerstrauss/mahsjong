//
//  MJPileUINode.cpp
//  Mahsjong
//
//  Created by Tina Lu on 4/16/25.
//

#include "MJPileUINode.h"

using namespace cugl;
using namespace cugl::scene2;
using namespace std;

#pragma mark -
#pragma mark Initialization

bool PileUINode::init(const std::shared_ptr<cugl::AssetManager>& assets) {
    if (!assets) {
        return false;
    }
    
    _assets = assets;
    _root = _assets->get<SceneNode>("pileui");
    
    _root->setContentSize(Size(1280, 720));

    cugl::Size screenSize = cugl::Application::get()->getDisplaySize();
    screenSize *= getContentSize().height/screenSize.height;
    
    float offset = (screenSize.width -getWidth())/2;
    setPosition(offset, getPosition().y);
    
    
    _state = OFF;
    _root->setVisible(false);
    return true;
}

void PileUINode::dispose() {
    _root = nullptr;
    _assets = nullptr;
}

void PileUINode::reset() {
    return;
}

/** Sets this scene node as active */
void PileUINode::setPileUIActive(bool active) {
    if(active) {
        _root->setVisible(true);
        _state = ON;
    }
    else {
        _root->setVisible(false);
        _state = OFF;
    }
}
