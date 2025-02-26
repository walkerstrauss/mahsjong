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
    _player->getHand().updateTilePositions();
    
    //If there was a click we check if it was on a tile in the pile
    if(_input.didRelease() && !_input.isDown()){
        cugl::Vec2 prev = _input.getPosition(); //Get our mouse posistion
        cugl::Vec2 mousePos = cugl::Scene::screenToWorldCoords(cugl::Vec3(prev));
        _player->getHand().clickedTile(mousePos);
        _pile->pairs(mousePos);
    }
    if(_input.getKeyPressed() == KeyCode::P && _input.getKeyDown()){
        _player->getHand().playSet();
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
            _player->getHand().makeSet();
        }
        
        _text->setText(strtool::format("Score: %d", _player->_totalScore));
        _text->layout();
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
//    _player->getHand().draw(_batch);
//    _tileSet->draw(_batch, getSize());
    _tileSet->draw(_batch, getSize());

    _batch->setColor(Color4::GREEN);
    _batch->drawText(_text,Vec2(getSize().width - _text->getBounds().size.width - 10,
                                getSize().height-_text->getBounds().size.height));
    
    if (_pile->getVisibleSize() == 0 && _tileSet->deck.size() != 14) { //Only update pile if we still have tiles from deck
        _pile->createPile();
    }

    _batch->end();
}
