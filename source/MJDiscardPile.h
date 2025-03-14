//
//  MJDiscardPile.h
//  Mahsjong
//
//  Created by Walker Strauss on 3/5/25.
//

#ifndef __MJ_DISCARD_PILE_H__
#define __MJ_DISCARD_PILE_H__

#include <cugl/cugl.h>
#include "MJTileSet.h"
#include "MJPlayer.h"
#include "MJInputController.h"

/**
 * This is the class initializing and handling the discard pile
 */
class DiscardPile {
protected:
    /** Number of tiles in the discard pile */
    int _size;
    /** One dimensional vector representing discard pile tiles (not including top tiles) */
    std::vector<std::shared_ptr<TileSet::Tile>> _discardPile;
    /** Map continaing tiles in discard pile */
    std::map<std::string, std::shared_ptr<TileSet::Tile>> _discardMap;
    /** One dimensional vector representing the top layer of the discard pile */
    std::shared_ptr<TileSet::Tile> _topTile;
    /** A reference to the player */
    std::shared_ptr<Player> _player;
    /** A reference to the asset manager */
    std::shared_ptr<cugl::AssetManager> _assets;
    /** Which top tile is selected (0 for none, 1 for first, 2 for second) */
    int _selectedTopTile;
public:

#pragma mark -
#pragma mark Constructors
    /**
     * Initializes a discard pile with default values
     */
    DiscardPile() : _size(0), _selectedTopTile(0) {}
    
    /**
     * Initializes the discard pile with asset manager
     *
     * @param assets    the asset manager for the game
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets);

#pragma mark -
#pragma mark Gameplay Handling
    
    /**
     * Method to get size of discard pile
     *
     * @return the size of the discard pile
     */
    int getSize() {
        return _size;
    }
    
    /**
     * Method to get the selected discard tile
     *  (0 for none, 1 for first, 2 for second)
     *
     * @return a number representing which top tile is selected
     */
    int getSelectedTopTile() {
        return _selectedTopTile;
    }
    
    /** Method to return the top tiles of the discard pile
     *
     * @return the tile on the top of the discard pile
     */
    std::shared_ptr<TileSet::Tile> getTopTile(){
        return _topTile;
    }
    
    /**
     * Method to check if player has selected the top tile of the discard pile
     */
    bool isTileSelected(const cugl::Vec2& mousePos);
    
    /**
     * Method to add tile to the discard pile. Moves top tile to lower layer and adds new tile
     *
     * @param tile     the tile to add to the discard (the new top tile)
     * @return true if tiles successfully added to top layer of the pile, and false otherwise
     */
    bool addTile(std::shared_ptr<TileSet::Tile> tile){
        if (getTopTile() != nullptr) {
            _discardPile.push_back(_topTile);
        }
        tile->pos = cugl::Vec2(990,520);
        _discardMap.insert({tile->toString() + " " + std::to_string(tile->_id), tile});
        _topTile = tile;
        _size = static_cast<int>(_discardPile.size()) + 1;
        return true;
    }
    
    /**
     * Method to take a tile from the discard pile.
     * Returns a tile for the player to add to hand and updates the dicard pile
     *
     * @return a tile to add to the player hand
     */
    std::shared_ptr<TileSet::Tile> drawTopTile();
    
    /**
     * Method to update the position of the discard pile tile
     */
    void updateTilePositions();
    
    /**
     * Method to update the discard pile model
     *
     * @param timestep The amount of time (in seconds) since the last frame
     */
    void update(float timestep);
    
    /**
     * Method to render the top card of the discard pile
     */
//    void render(const std::shared_ptr<cugl::graphics::SpriteBatch>& batch);
    
    /**
     * Removes top tile in discard pile
     */
    void removeTopTile(){ _topTile = nullptr; };

};

#endif /*__MJ_DISCARD_PILE_H__*/
