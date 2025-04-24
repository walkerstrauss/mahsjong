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
    
    // get the actual size of the screen.
    cugl::Size screenSize = cugl::Application::get()->getDisplaySize();
    
    //auto cam = OrthographicCamera::alloc(getDisplaySize());
    auto cam = OrthographicCamera::alloc(screenSize);
    
    
#ifdef CU_TOUCH_SCREEN
    Input::activate<Touchscreen>();
#else
    Input::activate<Mouse>();
    Input::get<Mouse>()->setPointerAwareness(Mouse::PointerAwareness::DRAG);

#endif
    Input::activate<Keyboard>();
    Input::activate<TextInput>();
    
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
    
    // Get rid of wrong start button
    std::shared_ptr<scene2::Button> wrongStart = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("load.after.landingscene.button1"));
    wrongStart->setVisible(false);
    
    _loading.start();
    
    _inputController = std::make_shared<InputController>();
    _inputController->init();
    
    AudioEngine::start();
    netcode::NetworkLayer::start(netcode::NetworkLayer::Log::INFO);
//    _network = NetworkController();
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
    _settings.dispose();
    _pause.dispose();
    _gameover.dispose();
    _info.dispose();
    _inputController->dispose();
//    _network->dispose();
    _assets = nullptr;
    _batch = nullptr;
    
#ifdef CU_MOBILE
    Input::deactivate<Touchscreen>();
#else
    Input::deactivate<Mouse>();
#endif
    Input::deactivate<Keyboard>();
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
    if(_inputController) {
        _inputController->readInput();
        _inputController->update();
    }
    if (_network) {
        _network->update(timestep);
    }
    switch (_scene) {
        case LOAD:
            updateLoadingScene(timestep);
            break;
        case MENU:
            _assets->loadDirectory("json/assets.json");
            updateMenuScene(timestep);
            break;
        case HOST:
//            _assets->loadDirectory("json/constants.json");
            updateHostScene(timestep);
            break;
        case CLIENT:
            updateClientScene(timestep);
            break;
        case GAME:
            updateGameScene(timestep);
            break;
        case SETTINGS:
            updateSettingScene(timestep);
            break;
        case PAUSE:
            updatePauseScene(timestep);
            break;
        case OVER:
            updateGameOverScene(timestep);
            break;
        case INFO:
            updateInfoScene(timestep);
            break;
        case TUTORIAL:
            updateTutorialScene(timestep);
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
           _loading.renderAfter();
           break;
       case MENU:
           _mainmenu.render();
           break;
       case HOST:
           _hostgame.makeBlackBackground();
           _hostgame.render();
           break;
       case CLIENT:
           _joingame.makeBlackBackground();
           _joingame.render();
           break;
       case GAME:
           _gameplay.render();
           break;
       case SETTINGS:
           if (_settings.scene == SettingScene::PrevScene::PAUSED){
               _gameplay.render();
           } else if (_settings.scene == SettingScene::PrevScene::MAIN){
               _mainmenu.render();
           }
           _settings.render();
           break;
       case PAUSE:
           _gameplay.render();
           _pause.render();
           break;
       case OVER:
           _gameover.render(_batch);
           break;
       case INFO:
           _info.render();
           break;
       case TUTORIAL:
           _tutorial.render();
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
       _loading.resizeScene();
   } else {
       _loading.dispose(); // Permanently disables the input listeners in this mode
       
       
       _network = std::make_shared<NetworkController>();
       _network->init(_assets);
       AnimationController::getInstance().init(_assets);
       AudioController::getInstance().init(_assets);
       _mainmenu.init(_assets);
       _mainmenu.setSpriteBatch(_batch);
       _hostgame.init(_assets, _network);
       _hostgame.setSpriteBatch(_batch);
       _joingame.init(_assets, _network);
       _joingame.setSpriteBatch(_batch);
       _settings.init(_assets);
       _settings.setSpriteBatch(_batch);
       _pause.init(_assets);
       _pause.setSpriteBatch(_batch);
       _gameover.init(_assets);
       _gameover.setSpriteBatch(_batch);
       _mainmenu.setActive(true);
       _info.init(_assets);
       _info.setSpriteBatch(_batch);
       _tutorial.init(_assets, _inputController);
       _tutorial.setSpriteBatch(_batch);
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
       case MenuScene::Choice::SETTING:
           _mainmenu.setActive(false);
           _settings.setActive(true);
           _settings.scene = SettingScene::PrevScene::MAIN;
           _scene = State::SETTINGS;
           break;
       case MenuScene::Choice::TUTORIAL:
           _mainmenu.setActive(false);
           _tutorial.setActive(true);
           _scene = TUTORIAL;
           break;
       case MenuScene::Choice::NONE:
           // DO NOTHING
           break;
   }
}

/**
 * Individualized update method for the host scene.
 *
 * This method keeps the primary {@link #update} from being a mess of switch
 * statements. It also handles the transition logic from the host scene.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void MahsJongApp::updateHostScene(float timestep) {
    _hostgame.update(timestep);
    if(_hostgame.getBackClicked()){
        _scene = MENU;
        _hostgame.setActive(false);
        _mainmenu.setActive(true);
    } else if (_network->getStatus() == NetworkController::Status::START) {
        _gameplay.init(_assets, _network, _inputController);
        _gameplay.setSpriteBatch(_batch);
        _hostgame.setActive(false);
        _gameplay.setActive(true);
        _gameplay.setGameActive(true);
        _scene = State::GAME;
    } else if (_network->getStatus() == NetworkController::Status::NETERROR) {
        _scene = MENU;
        _network->disconnect();
        _hostgame.setActive(false);
        _mainmenu.setActive(true);
        _gameplay.dispose();
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

    if(_joingame.getBackClicked()){
        _scene = MENU;
        _joingame.setActive(false);
        _mainmenu.setActive(true);
    } else if (_network->getStatus() == NetworkController::Status::INGAME) {
        _gameplay.init(_assets, _network, _inputController);
        _gameplay.setSpriteBatch(_batch);
        _joingame.setActive(false);
        _gameplay.setActive(true);
        _gameplay.setGameActive(true);
        _scene = GAME;
    }
    else if (_network->getStatus() == NetworkController::Status::NETERROR) {
        _network->disconnect();
        _joingame.setActive(false);
        _mainmenu.setActive(true);
        _gameplay.dispose();
        _scene = MENU;
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
        return;
    }
    switch (_gameplay.getChoice()){
        case GameScene::Choice::PAUSE:
            _gameplay.setGameActive(false);
            _pause.setActive(true);
            _scene = State::PAUSE;
            break;
        case GameScene::Choice::SETS:
            // Add logic for transitioning to sets scene
            break;
        case GameScene::Choice::WIN:
            _gameplay.setGameActive(false);
            _gameover.type = GameOverScene::Type::WIN;
            _gameover.setActive(true);
            _scene = State::OVER;
            break;
        case GameScene::Choice::LOSE:
            _gameplay.setGameActive(false);
            _gameover.type = GameOverScene::Type::LOSE;
            _gameover.setActive(true);
            _scene = State::OVER;
            break;
        case GameScene::Choice::DISCARDED:
            if (_gameplay.discardedTiles.empty()){
                CULog("Discarded tiles is empty");
                break;
            }
            _gameplay.discardedTiles.clear();
            _gameplay._choice = GameScene::Choice::NONE;
            break;
            break;
        case GameScene::Choice::DRAW_DISCARD:
            _gameplay.discardDrawTile = nullptr;
            _gameplay._choice = GameScene::Choice::NONE;
            break;
        case GameScene::Choice::INFO:
            _gameplay.setGameActive(false);
            _info.setActive(true);
            _scene = INFO;
            break;
        case GameScene::Choice::SETTING:
            _gameplay.setGameActive(false);
            _settings.setActive(true);
            _settings.scene = SettingScene::PrevScene::PAUSED;
            _scene = State::SETTINGS;
        case GameScene::Choice::NONE:
            // Do nothing
            break;
    }
}

/**
 * Individualized update method for the setting scene
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void MahsJongApp::updateSettingScene(float timestep){
    _settings.update(timestep);
    switch (_settings.choice){
        case SettingScene::Choice::MENU:
            _settings.setActive(false);
            _mainmenu.setActive(true);
            _scene = State::MENU;
            break;
        case SettingScene::Choice::PAUSE:
            _settings.setActive(false);
            _gameplay.setGameActive(true);
            _scene = State::GAME;
            break;
        case SettingScene::Choice::MUSICON:
            // TODO: Handle turning game music on
            break;
        case SettingScene::Choice::MUSICOFF:
            // TODO: Handle turning game music off
            break;
        case SettingScene::Choice::SOUNDON:
            // TODO: Handle turning game sound on
            break;
        case SettingScene::Choice::SOUNDOFF:
            // TODO: Handling turning game sound off
            break;
        case SettingScene::Choice::NONE:
            // Do nothing
            break;
    }
}

/**
 * Individualized update method for the pause scene
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void MahsJongApp::updatePauseScene(float timestep) {
    _pause.update(timestep);
    switch (_pause.choice){
        case PauseScene::Choice::MENU:
            _pause.setActive(false);
            _mainmenu.setActive(true);
            _network->disconnect();
            _gameplay.dispose();
            _scene = State::MENU;
            break;
        case PauseScene::Choice::SETTINGS:
            _pause.setActive(false);
            _settings.setActive(true);
            _settings.scene = SettingScene::PrevScene::PAUSED;
            _scene = State::SETTINGS;
            break;
        case PauseScene::Choice::CONTINUE:
            _pause.setActive(false);
            _gameplay.setGameActive(true);
            _scene = State::GAME;
            break;
        case PauseScene::Choice::NONE:
            // Do nothing
            break;
    }
}

/**
 * Individualized update method for the game over scene
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void MahsJongApp::updateGameOverScene(float timestep) {
    _gameover.update(timestep);
    if (_gameover.choice == GameOverScene::Choice::MENU){
        _gameover.setActive(false);
        _mainmenu.setActive(true);
        _network->disconnect();
        _scene = State::MENU;
        _gameplay.dispose();
    } else if (_gameover.choice == GameOverScene::Choice::NONE){
        // Do nothing
        return;
    }
}

/**
* Individualzed update method for the info scene
*
* @param timestep   The amount of time (in seconds) since the last frame
*/
void MahsJongApp::updateInfoScene(float timestep){
    _info.update(timestep);
    if (_info.choice == InfoScene::BACK){
        _info.setActive(false);
        _info.choice = InfoScene::NONE;
        _gameplay.setGameActive(true);
        _scene = GAME;
    }
}

/**
* Individualzed update method for the tutorial scene
*
* @param timestep   The amount of time (in seconds) since the last frame
*/
void MahsJongApp::updateTutorialScene(float timestep){
    _tutorial.update(timestep);
    switch (_tutorial.getChoice()){
        case TutorialScene::NONE:
            break;
        case TutorialScene::BACK:
            _tutorial.setActive(false);
            _mainmenu.setActive(true);
            _scene = MENU;
            break;
    }
}

