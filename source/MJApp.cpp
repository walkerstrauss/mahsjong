//
//  MJApp.cpp
//  Mahsjong
//
//  Created by Patrick Choo on 2/17/25.
//

#include "MJApp.h"

using namespace cugl;
using namespace cugl::graphics;
using namespace cugl::audio;
using namespace cugl::scene2;


#pragma mark -
#pragma mark Gameplay Control

/**
 * The method called after OpenGL is initialized, but before the application
 *
 * This is the method in which all user-defined program initialization should take place.
 * You should not create a new init() method.
 */
void MahsJongApp::onStartup() {
    _assets = AssetManager::alloc();
    _batch = SpriteBatch::alloc();
    auto cam = OrthographicCamera::alloc(getDisplaySize());
    
    // Start-up basic input (DESKTOP ONLY)
    Input::activate<Mouse>();
    Input::get<Mouse>()->setPointerAwareness(Mouse::PointerAwareness::DRAG);
    Input::activate<Keyboard>();
    
    _assets->attach<Texture>(TextureLoader::alloc()->getHook());
    _assets->attach<Sound>(SoundLoader::alloc()->getHook());
    _assets->attach<Font>(FontLoader::alloc()->getHook());
    _assets->attach<JsonValue>(JsonLoader::alloc()->getHook());
    _assets->attach<WidgetValue>(WidgetLoader::alloc()->getHook());
    _assets->attach<Button>(WidgetLoader::alloc()->getHook());
    _assets->attach<scene2::SceneNode>(Scene2Loader::alloc()->getHook());
    _assets->loadDirectory("json/loading.json");

    //Create a "loading" screen
    _scene = State::LOAD;
    _loading.init(_assets, "json/assets.json");
    _loading.setSpriteBatch(_batch);
    
    _loading.start();
    AudioEngine::start();
    netcode::NetworkLayer::start(netcode::NetworkLayer::Log::INFO);
    Application::onStartup(); //YOU MUST END with call to parent
};

/**
 * The method called when the application is ready to quit.
 *
 * This is the method to dispose of all resources allocated by this
 * application. As a rule of thumb, everything created in onStartup() should
 * be deleted here
 */
void MahsJongApp::onShutdown() {
    _loading.dispose();
    _gameplay.dispose();
    _hostgame.dispose();
    _joingame.dispose();
    _assets = nullptr;
    _batch = nullptr;
    
    Input::deactivate<Keyboard>();
    Input::deactivate<Mouse>();
    Input::deactivate<TextInput>();
    netcode::NetworkLayer::stop();
    
    
    AudioEngine::stop();
    Application::onShutdown(); // YOU MUST END with call to parent 
}

/**
 * The method called to update the application data.
 *
 * This is your core loop and should be replaced with your custom implementation.
 * This method should contain any code that is not an OpenGL call.
 *
 * When overriding this method, you do not need to call the parent method
 * at all. The default implmentation does nothing.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void MahsJongApp::update(float timestep) {
    if(!_loaded && _loading.isActive()) {
        _loading.update(0.01f);
    } else if (!_loaded) {
        _loading.dispose(); // Disables the input listeners in this mode
        _assets->loadDirectory("json/assets.json");
        
        _matchScene = std::make_shared<scene2::Scene2>();
        if (!_matchScene->init()) {
            std::cerr << "Scene2 initialization failed!" << std::endl;
            // Handle failure (return, break, or handle error state)
        }
        std::shared_ptr<scene2::SceneNode> node = _assets->get<scene2::SceneNode>("matchscene");
        node->setVisible(true);
        auto texture = _assets->get<Texture>("background_gameplay");
        if (texture == nullptr) {
            CULog("Failed to load background texture!");
        } else {
            CULog("Background texture loaded successfully.");
        }
        std::shared_ptr<scene2::SceneNode> childNode = node->getChild(0);
        if (childNode) {
            // Now you can manipulate or render the child node
            CULog("Child node position: (%f, %f)", childNode->getPosition().x, childNode->getPosition().y);
        }
        node->doLayout();
        _matchScene->addChild(node);
        CULog("Scenenode position: (%f, %f)", node->getPosition().x, node->getPosition().y);
        _matchScene->setSpriteBatch(_batch);
        _matchScene->setActive(true);
        
        // All buttons need to be buttons not nodes
        auto discardNode = node->getChildByName("discard");
        _loaded = true;
    } else {
//        _gameplay.update(timestep);
        return;
    }
}

/**
* The method called to draw the application to the screen.
*
* This is your core loop and should be replaced with your custom implementation.
* This method should OpenGL and related drawing calls.
*
* When overriding this method, you do not need to call the parent method
* at all. The default implmentation does nothing.
*/
void MahsJongApp::draw() {
   switch (_scene) {
       case LOAD:
           _loading.render();
           break;
       case MENU:
           _mainmenu.render();
           break;
       case HOST:
           _hostgame.render();
           break;
       case CLIENT:
           _joingame.render();
           break;
       case GAME:
           _gameplay.render();
           break;
   }
}

/**
* Inidividualized update method for the loading scene.
*
* This method keeps the primary {@link #update} from being a mess of switch
* statements. It also handles the transition logic from the loading scene.
*
* @param timestep  The amount of time (in seconds) since the last frame
*/
void MahsJongApp::updateLoadingScene(float timestep) {
   if (_loading.isActive()) {
       _loading.update(timestep);
   } else {
       _loading.dispose(); // Permanently disables the input listeners in this mode
       _mainmenu.init(_assets);
       _mainmenu.setSpriteBatch(_batch);
       _hostgame.init(_assets);
       _hostgame.setSpriteBatch(_batch);
       _joingame.init(_assets);
       _joingame.setSpriteBatch(_batch);
       _gameplay.init(_assets);
       _gameplay.setSpriteBatch(_batch);

       _mainmenu.setActive(true);
       _scene = State::MENU;
   }
}


/**
* Inidividualized update method for the menu scene.
*
* This method keeps the primary {@link #update} from being a mess of switch
* statements. It also handles the transition logic from the menu scene.
*
* @param timestep  The amount of time (in seconds) since the last frame
*/
void MahsJongApp::updateMenuScene(float timestep) {
   _mainmenu.update(timestep);
   switch (_mainmenu.getChoice()) {
       case MenuScene::Choice::HOST:
           _mainmenu.setActive(false);
           _hostgame.setActive(true);
           _scene = State::HOST;
           break;
       case MenuScene::Choice::JOIN:
           _mainmenu.setActive(false);
           _joingame.setActive(true);
           _scene = State::CLIENT;
           break;
       case MenuScene::Choice::NONE:
           // DO NOTHING
           break;
   }
}

/**
* Inidividualized update method for the host scene.
*
* This method keeps the primary {@link #update} from being a mess of switch
* statements. It also handles the transition logic from the host scene.
*
* @param timestep  The amount of time (in seconds) since the last frame
*/
void MahsJongApp::updateHostScene(float timestep) {
   _hostgame.update(timestep);
   switch (_hostgame.getStatus()) {
       case HostScene::Status::ABORT:
           _hostgame.setActive(false);
           _mainmenu.setActive(true);
           _scene = State::MENU;
           break;
       case HostScene::Status::START:
           _hostgame.setActive(false);
           _gameplay.setActive(true);
           _scene = State::GAME;
           // Transfer connection ownership
           _gameplay.setConnection(_hostgame.getConnection());
           _hostgame.disconnect();
           _gameplay.setHost(true);
           break;
       case HostScene::Status::WAIT:
       case HostScene::Status::IDLE:
           // DO NOTHING
           break;
   }
}

/**
* Inidividualized update method for the client scene.
*
* This method keeps the primary {@link #update} from being a mess of switch
* statements. It also handles the transition logic from the client scene.
*
* @param timestep  The amount of time (in seconds) since the last frame
*/
void MahsJongApp::updateClientScene(float timestep) {
   _joingame.update(timestep);
   switch (_joingame.getStatus()) {
       case ClientScene::Status::ABORT:
           _joingame.setActive(false);
           _mainmenu.setActive(true);
           _scene = State::MENU;
           break;
       case ClientScene::Status::START:
           _joingame.setActive(false);
           _gameplay.setActive(true);
           _scene = State::GAME;
           // Transfer connection ownership
           _gameplay.setConnection(_joingame.getConnection());
           _joingame.disconnect();
           _gameplay.setHost(false);
           break;
       case ClientScene::Status::WAIT:
       case ClientScene::Status::IDLE:
       case ClientScene::Status::JOIN:
           // DO NOTHING
           break;
   }
}

/**
* Inidividualized update method for the game scene.
*
* This method keeps the primary {@link #update} from being a mess of switch
* statements. It also handles the transition logic from the game scene.
*
* @param timestep  The amount of time (in seconds) since the last frame
*/
void MahsJongApp::updateGameScene(float timestep) {
   _gameplay.update(timestep);
   if (_gameplay.didQuit()) {
       _gameplay.setActive(false);
       _mainmenu.setActive(true);
       _gameplay.disconnect();
       _scene = State::MENU;
   }
}


