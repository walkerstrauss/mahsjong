//
//  MJAnimatedNode.cpp
//  Mahsjong
//
//  Created by Walker Strauss on 4/9/25.
//

#include "MJAnimatedNode.h"

#include <cugl/graphics/CUTexture.h>
#include <cugl/core/math/CUPoly2.h>
#include <cugl/scene2/CUTexturedNode.h>

using namespace cugl;
using namespace cugl::scene2;
using namespace cugl::graphics;

AnimatedNode::AnimatedNode():
_rows(0), _cols(0), _frame(0), _limit(0), _timeSinceFrameAdvance(0.0f),_isPlaying(false),_isInterrupting(false) {}

AnimatedNode::~AnimatedNode() {}

void AnimatedNode::play(const std::string& key, AnimationType type, const std::shared_ptr<Texture>& texture){
    const auto& animMap = (type == AnimationType::IDLE) ? _idleAnims : _interruptAnims;
    auto it = animMap.find(key);
    CUAssertLog(it != animMap.end(), "Animation with key '%s' not found", key.c_str());
    
    _currAnim = it->second;
    _currKey = key;
    if (texture){
        setTexture(texture);
    }
    _frame = _currAnim.startFrame;
    _currAnim.playedCount = 0;
    _currAnim.done = false;
    _timeSinceFrameAdvance = 0.0f;
    _isPlaying = true;
    _isInterrupting = (type == INTERRUPT);
    
    _rows = _currAnim.rows;
    _cols = _currAnim.cols;
    _limit = _currAnim.endFrame + 1;
    _currAnim.done = false;
    initWithSheet(_texture, _rows, _cols, _limit);
    setFrame(_frame);
    return;
}

void AnimatedNode::stop(){
    _isPlaying = false;
    _isInterrupting = false;
}

void AnimatedNode::update(float timestep){
    if (!_isPlaying) return;
    _timeSinceFrameAdvance += timestep;
    
    float frameDur = 1.0f / _currAnim.fps;
    if (_timeSinceFrameAdvance >= frameDur){
        _timeSinceFrameAdvance -= frameDur;
        _frame++;
        
        if (_frame > _currAnim.endFrame) {
            _currAnim.playedCount++;
            
            if (_currAnim.shouldReplay()){
                _frame = _currAnim.startFrame;
            } else {
                _isPlaying = false;
                _currAnim.done = true;
                
                if (_isInterrupting) {
                    _isInterrupting = false;
                    if (!_defaultIdleKey.empty()){
                        play(_defaultIdleKey, IDLE, Texture::getBlank());
                        setVisible(false);
                    }
                }
                
                return;
            }
        }
        
        setFrame(_frame);
    }
}
