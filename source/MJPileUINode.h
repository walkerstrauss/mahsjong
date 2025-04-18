//
//  MJPileUINode.h
//  Mahsjong
//
//  Created by Tina Lu on 4/16/25.
//

#ifndef __MJ_PILE_UI_NODE_H__
#define __MJ_PILE_UI_NODE_H__

#include <cugl/cugl.h>

using namespace cugl;
using namespace cugl::scene2;
using namespace std;

/**
 * This node is responsible for displaying the pile UI within the game scene.
 */
class PileUINode : public cugl::scene2::SceneNode {
public:
    /** The state of the PileUINode */
    enum State {
        /** Idle State */
        IDLE,
        /** PileUI active */
        ON,
        /** PileUI not active */
        OFF
    };
    
protected:
    /** Asset manager for this game mode */
    std::shared_ptr<cugl::AssetManager> _assets;
    /** The current state of PileUINode */
    State _state;
    
public:
//    /** Input controller */
//    InputController input;
    /** The root scene node */
    std::shared_ptr<SceneNode> _root;
    
#pragma mark -
#pragma mark Constructors

    PileUINode() : cugl::scene2::SceneNode() {}
    
    /**
     * Initializes the discard UI node from assets.
     *
     * @param assets The asset manager for loading UI layout and resources
     * @return true if initialization was successful
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets);

    /**
     * Disposes of any resources allocated to this node.
     */
    void dispose() override;
    
#pragma mark -
#pragma mark UI Updates

    /**
     * Resets the internal state.
     */
    void reset();
    
    void update(float timestep) {
        return;
    }
    
    /** Gets the current state of this scene node */
    State getState() {
        return _state;
    }
    
    /** Sets the current state of this scene node */
    void setState(State state) {
        _state = state;
    }
    
    /** Sets this scene node as active */
    void setPileUIActive(bool active);
    
};

#endif /* __MJ_PILE_UI_NODE_H__ */
