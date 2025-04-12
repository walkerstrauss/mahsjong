//
//  MJAnimatedNode.cpp
//  Mahsjong
//
//  Created by Walker Strauss on 4/9/25.
//

#include "MJAnimatedNode.h"

using namespace cugl;
using namespace cugl::scene2;
using namespace cugl::graphics;

AnimatedNode::AnimatedNode():
_rows(0), _cols(0), _frame(0), _limit(0), _timeSinceFrameAdvance(0.0f),_isPlaying(false),_isInterrupting(false) {}

AnimatedNode::~AnimatedNode() {}

bool AnimatedNode::initWithSheet(const std::shared_ptr<Texture>& texture, int rows, int cols){
    CUAssertLog(texture, "Texture must not be null");
    _texture = texture;
    return true;
}

bool AnimatedNode::initWithData(const AssetManager* assets, const std::shared_ptr<JsonValue>& json, float fps){
    CUAssertLog(json, "JsonValue must not be null");
    
    // Load all idle animations from the JSON
    auto idleAnims = json->get("idle");
    for (auto animJson : idleAnims->children()){
        Animation a;
        a.init(AnimationType::IDLE, animJson->getString("key"), animJson->getInt("rows"),animJson->getInt("cols"),animJson->getInt("count"), fps, true);
        _idleAnims[a.key] = a;
    }
    
    // Get default idle animation from JSON – if present
    if (idleAnims->has("default")){
        setDefaultIdleKey(idleAnims->getString("default"));
    }
    
    // Load all interrupting animations from the JSON
    auto interruptAnims = json->get("interrupt");
    for (auto animJson : interruptAnims->children()){
        Animation a;
        a.init(AnimationType::INTERRUPT, animJson->getString("key"), animJson->getInt("rows"), animJson->getInt("cols"), animJson->getInt("count"), fps, false);
        _interruptAnims[a.key] = a;
    }
    return true;
}

void AnimatedNode::play(const std::string& key, AnimationType type){
    const auto& animMap = (type == AnimationType::IDLE) ? _idleAnims : _interruptAnims;
    CUAssertLog(animMap.find(key) != animMap.end(), "Animation with key '%s' not found", key.c_str());
    
    _currKey = key;
    _currAnim = animMap.at(key);
    _timeSinceFrameAdvance = 0.0f;
    _isPlaying = true;
    _isInterrupting = (type == INTERRUPT);
    _currAnim.done = false;
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
            if (_currAnim.loop && !_isInterrupting){
                _frame = _currAnim.startFrame;
            } else {
                _isPlaying = false;
                _currAnim.done = true;
                
                if (_isInterrupting) {
                    _isInterrupting = false;
                    if (!_defaultIdleKey.empty()){
                        play(_defaultIdleKey, IDLE);
                    }
                }
                
                return;
            }
        }
        
        setFrame(_frame);
    }
}

void AnimatedNode::setFrame(int frame){
    CUAssertLog(frame >= 0 && frame < _limit, "Invalid animation frame %d", frame);
    
    _frame = frame;
    float x = (frame % _cols)*_bounds.size.width;
    float y = _texture->getSize().height - (1+frame/_cols)*_bounds.size.height;
    float dx = x-_bounds.origin.x;
    float dy = y-_bounds.origin.y;
    _bounds.origin.set(x,y);
    shiftTexture(dx, dy);
}

