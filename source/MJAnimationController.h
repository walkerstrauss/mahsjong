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
    float _frameTimer = 0.0f;
    float _frameDelay = 0.2f;
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
    
    /** Struct representation tile movement and scaling animation */
    struct TileAnim {
        std::shared_ptr<TileSet::Tile> tile;
        Vec2 startPos, endPos;
        float startScale, endScale;
        float origScale;
        int frames;
        int currFrame;
        bool done;
        /** Whether we are animating growing in selection */
        bool growing;
        
        TileAnim(std::shared_ptr<TileSet::Tile> tile, Vec2 startPos, Vec2 endPos, float startScale, float endScale, int fps, bool isGrowing = true) : tile(tile), startPos(startPos), endPos(endPos), startScale(startScale), endScale(endScale), origScale(startScale), currFrame(0), done(false), growing(isGrowing) {
            frames = fps;
        }
        
        void update(float dt){
            if (done) return;
            
            currFrame++;
            
            if (currFrame >= frames){
                currFrame = frames;
                done = true;
            }
            
            float diffTime = float(currFrame) / float(frames);
            tile->pos = Vec2(startPos * (1-diffTime) + endPos * diffTime);
            tile->_scale = startScale * (1-diffTime) + endScale * diffTime;
        }
    };
    
    /** Struct for sprite node animations */
    struct SpriteNodeAnim {
        std::shared_ptr<TileSet::Tile> tile;
        std::shared_ptr<scene2::SpriteNode> faceSpriteNode;
        std::shared_ptr<graphics::Texture> convergeSheet;
        std::shared_ptr<graphics::Texture> divergeSheet;
        
        int frames;
        int time;
        
        bool converging;
        bool diverging;
        bool done;

        SpriteNodeAnim(std::shared_ptr<TileSet::Tile>& tile, std::shared_ptr<cugl::graphics::Texture> convergeSheet, std::shared_ptr<cugl::graphics::Texture> divergeSheet, int fps) : tile(tile), convergeSheet(convergeSheet), divergeSheet(divergeSheet), frames(fps), converging(true), diverging(false), done(false) {
            faceSpriteNode = tile->getFaceSpriteNode();
            time = 0;
        };
        
        void update(float dt) {
            if(done) {
                return;
            }
            
            if(converging && !diverging && faceSpriteNode->getTexture() != convergeSheet) {
                faceSpriteNode->initWithSheet(convergeSheet, 4, 4);
                faceSpriteNode->setFrame(0);
            }
            
            CULog("HERHIUEHPIRHEIHUIPRHEEIPOHR");
            
            time += dt;
            
            if(time > 1.0f / frames) {
                if(converging) {
                    faceSpriteNode->setFrame(faceSpriteNode->getFrame() + 1);
                    if(faceSpriteNode->getFrame() > 15) {
                        converging = false;
                        diverging = true;
                        faceSpriteNode->initWithSheet(divergeSheet, 4, 4);
                        faceSpriteNode->setFrame(15);
                    }
                }
                else if(diverging) {
                    faceSpriteNode->setFrame(faceSpriteNode->getFrame() - 1);
                    if(faceSpriteNode->getFrame() < 0) {
                        faceSpriteNode->setTexture(tile->toString() + " new");
                        diverging = false;
                        done = true;
                    }
                }
            }
            CULog("here");
        }
    };
    
    /** Reference to asset manager for getting sprite sheets */
    std::shared_ptr<cugl::AssetManager> _assets;
    /** Vector holding sprites for controller animating */
    std::vector<SpriteSheetAnimation> _spriteSheetAnimations;
    /** Vector holding tile animations */
    std::vector<TileAnim> _TileAnims;
    /** Vector holding sprite node animations */
    std::vector<SpriteNodeAnim> _spriteNodeAnims;
    /** Whether the animation controller is currently paused */
    bool _paused;
    
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
    void addTileAnim(const std::shared_ptr<TileSet::Tile>& tile, Vec2 startPos, Vec2 endPos, float startScale, float endScale, int fps, bool isGrowing = true){
        _TileAnims.emplace_back(tile, startPos, endPos, startScale, endScale, fps, isGrowing);
    }
    
    /**
     * Method to add a sprite node animation
     */
    void addSpriteNodeAnim(std::shared_ptr<TileSet::Tile>& tile, std::shared_ptr<graphics::Texture> fromTexture,  std::shared_ptr<graphics::Texture> toTexture, int fps) {
        _spriteNodeAnims.emplace_back(tile, fromTexture, toTexture, fps);
    }
    
    bool isTileAnimated(const std::shared_ptr<TileSet::Tile>& tile){
        for (const auto& anim : _TileAnims){
            if (anim.tile == tile){
                return true;
            }
        }
        return false;
    }
    
    /**
     * Update all animations
     */
    void update(float dt);
    
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
        addTileAnim(tile, tile->pos, tile->pos + Vec2(0, 5.0f), tile->_scale, tile->_scale * 1.4f, f/2);
    }
    
    void animateTileDeselect(std::shared_ptr<TileSet::Tile> tile, float f){
        addTileAnim(tile, tile->pos, tile->pos - Vec2(0, 10.0f), tile->_scale, tile->_scale, f);
    }
    
    void animateTileMorph(std::shared_ptr<TileSet::Tile>& tile, std::shared_ptr<graphics::Texture> fromTexture,  std::shared_ptr<graphics::Texture> toTexture, float f) {
        addSpriteNodeAnim(tile, fromTexture, toTexture, f);
    }
};

#endif
