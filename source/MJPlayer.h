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
#include "MJPile.h"
#include "MJScoreManager.h"

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

//TODO: change the HAND to be a subclass of the player.

class Player;

class Hand{
    
private:
    
    // score for a turn
    int _score = 0;
    
    // number of tiles to discard at once;
    int _discardCount = 0;
    
    // numeber of discards per turn;
    int _discardsTurn = 0;
    
    // pointer to the player that owns this hand.
    Player* _player;
    
public:

    
    // dynamic container of the hand tiles.
    std::vector<std::shared_ptr<TileSet::Tile>> _tiles;
    
    // dynamic container of the played sets.
    std::vector<std::vector<std::shared_ptr<TileSet::Tile>>> _playedSets;
    
    // set of sets of tiles which are selected on-line.
    std::vector<std::vector<std::shared_ptr<TileSet::Tile>>> _selectedSets;
    
    // set of selected tiles on-line.
    std::vector<std::shared_ptr<TileSet::Tile>> _selectedTiles;
    
    
public:
    
    Hand(Player* player);
    
    bool init(std::shared_ptr<TileSet>& tileSet);
    
    
    int getTileCount() const;
    
    /**
     Draws tiles from the pile.
     @param a pile.
     */
    void drawFromPile(std::shared_ptr<Pile>& pile);
    
    /**
     Discards a tile from the hand.
     If there are numerous tiles of the same type -- remove the first occurance. ???
     
     returns true if the discard was sucessful, and false otherwise.
     */
    void discard(std::shared_ptr<TileSet::Tile> tile);
    

    /**
     Player constructs a set to play
     It must be a valid set.
     
     */
    bool makeSet();
    
    /**
     Plays a set (from 2 to 4 cards).
     
     @return true if a set was played sucessfully and false otherwise.
     */
    bool playSet();
    
    /**
     Checks if the given set of tiles "selectedTiles" is valid under the game's set of rules.
     @param setTiles - the tiles to check.
     @return true if the tiles form a VALID set, and false otherwise.
     */
    bool isSetValid(const std::vector<std::shared_ptr<TileSet::Tile>>& selectedTiles);
    
    /**
     @Return the global variable _playedSets.
     */
    const std::vector<std::vector<std::shared_ptr<TileSet::Tile>>>& getPlayedSets() const{
        return _playedSets;
    }

    /**
     TODO: implement this.
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
    
    void clickedTile(const cugl::Vec2 mousePos);
    /**
     Confirms if a set isStraight.
     */
    bool isStraight(const std::vector<std::shared_ptr<TileSet::Tile>>& selectedTiles);
    
    /**
     Confirms if a set if of a kind.
     */
    bool isOfaKind(const std::vector<std::shared_ptr<TileSet::Tile>>& selectedTiles);
    
    /**
     Sorts tiles by Rank in ascending order.
     @param a vector of selected tiles.
     @return a vector of tiles.
     
     */
    std::vector<std::shared_ptr<TileSet::Tile>> getSortedTiles(const std::vector<std::shared_ptr<TileSet::Tile>>& selectedTiles);
    
    
    void updateTilePositions();
    
    void draw(const std::shared_ptr<cugl::graphics::SpriteBatch>& batch);
    
    
    // functions for scoring: TODO: not completed. -remove from here once they are completed in the ScoreManager. 
    
    bool hasJack(std::vector<std::shared_ptr<TileSet::Tile>> selectedTiles);
};

class Player {
private:
    
    Hand _hand;
  
public:
    
    int _totalScore = 0;
    int _turnsLeft = 5;
    bool discarding = false;

    
public:
    
    Player() : _hand(this) {}
    
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





