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
    
    /**
     * Currently this is the default loading screen taken from the labs. The json limits us in the
     * design of a loading screen; hence, we may need to make a new file for loading.
     * 
     * TODO: Implement new loading screen either in new file or json
     */
    // Needed for loading screen
    _assets->attach<scene2::SceneNode>(Scene2Loader::alloc()->getHook());
    _assets->loadDirectory("json/loading.json");
    _loading.setSpriteBatch(_batch);
    
    // Create a "loading" screen
    _loaded = false;
        
    /**
     * Assets json is currently empty because we have no assets yet!
     * TODO: Add assets to json as seen necessary. You must also add the file to the textures folder
     */
    _loading.init(_assets, "json/assets.json");
     _loading.start();
    
    AudioEngine::start();
    Application::onStartup(); //YOU MUST END with call to parent
};


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

void MahsJongApp::update(float timestep) {
    if(!_loaded && _loading.isActive()) {
        _loading.update(0.01f);
    } else if (!_loaded) {
        _loading.dispose(); // Disables the input listeners in this mode
        _assets->loadDirectory("json/assets.json");
        _gameplay.setSpriteBatch(_batch);
        _gameplay.init(_assets);
        
        _loaded = true;
    } else {
        _gameplay.update(timestep);
    }
}

void MahsJongApp::draw(){
    if (!_loaded){
        _loading.render();
    } else {
        _gameplay.render();
    }
}

