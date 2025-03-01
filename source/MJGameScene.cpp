//
//  MJGameScene.cpp
//  Mahsjong
//
//  Created by Patrick Choo on 2/18/25.
//
#include <cugl/cugl.h>
#include <iostream>
#include <sstream>

#include "MJGameScene.h"

using namespace cugl;
using namespace cugl::graphics;
using namespace cugl::audio;
using namespace std;

#pragma mark -
#pragma mark Level Layout

// Lock the screen esize to a fixed heigh regardless of aspect ratio
// PLEASE ADJUST AS SEEN FIT
#define SCENE_HEIGHT 720


#pragma mark -
#pragma mark Constructors
/**
 * Initializes the controller contents, and starts the game
 */

bool GameScene::init(const std::shared_ptr<cugl::AssetManager>& assets) {
    // Initialize the scene to a locked height
    if (assets == nullptr) {
        return false;
    }
    else if (!Scene2::initWithHint(Size(0, SCENE_HEIGHT))) {
        return false;
    }

    // Game Win and Lose bool
    _gameWin = false;
    _gameLose = false;

    // Start up the input handler
    _assets = assets;

    /**
    * Setting up the background image and constant values
    * TODO: Please edit scene background and constants as seen fit
    */

    /**
    * Setting up objects and textures
    * TODO: Please intialize objects and their textures
    */
    _tileSet = std::make_shared<TileSet>();

//    _tileSet->setTexture(assets->get<Texture>("tile"));
    
    
    _player = std::make_shared<Player>();
    _player->getHand().init(_tileSet);
    _player->getHand().updateTilePositions();
    
    std::string msg = strtool::format("Score: %d", _player->_totalScore);
    _text = TextLayout::allocWithText(msg, assets->get<Font>("pixel32"));
    _text->layout();
    _win = TextLayout::allocWithText("You Won!\nReset with R", assets->get<Font>("pixel32"));
    _win->layout();
    _lose = TextLayout::allocWithText("You Lost!\nReset with R", assets->get<Font>("pixel32"));
    _lose->layout();
    _tileSet->setAllTileTexture(assets);
    _pile = std::make_shared<Pile>(); //Init our pile
    _pile->initPile(5, _tileSet);

    _input.init(); //Init the input controller
    
    // Initialize grandma tile label
    _gmaLabelTexture = assets->get<Texture>("grandma text");
    if (!_gmaLabelTexture){
        CULog("missing gma text");
        return false;
    } else {
        _tileSet->gmaTexture = _gmaLabelTexture;
    }
    
    return true;
}

void GameScene::dispose() {
    if (_active) {
        removeAllChildren();
        _active = false;
    }
}

#pragma mark -
#pragma mark Gameplay Handling
/**
 * Resets the status of the game so that we can play again
 * TODO: Please dispose or reset all of the objects you implement in this method when game is over
 */
void GameScene::reset() {
    _gameLose = false;
    _gameWin = false;
    init(_assets);
    return;
}

/**
 * The method to update the game mode
 *
 * @param timestep The amount of time (in seconds) since the last frame
 *
 * TODO: Please implement any gameplay associated with updates
 */
void GameScene::update(float timestep) {
    //Reading input
    _input.readInput();
    _input.update();
    if (_input.getKeyPressed() == KeyCode::R && _input.getKeyDown()) {
        reset();
    }

    //Win or lose?
    if (_gameLose || _gameWin) {
        return;
    }

    _player->getHand().updateTilePositions();
    
    //If there was a click we check if it was on a tile in the pile
    if((_input.didRelease() && !_input.isDown())){
        cugl::Vec2 prev = _input.getPosition(); //Get our mouse posistion
        cugl::Vec2 mousePos = cugl::Scene::screenToWorldCoords(cugl::Vec3(prev));
        _player->getHand().clickedTile(mousePos);
        _pile->pairs(mousePos, _player);
    }
    if(_pile->_pairs.size() == 2 && _player->getHand()._selectedTiles.size() == 2){
//        CULog("here");
        cugl::Vec2 prev = _input.getPosition(); //Get our mouse posistion
        cugl::Vec2 mousePos = cugl::Scene::screenToWorldCoords(cugl::Vec3(prev));
        _pile->pairs(mousePos, _player);
    }
    if(_input.getKeyPressed() == KeyCode::P && _input.getKeyDown()){
        _player->getHand().playSet(_tileSet);
        if(_player->getHand()._tiles.size() < 14){
            while(_player->getHand().grandmaToAdd > 0){
                std::shared_ptr<TileSet::Tile> wildTile = _tileSet->generateWildTile();
                wildTile->setWildTexture(_assets);
                wildTile->inHand = true;
                wildTile->_scale = 0.2;
                
                _tileSet->deck.push_back(wildTile);
                
                _player->getHand()._tiles.push_back(wildTile);
                _player->getHand().grandmaToAdd -= 1;
            }
            _player->getHand().drawFromPile(_pile);
        }
        
    } else if (_input.getKeyPressed() == KeyCode::D && _input.getKeyDown()){
        if(_player->getHand()._selectedTiles.size() >= 1 && _player->getHand()._selectedTiles.size() <= 4){
            if (!_player->discarding){
                _player->discarding = true;
                for (auto& tile : _player->getHand()._selectedTiles){
                    _player->getHand().discard(tile);
                    tile->selected = false;
                    tile->inHand = false;
                    tile->inPile = false;
                }
                _player->getHand()._selectedTiles.clear();
                _player->getHand().drawFromPile(_pile);
                _player->discarding = false;
            }
        }
        else{
            for (auto& tile : _player->getHand()._selectedTiles){
                tile->selected = false;
            }
            _player->getHand()._selectedTiles.clear();
        }
    } else if (_input.getKeyPressed() == KeyCode::S && _input.getKeyDown()){
            if(!_player->getHand().makeSet()){
                for(const auto& it : _player->getHand()._selectedTiles){
                    it->selected = false;
                }
                _player->getHand()._selectedTiles.clear();
                };
    }
        _text->setText(strtool::format("Score: %d", _player->_totalScore));
        _text->layout();

        //Win or Lose
        if (_player->_totalScore >= 20) {
            _gameWin = true;
        }
        else if (_player->_turnsLeft <= 0 && _gameWin != true) {
            _gameLose = true;
        }

    }

/**
 * Draws all this to the scene's SpriteBatch.
 */

void GameScene::render() {
    /**
    * This is the temporary view of our camera. Not supposed to do this.
    * TODO: Please edit camera view appropriately
    */
    _batch->begin(getCamera()->getCombined());

    /**
    * This is just a temporary texture to test we are in our game
    * TODO: Please fix with appropriate background texture
    */
    const std::shared_ptr<Texture> temp = Texture::getBlank();
    _batch->draw(temp, Color4(141,235,207,100), Rect(Vec2::ZERO,getSize()));
    _batch->draw(temp,Color4(37,41,88,255),Rect(Vec2(0.0f,620),Vec2(getSize())));
//    _player->getHand().draw(_batch);
//    _tileSet->draw(_batch, getSize());
    _tileSet->draw(_batch, getSize());

    _batch->setColor(Color4::GREEN);
    _batch->drawText(_text,Vec2(getSize().width - _text->getBounds().size.width - 10,
                                getSize().height-_text->getBounds().size.height));
    
    if (_pile->getVisibleSize() == 0 && _tileSet->deck.size() != 14) { //Only update pile if we still have tiles from deck
        _pile->createPile();
    }

    if (_gameWin) {
        _batch->setColor(Color4::BLUE);
        Affine2 trans;
        trans.scale(4);
        trans.translate(getSize().width / 2 - 2 * _win->getBounds().size.width, getSize().height / 2 - _win->getBounds().size.height);
        _batch->drawText(_win, trans);
    }
    else if (_gameLose) {
        _batch->setColor(Color4::RED);
        //_batch->drawText(_lose, Vec2((getSize().width) / 2 - _lose->getBounds().size.height, getSize().height / 2));
        Affine2 trans;
        trans.scale(4);
        trans.translate(getSize().width / 2 - 2 * _lose->getBounds().size.width, getSize().height / 2 - _lose->getBounds().size.height);
        _batch->drawText(_lose, trans);

    }


    _batch->end();
}
