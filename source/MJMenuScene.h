//
//  MJMenuScene.h
//  Mahsjong
//
//  Created by Patrick Choo on 3/3/25.
//

#ifndef __MJ_MENU_SCENE_H__
#define __MJ_MENU_SCENE_H__
#include <cugl/cugl.h>
#include <vector>
#include "MJAudioController.h"
#include "MJAnimationController.h"

/**
 * This class presents the menu to the player.
 *
 * There is no need for an input controller, as all input is managed by
 * listeners on the scene graph.  We only need getters so that the main
 * application can retrieve the state and communicate it to other scenes.
 */
class MenuScene : public cugl::scene2::Scene2 {
public:
    /**
     * The menu choice.
     *
     * This state allows the top level application to know what the user
     * chose.
     */
    enum Choice {
        /** User has not yet made a choice */
        NONE,
        /** User wants to host a game */
        HOST,
        /** User wants to join a game */
        JOIN,
        /** User wants to go to settings */
        SETTING,
        /** User wants to see the tutorial */
        TUTORIAL
    };

protected:
    /** Reference to the scene node for the homescene*/
    std::shared_ptr<cugl::scene2::SceneNode> _homescene;
    /** The asset manager for this scene. */
    std::shared_ptr<cugl::AssetManager> _assets;
    /** The menu button for hosting a game */
    std::shared_ptr<cugl::scene2::Button> _hostbutton;
    /** The menu button for joining a game */
    std::shared_ptr<cugl::scene2::Button> _joinbutton;
    /** The button to see the tutorial */
    std::shared_ptr<cugl::scene2::Button> _tutorialbutton;
    /** The player menu choice */
    Choice _choice;
    /** Reference to the sprite sheet for grandma animation*/
    std::shared_ptr<cugl::scene2::SpriteNode> _grandmaMainSheet;
    
    std::shared_ptr<cugl::scene2::SpriteNode> _rabbit;
    std::shared_ptr<cugl::scene2::SpriteNode> _rat;
    

    float frameTimer = 0.0f;
    float frameDelay = 0.2f;
public:
    /** The menu button to see the settings */
    std::shared_ptr<cugl::scene2::Button> settingsbutton;
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new  menu scene with the default values.
     *
     * This constructor does not allocate any objects or start the game.
     * This allows us to use the object without a heap pointer.
     */
    MenuScene() : cugl::scene2::Scene2() {}
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~MenuScene() { dispose(); }
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose() override;
    
    /**
     * Initializes the controller contents.
     *
     * In previous labs, this method "started" the scene.  But in this
     * case, we only use to initialize the scene user interface.  We
     * do not activate the user interface yet, as an active user
     * interface will still receive input EVEN WHEN IT IS HIDDEN.
     *
     * That is why we have the method {@link #setActive}.
     *
     * @param assets    The (loaded) assets for this game mode
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets);

#pragma mark -
#pragma mark Gameplay Handlers
    
    /**
     * Sets whether the scene is currently active
     *
     * This method should be used to toggle all the UI elements.  Buttons
     * should be activated when it is made active and deactivated when
     * it is not.
     *
     * @param value whether the scene is currently active
     */
    virtual void setActive(bool value) override;
    
    /**
     * Returns the user's menu choice.
     *
     * This will return NONE if the user had no yet made a choice.
     *
     * @return the user's menu choice.
     */
    Choice getChoice() const { return _choice; }

    void render() override;
    
    void update(float timestep) override;
};

#endif /* __MJ_MENU_SCENE_H__ */

