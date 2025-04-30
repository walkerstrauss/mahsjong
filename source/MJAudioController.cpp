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

AudioController* AudioController::_instance = nullptr;

bool AudioController::init(const std::shared_ptr<cugl::AssetManager>& assets){
    if (!assets){
        return false;
    }
    
    _assets = assets;
    
    _keys = {
        "ExitDiscard",
        "SelectDiscard",
        "PlayerJoined",
        "WrongAction",
        "OpponentTile",
        "PlayedSet",
        "Pig",
        "Rat",
        "Monkey",
        "Dragon",
        "Snake",
        "Rooster",
        "Ox",
        "Rabbit",
        "Pile",
        "Confirm",
        "Clear",
        "Click",
        "Unclick",
        "Exit",
        "Select",
        "Done",
        "Discard",
        "DrawDiscard",
        "bgm",
        "menuMusic"
    };
    
    for (const std::string& key  : _keys){
        auto sound = _assets->get<Sound>(key);
        if (sound) {
            _sounds[key] = sound;
        } 
    }

    // init the queue with the background music.
    _musicQueue = AudioEngine::get()->getMusicQueue();

    soundOn = true;

    
    return true;
}

#pragma mark -
#pragma mark Gameplay Handling

void AudioController::playSound(const std::string& key, bool loop){
    std::string instance = key+std::to_string(0);
    int i = 0;
    if (_sounds.find(key) != _sounds.end()){
        if (soundOn){
            auto sound = _sounds[key];
            while (AudioEngine::get()->isActive(instance)) {
                i++;
                instance = instance + std::to_string(i);
            }
            AudioEngine::get()->play(instance, sound, loop, 1.0f);
        }
    }
}

//void AudioController::playMusic(const std::string& key, bool loop){
//    if (_sounds.find(key) != _sounds.end()){
//        if (_bgMusicID != -1){
//            AudioEngine::get()->stop();
//        }
//
//        _bgMusicID = AudioEngine::get()->play(key, _sounds[key], loop, 1.0f);
//    } else {
//        CULog("No music with key: %s", key.c_str());
//    }
//}

//void AudioController::stopMusic() {
//    if (_bgMusicID != -1){
//        AudioEngine::get()->stop();
//        _bgMusicID = -1;
//    }
//}


void AudioController::stopMusic() {
    // clear the queue
  _musicQueue->clear(0.0f);
}

void AudioController::playMusic(const std::string& key, bool loop) {
  auto it = _sounds.find(key);
  if (it == _sounds.end()) {
    CULog("No music with key: %s", key.c_str());
    return;
  }
  // Clear whatever is playing. Instant cut for now, but can cahnage it later.
  _musicQueue->clear(0.0f);
  // play next song.
  if (soundOn){
    _musicQueue->play(it->second, loop, 1.0f, 0.0f);
  }
}

void AudioController::pauseMusic() {
    if (_musicHandle >= 0) {
        _musicQueue->pause(_musicHandle);
    }
}

void AudioController::resumeMusic() {
    if (_musicHandle >= 0) {
        _musicQueue->resume();
    }
}



void AudioController::setSoundVolume(const std::string& key, float value){
    AudioEngine::get()->setVolume(key, value);
}
