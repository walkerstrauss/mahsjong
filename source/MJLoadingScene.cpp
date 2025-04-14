//
//  MJLoadingScene.cpp
//  Mahsjong
//
//  Created by Mariia Tiulchenko on 13.04.2025.
//

#include <cugl/cugl.h>
#include <iostream>
#include <sstream>

#include "MJLoadingScene.h"


#pragma mark -
#pragma mark Level Layout

/** Regardless of logo, lock the height to this */
#define SCENE_HEIGHT  700

#pragma mark -
#pragma mark Constructors


//bool OurLoadingScene::init(const std::shared_ptr<cugl::AssetManager>& assets, const std::string& directory){
//    if (assets == nullptr) {
//        return false;
//    } else if (!Scene2::initWithHint(0,720)){
//        return false;
//    }
//
//    if (!cugl::scene2::LoadingScene::init(assets, directory)) {
//        return false;
//    }
//
//    _assets = assets;
//
//    _landingScene = _assets->get<scene2::SceneNode>("landingscene");
//
//    // Do an initial resize.
//    resizeScene();
//
//    _landingScene->doLayout();
//
//    return true;
//}



void OurLoadingScene::resizeScene() {
    
    //std::shared_ptr<cugl::scene2::SceneNode> loadLayer =
    //    std::dynamic_pointer_cast<cugl::scene2::SceneNode>(this->getChildByName("load"));

    //loadLayer->setContentSize(cugl::Size(1280, 720));
    
    if (_before) {
        cugl::Vec2 pos = _before->getPosition();
        CULog("before: x%f, y%f",pos.x, pos.y);
        //_before->setPosition(cugl::Vec2(offset, pos.y));
    }
    if (_after) {
        _after->setContentSize(cugl::Size(1280, 720));

        _after->setAnchor(cugl::Vec2(0, 0));
        cugl::Size screenSize = cugl::Application::get()->getDisplaySize();
        screenSize *= (_after->getContentSize().height / screenSize.height);
        float offset = (screenSize.width - _after->getWidth()) / 2;
        
       cugl::Vec2 pos = _after->getPosition();
        _after->setPosition(offset, _after->getPosition().y);
        CULog("after: x%f, y%f",pos.x, pos.y);
        //_after->setPosition(cugl::Vec2(offset, pos.y));
    }
    
    if (_bar) {
        cugl::Vec2 pos = _bar->getPosition();
        CULog("bar: x%f, y%f",pos.x, pos.y);
    }
    
    //auto button = _landingScene->getChildByName("load.play");
    
    if (_button) {
        cugl::Vec2 pos = _button->getPosition();
        CULog("button: x%f, y%f",pos.x, pos.y);
        //_button->setPosition(cugl::Vec2(pos.x + offset, pos.y));
    }
    

    
    
    /**
    
    // Set the content size on the root loading node.
    if (_landingScene) {
        _landingScene->setContentSize(1280,720);
        cugl::Size screenSize = cugl::Application::get()->getDisplaySize();
        screenSize *= _landingScene->getContentSize().height/screenSize.height;
        float offset = (screenSize.width -_landingScene->getWidth())/2.0f;
        
        _landingScene->setPosition(offset, _landingScene->getPosition().y);
        
    }
     
     */
}

void OurLoadingScene::renderAfter(){
    
    
    
    if (_batch == nullptr) {
        return;
    } else if (_batch->isDrawing()) {
        _batch->end();
    }

    _batch->begin(_camera->getCombined());
    _batch->setSrcBlendFunc(_srcFactor);
    _batch->setDstBlendFunc(_dstFactor);
    _batch->setBlendEquation(_blendEquation);
    
    _batch->draw(cugl::graphics::Texture::getBlank(), Color4(0,0,0,255), Rect(Vec2::ZERO,Application::get()->getDisplaySize()));

    for(auto it = _children.begin(); it != _children.end(); ++it) {
        (*it)->render(_batch, Affine2::IDENTITY, _color);
    }
    
    _batch->end();
    
}
