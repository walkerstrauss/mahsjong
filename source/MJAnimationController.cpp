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
    
    for (auto& anim: _TileAnims) {
        if (!anim.done){
            anim.update(dt);
        } else {
            if (anim.growing) {
                _TileAnims.erase(std::remove_if(_TileAnims.begin(), _TileAnims.end(), [&anim](const TileAnim& a) {return a.tile == anim.tile;}), _TileAnims.end());
                addTileAnim(anim.tile, anim.tile->pos, anim.tile->pos + Vec2(0, 5.0f), anim.tile->_scale, anim.origScale, anim.frames, false);
            } else {
                _TileAnims.erase(std::remove_if(_TileAnims.begin(), _TileAnims.end(), [&anim](const TileAnim& a) {return a.tile == anim.tile;}), _TileAnims.end());
            }
        }
    }
    
    for (auto& anim: _spriteNodeAnims) {
        if(!anim.done) {
            anim.update(dt);
        } else {
            _spriteNodeAnims.erase(std::remove_if(_spriteNodeAnims.begin(), _spriteNodeAnims.end(), [&anim](const SpriteNodeAnim& a) {
                return a.tile == anim.tile;}), _spriteNodeAnims.end());
        }
    }
    
    for (auto& anim: _fadeAnims){
        if(anim.active){
            anim.update(dt);
        } else {
            _fadeAnims.erase(std::remove_if(_fadeAnims.begin(), _fadeAnims.end(), [&anim](const FadeAnim& a) {return !anim.active;}), _fadeAnims.end());
        }
    }
}
