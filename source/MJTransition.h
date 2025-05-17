//
//  MJTransition.h
//  Mahsjong
//
//  Created by Patrick Choo on 5/10/25.
//

#ifndef __MJ_TRANSITION_H__
#define __MJ_TRANSITION_H__

#include <cugl/cugl.h>
#include <string>
#include "MJNetworkController.h"
#include "MJAnimationController.h"

using namespace cugl;
using namespace cugl::scene2;
using namespace cugl::netcode;
using namespace std;

/**
 * This class is used for transitioning throughout scenes. It is also used for the
 * splash screen.
 */

class TransitionScene : public cugl::scene2::Scene2 {
public:
    enum State {
        START,
        LOADING,
        FINISH
    };
    
    /** Returns the state of the transition scene*/
    State getState() { return _state; }
    
    /** Sets the current state of the transition scene*/
    void setState(State state) { _state = state; }
protected:
    /** Asset manager for this game mode */
    std::shared_ptr<AssetManager> _assets;
    /** The network controller (as made by this scene) */
    std::shared_ptr<NetworkController> _network;
    
    /** Background for transition scren */
    Rect background;
    /** Scene node for team logo */
    std::shared_ptr<TexturedNode> teamLogo;
    
    /** The current state of the transition scene */
    State _state;
public:
    /**
     * Creates a transition scene with the default values
     *
     * This constructor does not allocate any objects or start the game.
     * This allows us to use the object without a heap pointer
     */
    TransitionScene() : cugl::scene2::Scene2() {}
    
    /**
     * Initializes the transition scene contents
     *
     * The constructor does not allocate any objects or memory. This allows
     * us to have a non-pointer reference to this controller, reducing our memory
     * allocation. Instead, allocation happens in this method
     *
     * @param assets    the asset manager for the game
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets);
    
    /**
     * The method called to update the transition scene
     *
     * @param timestep The amount of time (in seconds) since the last frame
     */
    void update(float dt) override;
    
    /** Renders the current scene to the screen **/
    void render(std::shared_ptr<cugl::graphics::SpriteBatch>& batch);
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose() override;
    
};

#endif /* __MJ_GAME_SCENE_H__ */
