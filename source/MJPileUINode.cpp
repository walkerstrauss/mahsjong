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
    _select = _assets->get<SceneNode>("pileui.pile-ui.select");
    _rearrange1 = _assets->get<SceneNode>("pileui.pile-ui.rearrange1");
    _rearrange2 = _assets->get<SceneNode>("pileui.pile-ui.rearrange2");
    
    _state = NONE;
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

