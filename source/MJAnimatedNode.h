//
//  MJAnimatedNode.h
//  Mahsjong
//
//  Created by Walker Strauss on 4/9/25.
//

#ifndef __MJ_ANIMATED_NODE_H__
#define __MJ_ANIMATED_NODE_H__
#include <cugl/scene2/CUPolygonNode.h>
#include <cugl/core/math/CURect.h>
#include <cugl/graphics/CUTexture.h>
#include <unordered_map>

using namespace cugl;
using namespace cugl::scene2;
using namespace cugl::graphics;

/**
 * Class representing an animated scene node in the game – all tiles will be animated nodes by open beta
 */
class AnimatedNode : public PolygonNode {
public:
    enum AnimationType {
        IDLE,
        INTERRUPT
    };
    
protected:
    struct Animation {
        AnimationType type;
        std::string key;
        int rows, cols, startFrame, endFrame;
        float fps;
        int repeatCount = -1; // -1 means infinite
        int playedCount = 0;
        bool loop = true;
        bool done = false;
        
        bool init (AnimationType t, std::string k, int r, int c, int endF, float f, int repeat){
            type = t;
            key = k;
            rows = r;
            cols = c;
            startFrame = 0;
            endFrame = endF;
            fps = f;
            repeatCount = (t == IDLE) ? -1 : repeat; // Idle always infinite
            loop = (repeatCount == -1);
            done = false;
            return true;
        }
        
        bool shouldReplay() const {
            return type == IDLE || repeatCount == -1 || playedCount < repeatCount;
        }
    };
    
    std::unordered_map<std::string, Animation> _idleAnims, _interruptAnims;
    std::string _currKey;
    Animation _currAnim;
    float _timeSinceFrameAdvance;
    int _rows, _cols;
    int _frame, _limit;
    bool _isPlaying, _isInterrupting;
    Rect _bounds;
    std::string _defaultIdleKey;
public:
    /**
     * Default constructor
     **/
    AnimatedNode();
    
    /**
     * Destructor
     **/
    ~AnimatedNode();
    
    bool initWithSheet(const std::shared_ptr<Texture>&, int rows, int cols);
    
    bool initWithData(const AssetManager* assets, const std::shared_ptr<JsonValue>& json, float fps);
    
    void play(const std::string& key, AnimationType type);
    
    void stop();
    
    void update(float dt);
    
    bool isAnimating() const { return _isPlaying; }
    
    const std::string& getCurrentKey() const { return _currKey; }
    
    void setFrame(int frame);
    
    void setDefaultIdleKey(const std::string& key) { _defaultIdleKey = key; }
};

#endif
