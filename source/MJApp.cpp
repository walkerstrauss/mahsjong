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
    Input::activate<Keyboard>();
    
    _assets->attach<Texture>(TextureLoader::alloc()->getHook());
    _assets->attach<Sound>(SoundLoader::alloc()->getHook());
    _assets->attach<Font>(FontLoader::alloc()->getHook());
    _assets->attach<JsonValue>(JsonLoader::alloc()->getHook());
    _assets->attach<WidgetValue>(WidgetLoader::alloc()->getHook());
    
    // Needed for loading screen
    _sceneLoader = Scene2Loader::alloc();
    _assets->attach<scene2::SceneNode>(_sceneLoader->getHook());
    _assets->loadDirectory("json/loading.json");
    _loading.setSpriteBatch(_batch);
    
    // Create a "loading" screen
    _loaded = false;
    _loading.init(_assets, "json/assets.json");
     _loading.start();
    
    AudioEngine::start();
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
    _assets = nullptr;
    _batch = nullptr;
    
    Input::deactivate<Keyboard>();
    Input::deactivate<Mouse>();
    
    AudioEngine::stop();
    Application::onShutdown(); // YOU MUST END with call to parent 
}

/** Method to update the application data */
void MahsJongApp::update(float timestep) {
    if(!_loaded && _loading.isActive()) {
        _loading.update(0.01f);
    } else if (!_loaded) {
        _loading.dispose(); // Disables the input listeners in this mode
        _assets->attach<Button>(Scene2Loader::alloc()->getHook());
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
        
        _discardBtn = std::dynamic_pointer_cast<Button>(childNode->getChildByName("discard"));
        CULog(_discardBtn->getName().c_str());
        if (_discardBtn){
            _discardBtn->addListener([this](const std::string& name, bool down){
                CULog("pressed");
                if (!down){
                    CULog("discard");
                    // Handle discard
                }
            });
        } else {
            CULog("no discard");
        }
        _discardBtn->activate();
        _loaded = true;
    } else {
//        _gameplay.update(timestep);
        return;
    }
}

/** The method called to draw the appplication to the screen */
void MahsJongApp::draw(){
    if (!_loaded){
        _loading.render();
    } else {
        _matchScene->render();
    }
}

