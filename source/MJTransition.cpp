//
//  MJTransition.cpp
//  Mahsjong
//
//  Created by Patrick Choo on 5/10/25.
//

#include <cugl/cugl.h>
#include <iostream>
#include <sstream>
#include "MJTransition.h"

using namespace cugl;
using namespace cugl::graphics;
using namespace cugl::audio;
using namespace cugl::scene2;
using namespace std;

#define SCENE_HEIGHT 720

bool TransitionScene::init(const std::shared_ptr<cugl::AssetManager> &assets) {
    if(assets == nullptr) {
        return false;
    } else if (!Scene2::initWithHint(Size(0,SCENE_HEIGHT))) {
        std::cerr << "Scene2 initialization failed!" << std::endl;
        return false;
    }
    
    _assets = assets;
    
    Size screenSize = cugl::Application::get()->getDisplaySize();
    
    background = Rect(Vec2::ZERO, screenSize);
    teamLogo = std::dynamic_pointer_cast<TexturedNode>(scene2::TexturedNode::alloc());
    teamLogo->setTexture(_assets->get<Texture>("team logo"));
    
    setActive(false);
    return true;
}

void TransitionScene::update(float dt) {
    if(_state == START) {
    }
}

void TransitionScene::render(std::shared_ptr<cugl::graphics::SpriteBatch>& batch) {
    _batch->begin(getCamera()->getCombined());
    
    _batch->setColor(Color4::BLACK);
    _batch->fill(background);
    
    _batch->end();
}

void TransitionScene::dispose() {
    if(!isActive()) {
        removeAllChildren();
        background = Rect();
        teamLogo = nullptr;
    }
}



