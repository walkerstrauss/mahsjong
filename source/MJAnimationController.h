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
    /** Reference to asset manager for getting sprite sheets */
    std::shared_ptr<cugl::AssetManager> _assets;
    
    /** Struct that stores information about an sprite node being animated */
    struct Animation {
        std::shared_ptr<scene2::SpriteNode> node;
        int begin;
        int end;
        bool loop;
        
        int currFrame;
        float time;
        bool done;
        float fps;
        
        Animation(std::shared_ptr<SpriteNode> animNode, int first, int last, bool l, float framesPerSecond = 12.0f) : node(animNode), begin(first), end(last), loop(l), currFrame(first), time(0.0f), done(false), fps(framesPerSecond) {
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
    
    /** Vector holding sprites for controller animating */
    std::vector<Animation> _animations;
    
public:
#pragma mark -
#pragma mark Constructors
    AnimationController() : _assets(nullptr ) {}
    
    /**
     * Initializes the animation controller with an asset manager
     *
     * @param assets    The asset manager to get game sounds from
     * @param scene     The scene whose sprites this controller will animate
     * @return true if initialization was successful, and false otherwise
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, MenuScene scene);
    
    /**
     * Initializes the animation controller with an asset manager
     *
     * @param assets    The asset manager to get game sounds from
     * @param scene     The scene whose sprites this controller will animate
     * @return true if initialization was successful, and false otherwise
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, HostScene scene);
    
    /**
     * Initializes the animation controller with an asset manager
     *
     * @param assets    The asset manager to get game sounds from
     * @param scene     The scene whose sprites this controller will animate
     * @return true if initialization was successful, and false otherwise
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, ClientScene scene);
    
    /**
     * Initializes the animation controller with an asset manager
     *
     * @param assets    The asset manager to get game sounds from
     * @param scene     The scene whose sprites this controller will animate
     * @return true if initialization was successful, and false otherwise
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, GameScene scene);
    
    /**
     * Initializes the animation controller with an asset manager
     *
     * @param assets    The asset manager to get game sounds from
     * @param scene     The scene whose sprites this controller will animate
     * @return true if initialization was successful, and false otherwise
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, SettingScene scene);
    
    /**
     * Initializes the animation controller with an asset manager
     *
     * @param assets    The asset manager to get game sounds from
     * @param scene     The scene whose sprites this controller will animate
     * @return true if initialization was successful, and false otherwise
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, PauseScene scene);
    
    /**
     * Initializes the animation controller with an asset manager
     *
     * @param assets    The asset manager to get game sounds from
     * @param scene     The scene whose sprites this controller will animate
     * @return true if initialization was successful, and false otherwise
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, DiscardUIScene scene);
    
    /**
     * Initializes the animation controller with an asset manager
     *
     * @param assets    The asset manager to get game sounds from
     * @param scene     The scene whose sprites this controller will animate
     * @return true if initialization was successful, and false otherwise
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, GameOverScene scene);
    
#pragma mark -
#pragma mark Gameplay Handling
    /**
     * Method to add an animation to the vector of animations for this controller
     */
    void addAnimation(const std::shared_ptr<scene2::SpriteNode>& node, int first, int last, bool loop, float fps = 12.0f){
        _animations.emplace_back(node, first, last, loop, fps);
    }
    
    /**
     * Update all animations
     */
    void update(float dt) {
        for (auto& anim : _animations){
            anim.update(dt);
        }
    }
    
    /**
     * Pause all animations
     */
    void pause(){
        for (auto& anim : _animations){
            anim.time = 0.0f;
        }
    }
    
    /**
     * Reset all animations
     */
    void reset(){
        for (auto& anim : _animations){
            anim.currFrame = anim.begin;
            anim.time = 0.0f;
            anim.done = false;
        }
    }
    
    void stop(){
        for (auto& anim : _animations) {
            anim.done = true;
        }
    }
};

#endif
