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
    
    for (auto& anim: _spriteNodeMorphAnims) {
        if(!anim.done) {
            anim.update(dt);
        } else {
            _spriteNodeMorphAnims.erase(std::remove_if(_spriteNodeMorphAnims.begin(), _spriteNodeMorphAnims.end(), [&anim](const SpriteNodeMorphAnim& a) {
                return a.tile == anim.tile;}), _spriteNodeMorphAnims.end());
        }
    }
    
    for (auto& anim : _spriteNodeFlipAnims) {
      anim.update(dt);
    }
    _spriteNodeFlipAnims.erase(std::remove_if(_spriteNodeFlipAnims.begin(), _spriteNodeFlipAnims.end(), [](const SpriteNodeFlipAnim& a){ return a.done; }), _spriteNodeFlipAnims.end());
}
