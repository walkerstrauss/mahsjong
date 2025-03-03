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

using namespace cugl;
using namespace cugl::scene2;
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
    } else if (!Scene2::initWithHint(Size(0,SCENE_HEIGHT))) {
        return false;
    }
    
    // Start up the input handler
    _assets = assets;
    
    Size dimen = getSize();
    // Acquire the scene built by the asset loader and resize it the scene
    std::shared_ptr<scene2::SceneNode> scene = _assets->get<scene2::SceneNode>("menu");
    scene->setContentSize(dimen);
    scene->doLayout(); // Repositions the HUD
    _choice = Choice::NONE;
    _hostbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("menu.host"));
    _joinbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("menu.join"));
    
    // Program the buttons
    _hostbutton->addListener([this](const std::string& name, bool down) {
        if (down) {
            _choice = Choice::HOST;
        }
    });
    _joinbutton->addListener([this](const std::string& name, bool down) {
        if (down) {
            _choice = Choice::JOIN;
        }
    });

    addChild(scene);
    setActive(false);
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void MenuScene::dispose() {
    if (_active) {
        removeAllChildren();
        _active = false;
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
        } else {
            _hostbutton->deactivate();
            _joinbutton->deactivate();
            // If any were pressed, reset them
            _hostbutton->setDown(false);
            _joinbutton->setDown(false);
        }
    }
}
