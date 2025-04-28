//
//  MJMenuScene.cpp
//  Mahsjong
//
//  Created by Patrick Choo on 3/3/25.
//
#include <cugl/cugl.h>
#include <iostream>
#include <sstream>

#include "MJMenuScene.h"
#include "MJAudioController.h"

using namespace cugl;
using namespace cugl::scene2;
using namespace cugl::graphics;
using namespace cugl::audio;
using namespace std;

#pragma mark -
#pragma mark Level Layout

/** Regardless of logo, lock the height to this */
#define SCENE_HEIGHT  720


#pragma mark -
#pragma mark Constructors
/**
 * Initializes the controller contents, and starts the game
 *
 * In previous labs, this method "started" the scene.  But in this
 * case, we only use to initialize the scene user interface.  We
 * do not activate the user interface yet, as an active user
 * interface will still receive input EVEN WHEN IT IS HIDDEN.
 *
 * That is why we have the method {@link #setActive}.
 *
 * @param assets    The (loaded) assets for this game mode
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool MenuScene::init(const std::shared_ptr<cugl::AssetManager>& assets) {
    // Initialize the scene to a locked width
    if (assets == nullptr) {
        return false;
    } else if (!Scene2::initWithHint(0,SCENE_HEIGHT)){
        return false;
    }
    // Start up the input handler
    _assets = assets;
    
    _homescene = _assets->get<scene2::SceneNode>("home");
    _homescene->setContentSize(getSize());
    _homescene->getChild(0)->setContentSize(_homescene->getContentSize());
    cugl::Size screenSize = cugl::Application::get()->getDisplaySize();
    //cugl::Size screenSize = Size(0,SCENE_HEIGHT);
    
    screenSize *= _homescene->getContentSize().height/screenSize.height;
    
    float offset = (screenSize.width -_homescene->getWidth())/2;
    _homescene->setPosition(offset, _homescene->getPosition().y);
    
    if (!Scene2::initWithHint(screenSize)) {
        std::cerr << "Scene2 initialization failed!" << std::endl;
        return false;
    }
    
//    _homescene->setContentSize(getSize());
//    _homescene->doLayout();
//    _homescene->setPosition((Application::get()->getDisplayWidth() - _homescene->getContentWidth()) / 8, _homescene->getPosition().y);
    _choice = Choice::NONE;
    _hostbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("home.home.menu.button1"));
    _joinbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("home.home.menu.button2"));

//    // Program the buttons
    _hostbutton->addListener([this](const std::string& name, bool down) {
        if (down) {
            _choice = HOST;
//            AudioEngine::get()->play("confirm",_assets->get<Sound>("confirm"),false,1.0f);
            AudioController::getInstance().playSound("confirm");
        }
    });
    _joinbutton->addListener([this](const std::string& name, bool down) {
        if (down) {
            _choice = JOIN;
//            AudioEngine::get()->play("confirm",_assets->get<Sound>("confirm"),false,1.0f);
            AudioController::getInstance().playSound("confirm", false);
        }
    });
    settingsbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("home.home.button3"));
    settingsbutton->addListener([this](const std::string& name, bool down){
        if (down){
            _choice = SETTING;
//            AudioEngine::get()->play("confirm",_assets->get<Sound>("confirm"),false,1.0f);
            AudioController::getInstance().playSound("confirm", false);
        }
    });
    
    _tutorialbutton = std::dynamic_pointer_cast<Button>(_assets->get<SceneNode>("home.home.menu.button4"));
    _tutorialbutton->addListener([this](const std::string&name, bool down){
        if (down){
            AudioController::getInstance().playSound("confirm", false);
            _choice = TUTORIAL;
        }
    });
    _grandmaMainSheet = SpriteNode::allocWithSheet(_assets->get<Texture>("grandmaMain"), 2, 3, 5);
    _grandmaMainSheet->setAnchor(Vec2::ANCHOR_CENTER);
    _grandmaMainSheet->setPosition(340, 410);
    _grandmaMainSheet->setVisible(true);
    _grandmaMainSheet->setFrame(0);
    _grandmaMainSheet->setScale(0.13);
    
    _homescene->setVisible(true);
    addChild(_homescene);
    setActive(false);
    
    // Play the background music for the menu scene.
    AudioController::getInstance().init(_assets);
    AudioEngine::start();
    AudioController::getInstance().playMusic("menuMusic", true);
    
    
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void MenuScene::dispose() {
    if (_active) {
        AudioController::getInstance().stopMusic();
        removeAllChildren();
        _active = false;
        _hostbutton = nullptr;
        _joinbutton  = nullptr; 
    }
}


/**
 * Sets whether the scene is currently active
 *
 * This method should be used to toggle all the UI elements.  Buttons
 * should be activated when it is made active and deactivated when
 * it is not.
 *
 * @param value whether the scene is currently active
 */
void MenuScene::setActive(bool value) {
    if (isActive() != value) {
        Scene2::setActive(value);
        if (value) {
            _choice = NONE;
            _hostbutton->activate();
            _joinbutton->activate();
            settingsbutton->activate();
            _tutorialbutton->activate();
        } else {
            _hostbutton->deactivate();
            _joinbutton->deactivate();
            settingsbutton->deactivate();
            _tutorialbutton->deactivate();
            // If any were pressed, reset them
            settingsbutton->setDown(false);
            _hostbutton->setDown(false);
            _joinbutton->setDown(false);
            _tutorialbutton->setDown(false);
        }
    }
}

void MenuScene::render(){
    _batch->begin(_camera->getCombined());
    const std::shared_ptr<Texture> temp = Texture::getBlank();
    _batch->draw(temp, Color4(0,0,0,255), Rect(Vec2::ZERO,Application::get()->getDisplaySize()));
    _homescene->render(_batch);
    _grandmaMainSheet->render(_batch);
    
    _batch->setTexture(nullptr);
    
    Affine2 trans(_joinbutton->getParent()->getNodeToWorldTransform());
    _batch->setColor(Color4(255, 0, 0, 100));
    _batch->fill(trans.transform(_joinbutton->getBoundingBox()));
    _batch->setColor(Color4(0, 0, 255, 100));
    _batch->fill(trans.transform(_hostbutton->getBoundingBox()));
    _batch->end();
}

void MenuScene::update(float timestep){
    frameTimer += timestep;
    
    int frame = _grandmaMainSheet->getFrame();
    if (frameTimer >= frameDelay){
        frameTimer = 0.0;
        frame++;
    } else {
        return;
    }
    
    if (frame >= _grandmaMainSheet->getCount()){
        _grandmaMainSheet->setFrame(0);
    } else {
        _grandmaMainSheet->setFrame(frame);
    }
}
