//
//  MJAnimationController.h
//  Mahsjong
//
//  Created by Walker Strauss on 3/31/25.
//

#ifndef __MJ_ANIMATION_CONTROLLER_H__
#define __MJ_ANIMATION_CONTROLLER_H__
#include "MJGameScene.h"
#include "MJMenuScene.h"
#include "MJHostScene.h"
#include "MJClientScene.h"
#include "MJSettingScene.h"
#include "MJPauseScene.h"
#include "MJGameOverScene.h"
#include "MJTileSet.h"
#include "MJPile.h"

using namespace cugl;
using namespace cugl::graphics;
using namespace cugl::scene2;

class AnimationController {
private:
    static AnimationController* _instance;
    /** Struct that stores information about an sprite node being animated */
    struct SpriteSheetAnimation {
        std::shared_ptr<scene2::SpriteNode> node;
        int begin;
        int end;
        bool loop;
        
        int currFrame;
        float time;
        bool done;
        float fps;
        
        SpriteSheetAnimation(std::shared_ptr<SpriteNode> animNode, int first, int last, bool l, float framesPerSecond = 12.0f) : node(animNode), begin(first), end(last), loop(l), currFrame(first), time(0.0f), done(false), fps(framesPerSecond) {
            node->setFrame(currFrame);
        }
        
        void update(float dt) {
            if (done){
                return;
            }
            
            time += dt;
            
            if (time > 1.0f / fps){
                currFrame++;
                time = 0.0f;
            }
            
            if (currFrame > end){
                if (loop){
                    currFrame = begin;
                } else {
                    currFrame = end;
                    done = true;
                }
            }
            
            node->setFrame(currFrame);
        }
    };
    
    /** Struct representation tile select animation */
    struct SelectAnim {
        std::shared_ptr<TileSet::Tile> tile;
        Vec2 startPos, endPos;
        float startScale, endScale;
        float elapsed;
        int frames;
        bool done;
        
        SelectAnim(std::shared_ptr<TileSet::Tile> tile, Vec2 startPos, Vec2 endPos, float startScale, float endScale, int fps) : tile(tile), startPos(startPos), endPos(endPos), startScale(startScale), endScale(endScale), elapsed(0.0), done(false) {
            frames = fps;
        }
        
        void update(float dt){
            if (done) return;
            elapsed += dt;
            int currFrame = static_cast<int>(elapsed);
            if (currFrame >= frames){
                currFrame = frames;
                done = true;
            }
            
            float diffTime = float(currFrame) / float(frames);
            tile->pos = Vec2(startPos * (1-diffTime) + endPos * diffTime);
            tile->_scale = startScale * (1-diffTime) + endScale * diffTime;
        }
    };
    
    /** Reference to asset manager for getting sprite sheets */
    std::shared_ptr<cugl::AssetManager> _assets;
    /** Vector holding sprites for controller animating */
    std::vector<SpriteSheetAnimation> _spriteSheetAnimations;
    /** Vector holding tile animations */
    std::vector<SelectAnim> _SelectAnims;
    /** Whether the animation controller is currently paused */
    bool _paused;
    /** Whether we are animating growing in selection */
    bool _growing;
    /** Whether we are animating shrinking in selection */
    bool _shrinking;
    
public:
#pragma mark -
#pragma mark Constructors
    
    static AnimationController& getInstance(){
        if (!_instance){
            _instance = new AnimationController();
        }
        return *_instance;
    }
    
    AnimationController() : _assets(nullptr ) {}
    
    /**
     * Initializes the animation controller with an asset manager
     *
     * @param assets    The asset manager to get game sounds from
     * @return true if initialization was successful, and false otherwise
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets);
    
#pragma mark -
#pragma mark Gameplay Handling
    /**
     * Method to add an animation to the vector of animations for this controller
     */
    void addSpriteSheetAnimation(const std::shared_ptr<scene2::SpriteNode>& node, int first, int last, bool loop, float fps = 12.0f){
        _spriteSheetAnimations.emplace_back(node, first, last, loop, fps);
    }
    
    /**
     * Method to add a tile animation
     */
    void addSelectAnim(const std::shared_ptr<TileSet::Tile>& tile, Vec2 startPos, Vec2 endPos, float startScale, float endScale, int fps){
        _SelectAnims.emplace_back(tile, startPos, endPos, startScale, endScale, fps);
    }
    
    /**
     * Update all animations
     */
    void update(float dt) {
        if (_paused) return;
        
        for (auto& anim : _spriteSheetAnimations) {
            anim.update(dt);
        }
        
        for (auto& anim: _SelectAnims) {
            anim.update(dt);
        }
    }
    
    /**
     * Pause all animations
     */
    void pause(){
        _paused = true;
    }
    
    /**
     * Resume all animations
     */
    void resume(){
        _paused = false;
    }
    
    /**
     * Reset all animations
     */
    void reset(){
        for (auto& anim : _spriteSheetAnimations){
            anim.currFrame = anim.begin;
            anim.time = 0.0f;
            anim.done = false;
        }
    }
    
    /** Stop all animations */
    void stop(){
        for (auto& anim : _spriteSheetAnimations) {
            anim.done = true;
        }
    }
    
    void animateTileSelect(std::shared_ptr<TileSet::Tile> tile, float f){
        addSelectAnim(tile, tile->pos, tile->pos + Vec2(0, 3.0f), tile->_scale, tile->_scale * 1.4f, f);
    }
    
    void animateTileDeselect(std::shared_ptr<TileSet::Tile> tile, float f){
        addSelectAnim(tile, tile->pos, tile->pos - Vec2(0, 3.0f), tile->_scale, tile->_scale, f);
    }
    
};

#endif
