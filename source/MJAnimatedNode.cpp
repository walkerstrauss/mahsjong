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

bool AnimatedNode::initWithSheet(const std::shared_ptr<Texture>& texture, int rows, int cols, int limit){
    _rows = rows;
    _cols = cols;
    _limit = limit;
    setTexture(texture);
    
    Size frameSize = texture->getSize();
    frameSize.width /= cols;
    frameSize.height /= rows;
    
    _bounds.origin.set(0,0);
    _bounds.size = frameSize;
    _isPlaying = true;
    if (_currAnim.type == INTERRUPT){
        _isInterrupting = true;
    }
    
    Poly2 poly;
    poly.set(_bounds);
    setPolygon(poly);
    
    refresh();
    
    setContentSize(frameSize);
    return true;
}

bool AnimatedNode::initWithData(const AssetManager* assets, const std::shared_ptr<JsonValue>& value, std::string nodeKey, float fps){
    CUAssertLog(value, "JsonValue must not be null");
    auto json = value->get(nodeKey);
    CUAssertLog(json, "Node json value must be defined aka not null");
    
    if (!value) {
        return TexturedNode::init();
    } else if (!TexturedNode::initWithData(assets, json->get("idle")->get("default"))) {
        return false;
    }
    
    // Load all idle animations from the JSON
    auto idleAnims = json->get("idle");
    CUAssertLog(idleAnims && !idleAnims->children().empty(), "Idle anims must be present");
    for (auto animJson : idleAnims->children()){
        Animation a;
        a.init(AnimationType::IDLE,
               animJson->getString("texture"),
               animJson->getInt("rows"),
               animJson->getInt("cols"),
               animJson->getInt("count"),
               fps,
               -1);
        _idleAnims[a.key] = a;
    }
    // Load all interrupting animations from the JSON
    auto interruptAnims = json->get("interrupt");
    for (auto animJson : interruptAnims->children()){
        int repeat = animJson->has("repeat") ? animJson->getInt("repeat") : 1;
        Animation a;
        a.init(AnimationType::INTERRUPT,
               animJson->getString("texture"),
               animJson->getInt("rows"),
               animJson->getInt("cols"),
               animJson->getInt("count"),
               fps,
               repeat);
        _interruptAnims[a.key] = a;
    }
    setVisible(false);
    
    _bounds.size = _texture->getSize();
    _bounds.size.width  /= _cols;
    _bounds.size.height /= _rows;
    _bounds.origin.x = (_frame % _cols)*_bounds.size.width;
    _bounds.origin.y = _texture->getSize().height - (1+_frame/_cols)*_bounds.size.height;

    // And position it correctly
    Vec2 coord = getPosition();
    setPolygon(_bounds);
    setPosition(coord);
    refresh();
    return true;
}

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

void AnimatedNode::setFrame(int frame){
    CUAssertLog(frame >= 0 && frame < _limit, "Invalid animation frame %d", frame);
        _frame = frame;

        // Calculate the new bounds for this frame in the texture atlas
        float x = (frame % _cols) * _bounds.size.width;
        float y = (frame / _cols) * _bounds.size.height;

        // Debug: Print out the current bounds before updating
        std::cout << "Old bounds: " << _bounds.origin.x << ", " << _bounds.origin.y << std::endl;

        // Recalculate dx, dy based on the new bounds
        float dx = x - _bounds.origin.x;
        float dy = y - _bounds.origin.y;
    
        _bounds.origin.set(x,y);
        setContentSize(_bounds.size);
    
        // Debug: Print out the new bounds
        std::cout << "New bounds: " << _bounds.origin.x << ", " << _bounds.origin.y << std::endl;

        // Shift the texture based on the calculated offsets
        std::cout << "Shifting texture by dx: " << dx << ", dy: " << dy << std::endl;
        shiftTexture(dx, dy);
        refresh();
}

