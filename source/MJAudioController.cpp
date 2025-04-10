//
//  MJAudioController.cpp
//  Mahsjong
//
//  Created by Walker Strauss on 3/31/25.
//

#include <cugl/cugl.h>
#include <iostream>
#include <sstream>
#include "MJAudioController.h"

using namespace cugl;
using namespace cugl::audio;

#pragma mark -
#pragma mark Constructors

bool AudioController::init(const std::shared_ptr<cugl::AssetManager>& assets){
    if (!assets){
        return false;
    }
    
    _assets = assets;
    
    _keys = {
        "set",
        "set1",
        "win",
        "winning-hand",
        "match-background",
        "match-background1",
        "back",
        "confirm",
        "deselect",
        "select",
        "select2",
        "shuffle",
        "swap"
    };
    
    for (const std::string& key  : _keys){
        auto sound = _assets->get<Sound>(key);
        if (sound) {
            _sounds[key] = sound;
        } else {
            CULog("No sound with key: %s", key.c_str());
        }
    }
    
    return true;
}

#pragma mark -
#pragma mark Gameplay Handling

void AudioController::playSound(const std::string& key, bool loop){
    if (_sounds.find(key) != _sounds.end()){
        AudioEngine::get()->play(key, _sounds[key], loop, 1.0f);
    } else {
        CULog("No sound with key: %s", key.c_str());
    }
}

void AudioController::playMusic(const std::string& key, bool loop){
    if (_sounds.find(key) != _sounds.end()){
        if (_bgMusicID != -1){
            AudioEngine::get()->stop();
        }
        
        _bgMusicID = AudioEngine::get()->play(key, _sounds[key], loop, 1.0f);
    } else {
        CULog("No music with key: %s", key.c_str());
    }
}

void AudioController::stopMusic() {
    if (_bgMusicID != -1){
        AudioEngine::get()->stop();
        _bgMusicID = -1;
    }
}

void AudioController::setSoundVolume(const std::string& key, float value){
    AudioEngine::get()->setVolume(key, value);
}
