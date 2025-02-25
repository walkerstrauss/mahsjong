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
 *
 * TODO: Please implement scoring functionality
 */

class ScoreManager {
private:
    std::vector<std::vector<std::shared_ptr<TileSet::Tile>>> _playedHand;

public:
    /**
    * Intializes ScoreManager object that handles sets that player plays.
    *
    * @param played_set: The set of tile sets played by player
    */
    ScoreManager(const std::vector<std::vector<std::shared_ptr<TileSet::Tile>>>& played_sets);

    int calculateScore();

    bool isAlreadyPlayed(const std::vector<std::vector<int>>& already_played, const std::vector<int>& vectorized_set);

    bool isConsecutive(const std::vector<std::vector<int>>& already_played, const std::vector<int>& vectorized_set);

    bool canBePrev(const std::vector<std::shared_ptr<TileSet::Tile>>& prev,const std::vector<std::shared_ptr<TileSet::Tile>>& curr);

    std::vector<std::vector<std::shared_ptr<TileSet::Tile>>> makeKind(const std::vector<std::shared_ptr<TileSet::Tile>>& curr);

    std::vector<std::vector<std::shared_ptr<TileSet::Tile>>> makeStraight(const std::vector<std::shared_ptr<TileSet::Tile>>& curr);

    bool canBeKind(const std::vector<std::shared_ptr<TileSet::Tile>>& curr);

    bool hasWilds(const std::vector<std::shared_ptr<TileSet::Tile>>& curr);

};

#endif /* _MJ_SCORE_MANAGER_ */
