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
public:
    enum Choice {
        NONE,
        SHUFFLE
    };
private:
    /** Number of possible tiles in pile */
    int _pileSize;
    
public:
    std::shared_ptr<AssetManager> _assets;
    /** Two dimensional vector representing pile tiles */
    std::vector<std::vector<std::shared_ptr<TileSet::Tile>>> _pile;
    /** Random Generator */
    cugl::Random rdPile;
    /** Map containing all tiles and their pile coordinates */
    std::map<std::string, cugl::Vec2> _pileMap;
    /** Vector representing tiles drawn from the pile */
    std::vector<std::shared_ptr<TileSet::Tile>> _draw;
    /** A reference the tileset in our game */
    std::shared_ptr<TileSet> _tileSet;
    /** Stores the location of our pair in the pile */
    std::vector<std::shared_ptr<TileSet::Tile>> _pairs;
    /** The rect of the pile */
    cugl::Rect pileBox;
    /** Time for pile jump effect */
    float time;
    /** Choice */
    Choice choice;
#pragma mark -
#pragma mark Constructors
    
    /**
     * Initializes a new layer of the pile with tiles drawn from the deck
     *
     * @param size             the size the pile should be
     * @param tileSet      the tileset to draw from to build the pile
     */
    bool initPile(int size, std::shared_ptr<TileSet> tileSet, bool isHost, std::shared_ptr<AssetManager>& _assets);
    
    void initPileTutorialMode();
    /**
     * Creates a new pile according to size. Initializes all pile elements to 0
     *
     * @return true if pile was created successfully, and false otherwise
     */
    bool createEmptyPile();
    
    /**
     * Creates a new pile and positions them for drawing to the screen
     *
     * @return true if the pile was created successfully, and false otherwise
     */
    bool createPile();
    
    /**
     * Method to update the positions of the tiles in pile
     */
    void setTilePositions(bool shuffling);
    
    /**
     * Updates the positions of each tile and their textures
     */
    void updateTilePositions(float dt);
    
    void animTilePositions(int frames = 1);
    
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
    
    /** Returns the flattened 1D representation of the 2D pile. */
    std::vector<std::shared_ptr<TileSet::Tile>> flattenedPile() {
        std::vector<std::shared_ptr<TileSet::Tile>> flattenedPile;
        for (auto& row : _pile) {
            for (auto& tile : row) {
                if (tile != nullptr) {
                    flattenedPile.push_back(tile);
                }
            }
        }
        return flattenedPile;
    }
        
    /**
     *  Returns the index of the row that the given tile is in.
     */
    int selectedRow(std::shared_ptr<TileSet::Tile> tile);
    
    /**
     * Remakes pile according to the player who drew the last tile in the pile
     */
    void remakePile(bool shuffling);
    
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
     * Updates a singular tile within the pile (receiver action)
     *
     * @param pileTile  the json representation of a tile
     */
    void removePileTile(const std::shared_ptr<cugl::JsonValue> tileJson, bool isHostDraw);
    
    /**
     * Removes a singular tile within the pile (sender action
     */
    void removeTile(std::shared_ptr<TileSet::Tile> tile);
        
    /**
     * Reshuffles the tiles within the pile
     */
    void reshufflePile();
    
    /**
     * Fills the pile with nullptrs
     */
    void clearPile(){
        for (int i = 0; i < _pileSize; ++i) {
            for (int j = 0; j < _pileSize; ++j) {
                _pile[i][j] = nullptr;
            }
        }
    }
    
    /**
     * Remove inputted number of tiles iteratively
     */
    void removeNumTiles(int nums);
    
    /** Updates the pile indexes of tiles in the given row to the new order. */
    void updateRow(int row, const std::vector<std::shared_ptr<TileSet::Tile>>& tiles, float dt);
    
    /** Pile jump effect */
    void pileJump(float dt);
    
    /** Pile flip and move to position*/
    void pileFlipMoveAway(std::unordered_map<std::shared_ptr<TileSet::Tile>, Vec2> tilePos);
    
    /** Pile flip effect  and move to center*/
    void pileFlipMoveCenter();

    
};
#endif /* __MJ_PILE_H__ */



