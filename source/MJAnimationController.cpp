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
bool AnimationController::init(const std::shared_ptr<cugl::AssetManager>& assets) {
    return true;
}

#pragma mark -
#pragma mark Gameplay Handling

void AnimationController::update(float dt) {
    if (_paused) return;

    // Update and prune sprite sheet animations
    for (auto& anim : _spriteSheetAnimations) {
        anim.update(dt);
        if (anim.done) {
            _spriteSheetAnimations.erase(
                std::remove_if(_spriteSheetAnimations.begin(), _spriteSheetAnimations.end(),
                    [&anim](const SpriteSheetAnimation& a) {
                        return a.node == anim.node;
                    }),
                _spriteSheetAnimations.end()
            );
        }
    }

    // Update sprite node morph animations
    for (auto& anim : _spriteNodeMorphAnims) {
        anim.update(dt);
    }

    // Update sprite node flip animations
    for (auto& anim : _spriteNodeFlipAnims) {
        anim.update(dt);
    }

    // Update fade animations
    for (auto& anim : _fadeAnims) {
        if (anim.active) {
            anim.update(dt);
        }
    }
    
    for (auto& anim: _bounceAnims) {
        anim.update(dt);
    }

    _spriteNodeFlipAnims.erase(std::remove_if(_spriteNodeFlipAnims.begin(), _spriteNodeFlipAnims.end(),[](const SpriteNodeFlipAnim& a) {
                return a.done;}),_spriteNodeFlipAnims.end());

    _spriteNodeMorphAnims.erase(std::remove_if(_spriteNodeMorphAnims.begin(), _spriteNodeMorphAnims.end(),[](const SpriteNodeMorphAnim& a) {
                return a.done;}),_spriteNodeMorphAnims.end());

    _fadeAnims.erase(std::remove_if(_fadeAnims.begin(), _fadeAnims.end(),[](const FadeAnim& a) {
                return !a.active;}),_fadeAnims.end());
    
    _bounceAnims.erase(std::remove_if(_bounceAnims.begin(), _bounceAnims.end(),[](const BounceAnim& a) {
                return a.done;}),_bounceAnims.end());
}


void AnimationController::CalcDampedSpringMotionParams(tDampedSpringMotionParams* pOutParams, float deltaTime, float angularFrequency, float dampingRatio) {
    const float epsilon = 0.0001f;
    
    if(dampingRatio < 0.0f) dampingRatio = 0.0f;
    if(angularFrequency < 0.0f) angularFrequency  = 0.0f;
    
    if(angularFrequency < epsilon) {
        pOutParams->m_posPosCoef = 1.0f; pOutParams->m_posVelCoef = 0.0f;
        pOutParams->m_velPosCoef = 0.0f; pOutParams->m_velVelCoef = 1.0f;
        return;
    }
    
    if (dampingRatio > 1.0f + epsilon)
    {
        float za = -angularFrequency * dampingRatio;
        float zb = angularFrequency * sqrtf(dampingRatio*dampingRatio - 1.0f);
        float z1 = za - zb;
        float z2 = za + zb;

        float e1 = expf( z1 * deltaTime );
        float e2 = expf( z2 * deltaTime );

        float invTwoZb = 1.0f / (2.0f*zb);
            
        float e1_Over_TwoZb = e1*invTwoZb;
        float e2_Over_TwoZb = e2*invTwoZb;

        float z1e1_Over_TwoZb = z1*e1_Over_TwoZb;
        float z2e2_Over_TwoZb = z2*e2_Over_TwoZb;

        pOutParams->m_posPosCoef =  e1_Over_TwoZb*z2 - z2e2_Over_TwoZb + e2;
        pOutParams->m_posVelCoef = -e1_Over_TwoZb    + e2_Over_TwoZb;

        pOutParams->m_velPosCoef = (z1e1_Over_TwoZb - z2e2_Over_TwoZb + e2)*z2;
        pOutParams->m_velVelCoef = -z1e1_Over_TwoZb + z2e2_Over_TwoZb;
    }

    else if (dampingRatio < 1.0f - epsilon) {
        // under-damped
        float omegaZeta = angularFrequency * dampingRatio;
        float alpha     = angularFrequency * sqrtf(1.0f - dampingRatio*dampingRatio);

        float expTerm = expf( -omegaZeta * deltaTime );
        float cosTerm = cosf( alpha * deltaTime );
        float sinTerm = sinf( alpha * deltaTime );
            
        float invAlpha = 1.0f / alpha;

        float expSin = expTerm*sinTerm;
        float expCos = expTerm*cosTerm;
        float expOmegaZetaSin_Over_Alpha = expTerm*omegaZeta*sinTerm*invAlpha;

        pOutParams->m_posPosCoef = expCos + expOmegaZetaSin_Over_Alpha;
        pOutParams->m_posVelCoef = expSin*invAlpha;

        pOutParams->m_velPosCoef = -expSin*alpha - omegaZeta*expOmegaZetaSin_Over_Alpha;
        pOutParams->m_velVelCoef =  expCos - expOmegaZetaSin_Over_Alpha;
    } else {
        // critically damped
        float expTerm     = expf( -angularFrequency*deltaTime );
        float timeExp     = deltaTime*expTerm;
        float timeExpFreq = timeExp*angularFrequency;

        pOutParams->m_posPosCoef = timeExpFreq + expTerm;
        pOutParams->m_posVelCoef = timeExp;

        pOutParams->m_velPosCoef = -angularFrequency*timeExpFreq;
        pOutParams->m_velVelCoef = -timeExpFreq + expTerm;
    }
}
