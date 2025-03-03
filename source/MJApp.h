//
//  MJApp.h
//  Mahsjong
//
//  Created by Patrick Choo on 2/17/25.
//

#ifndef __MJ_APP_H__
#define __MJ_APP_H__
#include <cugl/cugl.h>
#include "MJGameScene.h"

/**
 * This class represents the application root for the Mah's Jong game
 */
class MahsJongApp : public cugl::Application{
protected:
    /** Global asset manager */
    std::shared_ptr<cugl::AssetManager> _assets;
    /** Global sprite batch for drawing */
    std::shared_ptr<cugl::graphics::SpriteBatch> _batch;
    /** Controller for loading scene */
    cugl::scene2::LoadingScene _loading;
    /** The primary controller for game world */
    GameScene _gameplay;
    /** Whether or not we finished loading all assets*/
    bool _loaded;
    /** Scene2 object for match scene */
    std::shared_ptr<cugl::scene2::Scene2> _matchScene;
    /** Scene loader reference */
    std::shared_ptr<cugl::scene2::Scene2Loader> _sceneLoader;
    /** Button to discard*/
    std::shared_ptr<cugl::scene2::Button> _discardBtn;
public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates, but does not initialized a new application. Remember,
     * no initialization in the constructor, only in init().
     */
    MahsJongApp() : cugl::Application(), _loaded(false) {}
    
    /**
     * Destructor when application quits
     */
    ~MahsJongApp() { }
    
#pragma mark -
#pragma mark Application State
    /**
     * The method called after OpenGL is initialized, but before the application
     *
     * This is the method in which all user-defined program initialization should take place.
     * You should not create a new init() method.
     */
    virtual void onStartup() override;
    
    /**
     * The method called when the application is ready to quit.
     *
     * This is the method to dispose of all resources allocated by this
     * application. As a rule of thumb, everything created in onStartup() should
     * be deleted here
     */
    virtual void onShutdown() override;
    
#pragma mark -
#pragma mark Application Loop

    /** Method to update the application data */
    virtual void update(float timestep) override;
    
    /** The method called to draw the appplication to the screen */
    virtual void draw() override;
};

#endif
