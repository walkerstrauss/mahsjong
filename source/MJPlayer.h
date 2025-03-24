//
//  MJPlayer.h
//  Mahsjong
//
//  Created by Patrick Choo on 2/18/25.
//

#ifndef __MJ_MAIN_MENU_H__
#define __MJ_MAIN_MENU_H__

#include <cugl/cugl.h>
#include <vector>
#include <iostream>
#include "MJTileSet.h"
#include "MJPile.h"
#include "MJScoreManager.h"

/**
 * This class represents a player in the game
 */
class Player;

/**
 * This class represents a player's hand
 */
class Hand{
    
private:
    // Field to keep track of score per turn
    int _score = 0;
    // Tracks number of cards discarded
    int _discardCount = 0;
    // How many times the player can discard per turn
    int _discardsTurn = 0;
    // Player whose hand this is
    Player* _player;
    
public:
    // The tiles in our hand
    std::vector<std::shared_ptr<TileSet::Tile>> _tiles;
    // Drawn pile tiles at a given time
    std::vector<std::shared_ptr<TileSet::Tile>> _drawnPile;
    // The sets we have made from our hand this turn
    std::vector<std::vector<std::shared_ptr<TileSet::Tile>>> _playedSets;
    // Contains all sets we have made from our hand this turn
    std::vector<std::vector<std::shared_ptr<TileSet::Tile>>> _selectedSets;
    // Holds all tiles that are selected in our hand
    std::vector<std::shared_ptr<TileSet::Tile>> _selectedTiles;
    // Keeps track of which grandma tile we are checking
    int grandmaToAdd;
    // Keeps track of current hand size
    int _size; 
    
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new hand for the player
     *
     * @param player    the player whose hand we are initilaizing
     */
    Hand(Player* player);
    
    /**
     * Initializes a new host hand by pulling 14 tiles from the game tileset
     *
     * @param tileSet   the tileset to draw from
     */
    bool initHand(std::shared_ptr<TileSet>& tileSet, bool isHost);
    
#pragma mark -
#pragma mark Gameplay Handling
    /**
     * Returns the number of tiles in our game
     */
    size_t getTileCount() const {
        return _tiles.size();
    }
    
    /**
     * Draws how ever many cards we need from the pile
     *
     * @param pile      the pile to draw to our hand from
     */
    void drawFromPile(std::shared_ptr<Pile>& pile, int number, bool isHost);
    
    /**
     * Draws the given tile from the discard pile and adds it to hand.
     */
    void drawFromDiscard(std::shared_ptr<TileSet::Tile> tile, bool isHost);
    
    /**
     * Discards a single specified tile from our hand
     *
     * @param tile      the tile to discard from out hand
     */
    bool discard(std::shared_ptr<TileSet::Tile> tile, bool isHost);
    
    /**
     * Discards all action tiles in hand and returns number of action tiles discarded.
     */
    int loseActions(bool isHost);
    
    /**
     * Method to make a set from your hand and add it to selected sets
     *
     * @returns true if successfully made VALID set, and false otherwise
     */
    bool makeSet();
    
    /**
     * Method to play a set from your hand (of 2 to 4 cards)
     *
     * @return true if a set was played sucessfully and false otherwise.
     */
    bool playSet(const std::shared_ptr<TileSet>& tileSet, bool isHost);
    
    /**
     * Checks if the given set of tiles "selectedTiles" is valid under the game's set of rules.
     *
     * @param setTiles      the tiles to check.
     * @return true if the tiles form a VALID set, and false otherwise.
     */
    bool isSetValid(const std::vector<std::shared_ptr<TileSet::Tile>>& selectedTiles);
    
    /**
     * Method to get all played sets from hand
     *
     * @return a vector of tiles containing the played sets
     */
    const std::vector<std::vector<std::shared_ptr<TileSet::Tile>>>& getPlayedSets() const{
        return _playedSets;
    }

    /**
     * Method to rearrange set in your selected sets before playing
     */
    void rearangeSet();
    
    /**
     * Resets the hand fields to handle a new turn.
     */
    void reset(){
        _discardCount = 0;
        _discardsTurn = 0;
    }
    
    /**
     * Counts the total number of selected tiles.
     */
    int countSelectedTiles();
    
    /**
     * Handles selection of tiles using information from input event
     *
     * @param mousePos      the position of the mouse in this frame
     */
    std::shared_ptr<TileSet::Tile> clickedTile(const cugl::Vec2 mousePos);
    
    /**
     * Confirms if a set isStraight.
     *
     * @param selectedTiles     the tiles to be checked for a straight
     */
    bool isStraight(const std::vector<std::shared_ptr<TileSet::Tile>>& selectedTiles);
    
    /**
     * Confirms if a set if of a kind.
     *
     * @param selectedTiles     the tiles to be checked for a three/four of a kind
     */
    bool isOfaKind(const std::vector<std::shared_ptr<TileSet::Tile>>& selectedTiles);
    
    bool isWinningHand();
    
    bool onePairFourSets(std::map<std::pair<TileSet::Tile::Rank, TileSet::Tile::Suit>, int>& tileCounts, int pair, int sets);
    
    /**
     * Method to sort the tiles by Rank and Suit in ascending order.
     *
     * @param selectedTiles     a vector of selected tiles.
     * @return a vector of tiles sorted by Rank and Suit
     */
    std::vector<std::shared_ptr<TileSet::Tile>> getSortedTiles(const std::vector<std::shared_ptr<TileSet::Tile>>& selectedTiles);
    
    /**
     * Updates the position of all tiles in the hand for drawing to the screen and selection detection
     */
    void updateTilePositions(cugl::Size sceneSize);
    
    /**
     * Method to draw the tiles in our hand to the screen (no longer used)
     *
     * @param batch     the SpriteBatch to draw the tiles in our hand to
     */
    void draw(const std::shared_ptr<cugl::graphics::SpriteBatch>& batch);
    
    /**
     * Method to check if selected tiles contain a wild card (jack)
     *
     * @param selectedTiles     the tiles to be checked for a wild card
     */
    bool hasJack(std::vector<std::shared_ptr<TileSet::Tile>> selectedTiles);
    
    
    std::shared_ptr<TileSet::Tile> getTileAtPosition(const cugl::Vec2& mousePos);
    
    
    
    
};

// Player as subclass of hand for handling individual turns for the player
class Player {
private:
    // The player's hand
    Hand _hand;
  
public:
    // The current total score of the player
    int _totalScore = 0;
    // The number of turns remaining
    int _turnsLeft = 5;
    // Whether or not we are currently discarding
    bool discarding = false;
    // Whether or not we are in exchange and play phase
    bool canExchange;
    // Whether or not the player has drawn this turn
    bool canDraw;

    std::shared_ptr<TileSet::Tile> _draggingTile = nullptr;
 

#pragma mark -
#pragma mark Constructors
    
    /**
     * Constructs a new player with this hand object
     */
    Player() : _hand(this) {}
    
#pragma mark -
#pragma mark Gameplay Handling
    
    /**
     * Gets the player's hand.
     */
    Hand& getHand(){
        return _hand;
    }
    
    /**
     * Method to end the player's turn
     */
    void endTurn(){
        if(_turnsLeft > 0){
            _turnsLeft--;
        }
    }
    
    
    std::shared_ptr<TileSet::Tile> getDraggingTile() const { return _draggingTile; }
    /**
     * Renders the current tiles in hand
     */
    void draw(const std::shared_ptr<cugl::graphics::SpriteBatch>& batch);
};

#endif /* __MJ_Player_H__ */





