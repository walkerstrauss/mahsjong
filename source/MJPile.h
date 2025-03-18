//
//  MJPile.h
//  Mahsjong
//
//  Created by Patrick Choo on 2/18/25.
//

#ifndef __MJ_PILE_H__
#define __MJ_PILE_H__

#include <cugl/cugl.h>
#include "MJTileSet.h"
#include "MJInputController.h"

/**
 * This is the class intializing and handling the pile.
 */
class Player;
class Pile {
private:
	/** Number of possible tiles in pile */
	int _pileSize;
    
public:
	/** Two dimensional vector representing pile tiles */
	std::vector<std::vector<std::shared_ptr<TileSet::Tile>>> _pile;
    /** Map containing all tiles and their pile coordinates */
    std::map<std::string, cugl::Vec2> _pileMap;
    /** Vector representing tiles drawn from the pile */
	std::vector<std::shared_ptr<TileSet::Tile>> _draw;
    /** A reference the tileset in our game */
	std::shared_ptr<TileSet> _tileSet;
	/** Stores the location of our pair in the pile */
    std::vector<std::shared_ptr<TileSet::Tile>> _pairs;
    
#pragma mark -
#pragma mark Constructors
    
    /**
     * Initializes a new layer of the pile with tiles drawn from the deck
     *
     * @param size             the size the pile should be
     * @param tileSet      the tileset to draw from to build the pile
     */
    bool initPile(int size, std::shared_ptr<TileSet> tileSet);
    
    /**
     * Creates a new pile and positions them for drawing to the screen
     *
     * @return true if the pile was created successfully, and false otherwise
     */
    bool createPile();
    
#pragma mark -
#pragma mark Gameplay Handling
    /**
     * Method to get the size of the pile
     *
     * @return the size of the pile
     */
	int getPileSize() {
        return _pileSize;
    }
    
    /**
     * Method to get the size of the remaining top layer of the pile
     *
     * @return the size of the visible layer of tiles in the pile
     */
    int getVisibleSize() {
        if (_pile.empty()) {
            return 0;
        }

        int visible = 0;
        for (int i = 0; i < _pileSize; i++) {
            for (int j = 0; j < _pileSize; j++) {
                if (_pile[i][j] != nullptr) {
                    visible += 1;
                }
            }
        }
        return visible;
    }

    /**
     * Method to get the tiles drawn from the pile for the player
     *
     * @param number_of_tiles   the number of tiles to draw from the pile
     * @return a vector of tiles to add to the player hand
     */
	std::vector<std::shared_ptr<TileSet::Tile>> tilesDrawn(int number_of_tiles);

    /**
     * Method to handle pair making for the pile, including removing from pile and returning removed tiles
     *
     * @param player    the player whose hand the tiles in the pair are being drawn to
     * @return a vector of tiles in the pair
     */
	std::vector<std::shared_ptr<TileSet::Tile>> pairTile(const std::shared_ptr<Player>& player);
    
    /**
     * Remakes pile according to the player who drew the last tile in the pile
     */
    void remakePile();
    
	/**
     * Method to draw the tiles in the pile
     *
     * @param batch     the SpriteBatch to draw the pile tiles to
     * @param size       the size of the pile
     * @param position the position of the pile to draw
     */
	void draw(const std::shared_ptr<cugl::graphics::SpriteBatch>& batch);
    
    /**
     * Method to check if the player has selected two tiles that form a pair and handle pairs
     *
     * @param player    the player for the game
     */
    void pairs(const cugl::Vec2 mousePos, const std::shared_ptr<Player>& player);
    
    /**
     * Updates a singular tile within the pile
     *
     * @param pileTile  the json representation of a tile
     */
    void removePileTile(const std::shared_ptr<cugl::JsonValue> tileJson, bool isHostDraw);
};

#endif /* __MJ_PILE_H__ */



