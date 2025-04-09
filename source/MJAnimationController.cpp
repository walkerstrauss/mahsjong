//
//  MJAnimationController.cpp
//  Mahsjong
//
//  Created by Walker Strauss on 3/31/25.
//
#include "MJAnimationController.h"

AnimationController* AnimationController::_instance = nullptr;

#pragma mark -
#pragma mark Constructors

static AnimationController& getInstance() {
    static AnimationController instance;
    return instance;
}

/**
 * Initializes the animation controller with an asset manager
 *
 * @param assets    The asset manager to get game sounds from
 * @return true if initialization was successful, and false otherwise
 */
bool AnimationController::init(const std::shared_ptr<cugl::AssetManager>& assets){
    _growing = false;
    _shrinking = false;
    return true;
}

/**
 * Initializes the animation controller with an asset manager
 *
 * @param assets    The asset manager to get game sounds from
 * @param scene     The scene whose sprites this controller will animate
 * @return true if initialization was successful, and false otherwise
 */
bool init(const std::shared_ptr<cugl::AssetManager>& assets, GameOverScene scene);
