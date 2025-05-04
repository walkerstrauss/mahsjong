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
    struct SpriteNodeMorphAnim {
        std::shared_ptr<TileSet::Tile> tile;
        std::shared_ptr<scene2::TexturedNode> backTextureNode;
        std::shared_ptr<scene2::SceneNode> container;
        std::shared_ptr<graphics::Texture> convergeSheet;
        std::shared_ptr<graphics::Texture> divergeSheet;
        std::shared_ptr<graphics::Texture> idle;
        
        int frames;
        int currFrame;
        float time;
        
        bool converging;
        bool diverging;
        bool done;
        
        SpriteNodeMorphAnim(std::shared_ptr<TileSet::Tile>& tile, std::shared_ptr<cugl::graphics::Texture> convergeSheet, std::shared_ptr<cugl::graphics::Texture> divergeSheet, std::shared_ptr<cugl::graphics::Texture> idle, int fps) : tile(tile), convergeSheet(convergeSheet), divergeSheet(divergeSheet), frames(fps), converging(true), diverging(false), done(false), currFrame(0), idle(idle){
            backTextureNode = tile->getBackTextureNode();
            container = tile->getContainer();
            time = 0;
            
            tile->getContainer()->removeChild(tile->getFaceSpriteNode());
            
            tile->setFrontSpriteNode(scene2::SpriteNode::allocWithSheet(convergeSheet, 4, 4));
            tile->getFaceSpriteNode()->setFrame(currFrame);
            
            float width_origin = tile->getContainer()->getContentSize().width/2;
            float height_origin = tile->getContainer()->getContentSize().height/2;
            
            tile->getFaceSpriteNode()->setAnchor(Vec2::ANCHOR_CENTER);
            tile->getFaceSpriteNode()->setPosition(width_origin, height_origin);
            
            tile->getContainer()->addChild(tile->getFaceSpriteNode());
        };
        
        void update(float dt) {
            if(done) {
                return;
            }
            
            time += dt;
            
            if(time > 1.0f / frames) {
                currFrame = converging ? currFrame + 1 : currFrame - 1;
                time = 0.0f;
                
                if(converging && !diverging) {
                    if(currFrame > 15) {
                        currFrame = 15;
                        tile->getContainer()->removeChild(tile->getFaceSpriteNode());
                        
                        tile->setFrontSpriteNode(scene2::SpriteNode::allocWithSheet(divergeSheet, 4, 4));
                        tile->getFaceSpriteNode()->setFrame(currFrame);
                        
                        float width_origin = tile->getContainer()->getContentSize().width/2;
                        float height_origin = tile->getContainer()->getContentSize().height/2;
                        
                        tile->getFaceSpriteNode()->setAnchor(Vec2::ANCHOR_CENTER);
                        tile->getFaceSpriteNode()->setPosition(width_origin, height_origin);
                        
                        tile->getContainer()->addChild(tile->getFaceSpriteNode());
                        
                        converging = false;
                        diverging = true;
                    }
                }
                else if (diverging && !converging) {
                    if(currFrame < 0) {
                        tile->getContainer()->removeChild(tile->getFaceSpriteNode());
                        
                        tile->setFrontSpriteNode(scene2::SpriteNode::allocWithSheet(idle, 1, 1));
                        
                        float width_origin = tile->getContainer()->getContentSize().width/2;
                        float height_origin = tile->getContainer()->getContentSize().height/2;
                        
                        tile->getFaceSpriteNode()->setAnchor(Vec2::ANCHOR_CENTER);
                        tile->getFaceSpriteNode()->setPosition(width_origin, height_origin);
                        
                        tile->getContainer()->addChild(tile->getFaceSpriteNode());
                        
                        diverging = false;
                        done = true;
                    }
                }
            }
            
            if(diverging || converging) {
                tile->getFaceSpriteNode()->setFrame(currFrame);
            }
        }
    };
    
    struct SpriteNodeFlipAnim {
        std::shared_ptr<TileSet::Tile> tile;
        std::shared_ptr<graphics::Texture> frontTexture;
        std::shared_ptr<graphics::Texture> backTexture;
        
        int frames;
        int currFrame;
        float scale;
        float time;
        
        bool done;
        bool converging;
        bool diverging;
        bool flipToFace;
        
        float speed = 8.0f;
        
        SpriteNodeFlipAnim(std::shared_ptr<TileSet::Tile> tile, std::shared_ptr<graphics::Texture> frontTexture, std::shared_ptr<graphics::Texture> backTexture, float scale, int fps, bool flipToFace) : tile(tile), frontTexture(frontTexture), backTexture(backTexture), scale(scale), frames(fps), currFrame(0), time(0), done(false), flipToFace(flipToFace) {
            tile->animating = true;
            converging = true;
            diverging = false;
        };
        
        void update(float dt) {
            if (done) {
                return;
            }
            
            time += dt * speed;
            
            if(time > 1.0f / frames) {
                currFrame += 1;
                time = 0.0f;
                if(converging) {
                    if(currFrame > frames) {
                        tile->getContainer()->removeChild(tile->getBackTextureNode());
                        
                        float width_origin = tile->getContainer()->getContentSize().width/2;
                        float height_origin = tile->getContainer()->getContentSize().height/2;
                        
                        tile->setBackTexture(backTexture);
                        tile->getBackTextureNode()->setAnchor(Vec2::ANCHOR_CENTER);
                        tile->getBackTextureNode()->setPosition(width_origin, height_origin);
                        
                        tile->getContainer()->addChild(tile->getBackTextureNode());
                        
                        if(flipToFace) {
                            tile->getContainer()->removeChild(tile->getFaceSpriteNode());
                            tile->getContainer()->addChild(tile->getFaceSpriteNode());
                        }
                        
                        converging = false;
                        diverging = true;
                        
                        currFrame = 0;
                    }
                }
                else if (diverging && currFrame > frames) {
                    tile->animating = false;
                    diverging = false;
                    done = true;
                }
                
                if (converging || diverging){
                    float xScale = tile->getContainer()->getScale().x;
                    if (converging) {
                        xScale -= scale / frames;
                    }
                    else {
                        xScale += scale / frames;
                    }
                    tile->getContainer()->setScale(xScale, tile->getContainer()->getScale().y);
                }
            }
        }
    };
    
    /** Reference to asset manager for getting sprite sheets */
    std::shared_ptr<cugl::AssetManager> _assets;
    /** Vector holding sprites for controller animating */
    std::vector<SpriteSheetAnimation> _spriteSheetAnimations;
    /** Vector holding tile animations */
    std::vector<TileAnim> _TileAnims;
    /** Vector holding sprite node animations */
    std::vector<SpriteNodeMorphAnim> _spriteNodeMorphAnims;
    std::vector<SpriteNodeFlipAnim> _spriteNodeFlipAnims;
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
    void addSpriteNodeMorphAnim(std::shared_ptr<TileSet::Tile>& tile, std::shared_ptr<graphics::Texture> fromTexture, std::shared_ptr<graphics::Texture> toTexture, std::shared_ptr<graphics::Texture> idle, int fps) {
        _spriteNodeMorphAnims.emplace_back(tile, fromTexture, toTexture, idle, fps);
    }
    
    void addSpriteNodeFlipAnim(std::shared_ptr<TileSet::Tile>& tile, std::shared_ptr<graphics::Texture> frontTexture, std::shared_ptr<graphics::Texture> backTexture, float scale, int fps, bool flipToFace) {
        _spriteNodeFlipAnims.emplace_back(tile, frontTexture, backTexture, scale, fps, flipToFace);
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
    
    void animateTileMorph(std::shared_ptr<TileSet::Tile>& tile, std::shared_ptr<graphics::Texture> fromTexture, std::shared_ptr<graphics::Texture> toTexture, std::shared_ptr<graphics::Texture> idle, float f) {
        addSpriteNodeMorphAnim(tile, fromTexture, toTexture, idle, f);
    }
    
    void animateTileFlip(std::shared_ptr<TileSet::Tile>& tile, std::shared_ptr<Texture>& frontTexture, std::shared_ptr<Texture>& backTexture, float scale, float f, bool flipToFace) {
        addSpriteNodeFlipAnim(tile, frontTexture, backTexture, scale, f, flipToFace);
    }
};

#endif
