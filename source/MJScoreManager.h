//
//  MJScoreManager.h
//  Mahsjong
//
//  Created by Patrick Choo on 2/18/25.
//

#ifndef _MJ_SCORE_MANAGER_H_
#define _MJ_SCORE_MANAGER_H_

#include <cugl/cugl.h>
#include "MJTileSet.h"

/**
 * This is the class handling all scoring in regards to player sets
 */
class ScoreManager {
private:
    /** Vector of vectors of tiles where each vector represents a set of tiles */
    std::vector<std::vector<std::shared_ptr<TileSet::Tile>>> _playedHand;

public:
#pragma mark -
#pragma mark Constructors
    
    /**
     * Initializes ScoreManager object handles sets that players play
     *
     * @param played_sets  The vector of tile sets played by the player
     */
    ScoreManager(const std::vector<std::vector<std::shared_ptr<TileSet::Tile>>>& played_sets);
    
#pragma mark -
#pragma mark Gameplay Handling
    /**
     * Method to calculate the score of the played sets
     *
     * @return the total score from all the played set
     */
    int calculateScore();
    
    /**
     * Method to convert current played hand to set made up of non-wild tiles
     *
     * @param playedHand    the hand played
     */
    void unWildHand(std::vector<std::vector<std::shared_ptr<TileSet::Tile>>> playedHand);
    
    /**
     * Method to check if this set has already been played
     *
     * @param already_played    vector of sets already played
     * @param vectorized_set    vector representing the set
     * @return true if set has already been played, false otherwise
     */
    bool isAlreadyPlayed(const std::vector<std::vector<int>>& already_played, const std::vector<int>& vectorized_set);

    /**
     * Method to check if a vector of tiles is consecutive
     *
     * @param already_played    vector of sets already played
     * @param vectorized_set    vector representing the set
     * @return true if these tiles are consecutive, and false otherwise
     */
    bool isConsecutive(const std::vector<std::vector<int>>& already_played, const std::vector<int>& vectorized_set);

    /**
     * Method to check if this set can be the previous set using wild tiles
     *
     * @param prev the vector of tiles representing previous set
     * @param curr the vector of tiles representing current set
     * @return true if the current set can be the previous set, and false otherwise
     */
    bool canBePrev(const std::vector<std::shared_ptr<TileSet::Tile>>& prev,const std::vector<std::shared_ptr<TileSet::Tile>>& curr);

    /**
     * Makes a three or four of a kind from the vector of tiles
     *
     * @param curr the vector of tiles representing current set
     * @return vector of tiles representing a three or four of a kind
     */
    std::vector<std::shared_ptr<TileSet::Tile>> makeKind(const std::vector<std::shared_ptr<TileSet::Tile>>& curr);

    /**
     * Makes a straight from the vector of tiles
     *
     * @param curr the vector of tiles representing current set
     * @return vector of tiles representing a straight
     */
    std::vector<std::shared_ptr<TileSet::Tile>> makeStraight(const std::vector<std::shared_ptr<TileSet::Tile>>& curr);

    /**
     * Method to check if these tiles can be a three or four of a kind
     *
     * @param curr   vector of tiles representing current set being scored
     * @return true if these tiles can be a three or four of a kind, and false otherwise
     */
    bool canBeKind(const std::vector<std::shared_ptr<TileSet::Tile>>& curr);

    /**
     * Method that makes known suits for scoring
     *
     * @param curr the current set of tiles to score
     * @return a vector of tiles
     */
    std::vector<std::shared_ptr<TileSet::Tile>> makeKnownSuits(const std::vector<std::shared_ptr<TileSet::Tile>>& curr);

    /**
     * Method that returns the number of jacks in the current set
     *
     * @param curr  the current vector of tiles being scored
     * @return the number of jacks in this vector of tiles
     */
    int numJacks(const std::vector<std::shared_ptr<TileSet::Tile>>& curr);

};

#endif /* _MJ_SCORE_MANAGER_ */
