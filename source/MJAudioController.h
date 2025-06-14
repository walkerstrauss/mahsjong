//
//  MJAudioController.h
//  Mahsjong
//
//  Created by Walker Strauss on 3/31/25.
//

#ifndef __MJ_AUDIO_CONTROLLER_H__
#define __MJ_AUDIO_CONTROLLER_H__

#include <cugl/cugl.h>
#include <unordered_map>

using namespace cugl;
using namespace cugl::audio;

class AudioController {
private:
    static AudioController* _instance;
    /** Reference to the asset manager */
    std::shared_ptr<cugl::AssetManager> _assets;
    /** Map to hold all sounds for the game */
    std::unordered_map<std::string, std::shared_ptr<Sound>> _sounds;

    //std::unordered_map<std::string, std::shared_ptr<Music>> _music;
    /** ID for background music */
    int _bgMusicID;
    /** Vector of keys for sounds in the game (for dynamic loading) */
    std::vector<std::string> _keys;
    
    std::shared_ptr<cugl::audio::AudioQueue>  _musicQueue;

    /** The handle returned by the queue when we first play the bgm */
    long _musicHandle = -1;

    bool _bgmPlaying = false;

    
public:
    /** Whether the game sound is currently on */
    bool soundOn;
#pragma mark -
#pragma mark Constructors
    AudioController() : _assets(nullptr), _bgMusicID(-1) {}
    
    static AudioController& getInstance(){
        if (!_instance){
            _instance = new AudioController();
        }
        return *_instance;
    }
    
    /**
     * Initializes the audio controller with an asset manager
     *
     * @param assets    The asset manager to get game sounds from
     * @return true if initialization was successful, and false otherwise
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets);
    
#pragma mark -
#pragma mark Gameplay Handling
    
    bool isMusicPlaying() const { return _bgmPlaying; }
    /**
     * Plays a sound effect specified by name
     *
     * @param key   The key for the sound in the controller's map of sounds
     * @param loop  Whether or not to loop the sound
     */
    void playSound(const std::string& key, bool loop = false);
    
    /** Plays a music track specified by name
     *
     * @param key   The key for the music track in the controller's map of music
     * @param loop  Whether or not to loop the music
     */
    void playMusic(const std::string& key, bool loop = true);
    
    /**
     * Stops the music currently being played
     */
    void stopMusic();

    
    /**
     * Adjusts volume
     *
     * @param key   the key of the sound effect to set the volume of
     * @param volume    the volume to set the audio to
     */
    void setSoundVolume(const std::string& key, float value);
    
    void playSelect(){
        if (soundOn){
            playSound("select",false);
        }
    }
    
    void toggleSound() {
        soundOn = !soundOn;
    }
};

#endif
