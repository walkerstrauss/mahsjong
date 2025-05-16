//
//  MJHelpScene.cpp
//  Mahsjong
//
//  Created by Walker Strauss on 5/5/25.
//

#include <cugl/cugl.h>
#include <iostream>
#include <sstream>

#include "MJHelpScene.h"

bool HelpScene::init(const std::shared_ptr<AssetManager>& assets, std::shared_ptr<InputController> inputController) {
    if (assets == nullptr) {
        return false;
    } else if (!Scene2::initWithHint(0,720)) {
        return false;
    }
    _assets = assets;
    _tutorialScene = _assets->get<cugl::scene2::SceneNode>("tutorial");

    AudioController::getInstance().init(_assets);

    Size dimen = getSize();
    _tutorialScene->setContentSize(dimen);
    _tutorialScene->getChild(0)->setContentSize(dimen);
    _tutorialScene->doLayout();
    _exit = std::dynamic_pointer_cast<scene2::Button>(_assets->get<cugl::scene2::SceneNode>("tutorial.tutorialScene.exit"));
    _back = std::dynamic_pointer_cast<scene2::Button>(_assets->get<cugl::scene2::SceneNode>("tutorial.tutorialScene.back"));
    _back->addListener([this](const std::string& name, bool down) {
        if (down) {
            if (_slide > 0) {
                _slide -= 1;
                _presentation->setTexture(_assets->get<cugl::graphics::Texture>(_slides[_slide]));
                _presentation->setContentSize(_tutorialScene->getContentSize());
                _presentation->setPosition(_tutorialScene->getWidth() / 2, _tutorialScene->getHeight() / 2);
            } else {
                _choice = HelpScene::BACK;
            }
        }
    });
    _exit->addListener([this](const std::string& name, bool down) {
        if (down) {
            _choice = HelpScene::BACK;
        }
    });
    _presentation = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<cugl::scene2::SceneNode>("tutorial.tutorialScene.tu1"));
    _slides = {"tu1", "tu2", "tu3", "tu4", "tu5", "tu6", "tu7", "tu8", "tu9", "tu10", "tu11"};
    _slide = 0;
    _presentation->setTexture(_assets->get<cugl::graphics::Texture>(_slides[_slide]));
    _presentation->setContentSize(_tutorialScene->getContentSize());
    _presentation->setPosition(_tutorialScene->getWidth() / 2, _tutorialScene->getHeight() / 2);
    
    _input = inputController;
    _choice = HelpScene::NONE;
    addChild(_tutorialScene);

    return true;
}

void HelpScene::update(float timestep) {
//    _input.readInput();
//    _input.update();

    if (_input->didPress() && !_back->isDown() && _slide < 11 && !_exit->isDown()) {
        if (_slide == 10){
            _slide = 0;
            _choice = BACK;
            return;
        }
        _slide += 1;
        _presentation->setTexture(_assets->get<cugl::graphics::Texture>(_slides[_slide]));
        _presentation->setContentSize(_tutorialScene->getContentSize());
        _presentation->setPosition(_tutorialScene->getWidth()/2, _tutorialScene->getHeight()/2);

        AudioController::getInstance().playSound("Confirm");
    }
}

void HelpScene::setActive(bool value) {
    if (isActive() != value) {
        Scene2::setActive(value);
        if (value) {
            _exit->activate();
            _back->activate();
        }
        else {
            _exit->deactivate();
            _back->deactivate();
            _choice = NONE;
        }
    }
}

void HelpScene::dispose() {
    if (_active) {
        removeAllChildren();
//        _input.dispose();

        _active = false;
    }
}
