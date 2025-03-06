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
    /** One dimensional vector representing the top layer of the discard pile */
    std::vector<std::shared_ptr<TileSet::Tile>> _topTiles;
    /** A reference to the player whose turn it is currently */
    std::shared_ptr<Player> _player;
    
public:

#pragma mark -
#pragma mark Constructors
    
    /**
     * Initializes a discard pile with default values
     */
    bool init();

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
    
    /** Method to return the top tiles of the discard pile
     *
     * @return a vector of tiles containing the top tiles of the discard pile
     */
    std::vector<std::shared_ptr<TileSet::Tile>> getTopTiles(){
        return _topTiles;
    }
    
    /**
     * Method to add tiles to the discard pile. Moves top tiles to lower layer and adds new tiles
     *
     * @param tiles     a vector of tiles to add to the discard pile
     * @return true if tiles successfully added to top layer of the pile, and false otherwise
     */
    bool addTiles(std::vector<std::shared_ptr<TileSet::Tile>> tiles){
        if (tiles.size() < 1){
            return false;
        }
        for (auto& tile : getTopTiles()){
            _discardPile.push_back(tile);
        }
        _topTiles = tiles;
        return true;
    }
};

#endif /*__MJ_DISCARD_PILE_H__*/
