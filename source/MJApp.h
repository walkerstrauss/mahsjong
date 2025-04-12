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
#include "MJMenuScene.h"
#include "MJHostScene.h"
#include "MJClientScene.h"
#include "MJSettingScene.h"
#include "MJPauseScene.h"
#include "MJGameOverScene.h"
#include "MJNetworkController.h"
#include "MJAudioController.h"
#include "MJAnimationController.h"
#include "MJMatchController.h"

/**
 * This class represents the application root for the Mah's Jong game
 */
class MahsJongApp : public cugl::Application {
protected:
    /**
     * The current active scene
     */
    enum State {
        LOAD,
        MENU,
        HOST,
        CLIENT,
        GAME,
        SETTINGS,
        PAUSE,
        OVER,
        TILESETUI
    };
    
    /** Global asset manager */
    std::shared_ptr<cugl::AssetManager> _assets;
    /** Global sprite batch for drawing */
    std::shared_ptr<cugl::graphics::SpriteBatch> _batch;
    /** The network interface */
//    std::shared_ptr<cugl::netcode::NetcodeConnection> _network;
    /** Controller for loading scene */
    
    //
    cugl::scene2::LoadingScene _loading;
    
    //Loading _loading;
    
    /** The menu scene to choose what to do */
    MenuScene _mainmenu;
    /** The scene to host a game*/
    HostScene _hostgame;
    /** The scene to join a game */
    ClientScene _joingame;
    /** The primary controller for game world */
    GameScene _gameplay;
    /** The discard UI scene */
    DiscardUIScene _tilesetui; 
    /** The scene for settings */
    SettingScene _settings;
    /** The scene for pausing the game */
    PauseScene _pause;
    /** The scene for when the match ends/ is over */
    GameOverScene _gameover;
    /** The match controller for the game*/
    MatchController _matchController; 
    /** Whether or not we finished loading all assets*/
    bool _loaded;
    /** Scene loader reference */
    std::shared_ptr<cugl::scene2::Scene2Loader> _sceneLoader;
    /** Button to discard*/
    std::shared_ptr<cugl::scene2::Button> _discardBtn;
    /** The current active scene */
    State _scene;
    /** The network controller */
    std::shared_ptr<NetworkController> _network;
    /** Reference to audio controller for game sounds */
//    std::shared_ptr<AudioController> _audio;
public:
    /**
     * Creates, but does not initialized a new application. Remember,
     * no initialization in the constructor, only in init().
     */
    MahsJongApp() : cugl::Application() {
        _scene = State::LOAD;
    }
    
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
    
private:
    /**
     * Inidividualized update method for the loading scene.
     *
     * This method keeps the primary {@link #update} from being a mess of switch
     * statements. It also handles the transition logic from the loading scene.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void updateLoadingScene(float timestep);

    /**
     * Inidividualized update method for the menu scene.
     *
     * This method keeps the primary {@link #update} from being a mess of switch
     * statements. It also handles the transition logic from the menu scene.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void updateMenuScene(float timestep);

    /**
     * Inidividualized update method for the host scene.
     *
     * This method keeps the primary {@link #update} from being a mess of switch
     * statements. It also handles the transition logic from the host scene.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void updateHostScene(float timestep);
    
    /**
     * Inidividualized update method for the client scene.
     *
     * This method keeps the primary {@link #update} from being a mess of switch
     * statements. It also handles the transition logic from the client scene.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void updateClientScene(float timestep);

    /**
     * Individualized update method for the game scene.
     *
     * This method keeps the primary {@link #update} from being a mess of switch
     * statements. It also handles the transition logic from the game scene.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void updateGameScene(float timestep);
    
    /**
     * Individualized update method for the setting scene
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void updateSettingScene(float timestep);
    
    /**
     * Individualized update method for the pause scene
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void updatePauseScene(float timestep);
    
    /**
     * Individualized update method for the game over scene
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void updateGameOverScene(float timestep);
    
    /**
     * Individualized update method for the tileset UI scene
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void updateTilesetUIScene(float timestep);
    
    /**
     * Individualzed update method for the model controller
     *
     * @param timestep   The amount of time (in seconds) since the last frame 
     */
    void updateMatchController(float timestep);
};

#endif
