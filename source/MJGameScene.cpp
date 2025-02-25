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
    

    _tileSet->setAllTileTexture(assets);
    _pile = std::make_shared<Pile>(); //Init our pile
    _pile->initPile(5, _tileSet);

    _input.init(); //Init the input controller
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
    _player->getHand().updateTilePositions();
    pairs(timestep);
}

/*
* Used in update, determines when Player selects pairs
*/

void GameScene::pairs(float dt) {
    _input.update();

    if (_input.didRelease() && !_input.isDown()) { //Did we click?
        cugl::Vec2 prev = _input.getPosition(); //Get our mouse posistion
        cugl::Vec2 mousePos = cugl::Scene::screenToWorldCoords(cugl::Vec3(prev));

        for (int i = 0; i < _pile->getPileSize(); i++) {//Loop through our pile
            for (int j = 0; j < _pile->getPileSize(); j++) {

                if (_pile->_pile[i][j] == nullptr) { //If no longer in pile
                    continue;
                }
                TileSet::Tile _tile = *_pile->_pile[i][j]; //Collect tile

                cugl::Size _size = _tile.getTileTexture()->getSize(); //Get tile posistion on pile UPDATE IF WE CHANGE HOW IT IS DRAWN
                float scale = _tile._scale;
                float x = j * (_size.width * scale + 1.0f) + (_size.width * scale / 2);
                float y = i * (_size.height * scale + 1.0f) + (_size.height * scale / 2);
                cugl::Vec2 pos(x, y);
                float halfWidth = (_size.width * scale) / 2;
                float halfHeight = (_size.height * scale) / 2;

                if (mousePos.x >= pos.x - halfWidth && mousePos.x <= pos.x + halfWidth && mousePos.y >= pos.y - halfHeight && mousePos.y <= pos.y + halfHeight) { //If mouse clicked tile
                    
                    int index = 0;
                    for (const auto& it : _pile->_pairs) { //Checks whether the tile we selected is already selected. if it is deselect
                        if (it.x == i && it.y == j) {
                            _tile._scale = 0.2;
                            _pile->_pairs.erase(_pile->_pairs.begin() + index);

                            return; //If it is already in the pairs, remove it from pairs
                        }
                        index += 1;
                    }

                    if (_pile->_pairs.size() >= 2) { //Do we have a pair selected?
                        _pile->pairTile();
                        _pile->_pairs.clear();
                    }
                    else { //Add path to tile from pile
                        cugl::Vec2 pilePos(i, j);
                        _pile->_pairs.push_back(pilePos);
                        _tile._scale = 0.3;
                    }
                    return;
                }
            }
        }

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
    _batch->draw(temp, Color4("white"), Rect(Vec2::ZERO,getSize()));
//    _player->getHand().draw(_batch);
    
//    _tileSet->draw(_batch, getSize());
    _batch->draw(temp, Color4("white"), Rect(Vec2::ZERO, getSize()));
    _tileSet->draw(_batch, getSize());

    if (_pile->getVisibleSize() == 0 && _tileSet->deck.size() != 14) { //Only update pile if we still have tiles from deck
        _pile->createPile();
    }

    _batch->end();
}






