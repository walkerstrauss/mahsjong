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

/**
 * Initializes the animation controller with an asset manager
 *
 * @param assets    The asset manager to get game sounds from
 * @return true if initialization was successful, and false otherwise
 */
bool AnimationController::init(const std::shared_ptr<cugl::AssetManager>& assets){
    return true;
}

#pragma mark -
#pragma mark Gameplay Handling

void AnimationController::update(float dt) {
    if (_paused) return;
    
    for (auto& anim : _spriteSheetAnimations) {
        anim.update(dt);
        
        if (anim.done){
            _spriteSheetAnimations.erase(std::remove_if(_spriteSheetAnimations.begin(), _spriteSheetAnimations.end(), [&anim](const SpriteSheetAnimation& a) {return a.node == anim.node;}), _spriteSheetAnimations.end());
        }
    }
    
    for (auto& anim: _spriteNodeAnims) {
        if(!anim.done) {
            anim.update(dt);
        }
    }
    
    for (auto& anim: _fadeAnims){
        if(anim.active){
            anim.update(dt);
        }
    }
    
    _spriteNodeAnims.erase(std::remove_if(_spriteNodeAnims.begin(), _spriteNodeAnims.end(), [](const SpriteNodeAnim& a) {return a.done;}), _spriteNodeAnims.end());
    _fadeAnims.erase(std::remove_if(_fadeAnims.begin(), _fadeAnims.end(), [](const FadeAnim& a) {return a.active;}), _fadeAnims.end());
}
