//
//  MJPlayer.h
//  Mahsjong
//
//  Created by Patrick Choo on 2/18/25.
//  Modified by Mariia Tiulchenko on 2/22/25.

#ifndef __MJ_MAIN_MENU_H__
#define __MJ_MAIN_MENU_H__

#include <cugl/cugl.h>

#include <vector>
#include <iostream>


#include "MJTileSet.h"

/**
 * This is the class intializing all player features and interactions
 *
 * Note: Since many attributes of this game fall under the same object, we combine
 * the player, scoring, and hand into one class. SUBJECT TO CHANGE, PLEASE
 * CHANGE TO WHATEVER IS MOST CONVENIENT FOR YOU
 *
 * TODO: Please implement all player interactions
 *
 */


class Hand{
private:
    
    // dynamic container of the hand tiles
    std::vector<std::shared_ptr<TileSet::Tile>> _tiles;
    
    // dynamic container of the played sets;
    std::vector<std::vector<std::shared_ptr<TileSet::Tile>>> _playedSets;
    
    std::vector<std::vector<std::shared_ptr<TileSet::Tile>>> _selectedSets;
    
    // score for a turn
    int _score = 0;
    
    // number of tiles to discard at once;
    int _discardCount = 0;
    
    // numeber of discards per turn;
    int _discardsTurn = 0;
    
public:
    
    Hand(){
        
    }
    
    bool init(std::shared_ptr<TileSet>& tileSet);
    
    /**
     Add the tile to the hand and mark it to be in the hand.
     */
    void draw(std::shared_ptr<TileSet::Tile> tile);
    
    /**
     Discards a tile from the hand.
     If there are numerous tiles of the same type -- remove the first occurance. ???
     
     returns true if the discard was sucessful, and false otherwise.
     */
    bool discard(std::shared_ptr<TileSet::Tile> tile);
    

    /**
     Player constructs sets to play
     It must be a valid set(s) from 1 to 7 in total.
     
     */
    bool makeSet(const std::vector<std::shared_ptr<TileSet::Tile>>& selectedTiles);
    
    /**
     Plays a set (from 2 to 4 cards).
     
     @param
     @return
     */
    bool playSet(); // const std::vector<std::shared_ptr<TileSet::Tile>>& setTiles
    
    /**
     Checks if the given set of tiles "setTiles" is valid under the game's set of rules.
     
     @param setTiles - the tiles to check.
     @return true if the tiles form a VALID set, and false otherwise.
     */
    bool isSetValid(const std::vector<std::shared_ptr<TileSet::Tile>>& selectedTiles);
    
    /**
     returns _playedSets.
     */
    const std::vector<std::vector<std::shared_ptr<TileSet::Tile>>>& getPlayedSets() const{
        return _playedSets;
    }

    /**
     ????
     */
    void rearangeSet();
    
    /**
     Resets for a new turn.
     */
    void reset(){
        
        _discardCount = 0;
        _discardsTurn = 0;
    }
    
    /**
     Counts the total number of selected tiles.
     */
    int countSelectedTiles();
    
    bool isConsecutiveSet(const std::vector<std::shared_ptr<TileSet::Tile>>& selectedTiles);
    
    bool isStraight(const std::vector<std::shared_ptr<TileSet::Tile>>& selectedTiles);
    
    bool isOfaKind(const std::vector<std::shared_ptr<TileSet::Tile>>& selectedTiles);
    
    std::vector<std::shared_ptr<TileSet::Tile>> getSortedTiles(const std::vector<std::shared_ptr<TileSet::Tile>>& selectedTiles);
    
    void updateTilePositions();
    
    void draw(const std::shared_ptr<cugl::graphics::SpriteBatch>& batch);
};

class Player {
private:
    
    Hand _hand;
    int _totalScore = 0;
    int _turnsLeft = 5;
    
public:
    
    Player(){};
    
    /**
     Gets the player's hand.
     */
    Hand& getHand(){
        return _hand;
    }
    
    void endTurn(){
        if(_turnsLeft > 0){
            _turnsLeft--;
            _hand.reset();
        }
    }
    
    
};



#endif /* __MJ_Player_H__ */


