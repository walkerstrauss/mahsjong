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
#define SCENE_HEIGHT  720

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
    std::shared_ptr<SceneNode> node = _assets->get<SceneNode>("load.after");
    node->setContentSize(getSize());
    node->doLayout();
    //std::shared_ptr<cugl::scene2::SceneNode> loadLayer =
    //    std::dynamic_pointer_cast<cugl::scene2::SceneNode>(this->getChildByName("load"));

    //loadLayer->setContentSize(cugl::Size(1280, 720));
    
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
