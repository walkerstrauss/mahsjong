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
    
    for (auto& anim: _SelectAnims) {
        if (!anim.done){
            anim.update(dt);
        } else {
            if (anim.growing) {
                _SelectAnims.erase(std::remove_if(_SelectAnims.begin(), _SelectAnims.end(), [&anim](const SelectAnim& a) {return a.tile == anim.tile;}), _SelectAnims.end());
                addSelectAnim(anim.tile, anim.tile->pos, anim.tile->pos + Vec2(0, 5.0f), anim.tile->_scale, anim.origScale, anim.frames, false);
            } else {
                _SelectAnims.erase(std::remove_if(_SelectAnims.begin(), _SelectAnims.end(), [&anim](const SelectAnim& a) {return a.tile == anim.tile;}), _SelectAnims.end());
            }
        }
    }
}
