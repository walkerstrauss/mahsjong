//
//  MJScoreManager.cpp
//  Mahsjong
//
//  Created by Patrick Choo on 2/18/25.
//

#include "MJScoreManager.h"
#include "MJPlayer.h"
#include "MJTileSet.h"

/**
 * This is the class handling all scoring in regards to player sets
 *
 * TODO: Please implement scoring functionality
 *
 */
ScoreManager::ScoreManager(const std::vector<std::vector<std::shared_ptr<TileSet::Tile>>>& played_sets) {
    _playedHand = played_sets;
}


int ScoreManager::calculateScore() {
    int score = 0;
    std::vector<std::vector<int>> already_played;
    for (const auto& tileSet : _playedHand) {
        score += tileSet.size();
        
        //make a vector that is easy to compare to verify if set has been played already
        std::vector<int> vectorized_set;
        TileSet::Tile::Suit suitysuit = tileSet[0]->getSuit();
        int suit = static_cast<int>(suitysuit);
        vectorized_set.push_back(suit);

        for (const auto& tile : tileSet) {
            TileSet::Tile::Rank rankyrank = tile->getRank();
            int rank = static_cast<int>(rankyrank);
            vectorized_set.push_back(rank);
        }
        
        std::sort(vectorized_set.begin() + 1, vectorized_set.end());

        //add combo if set is is consecutive or successive to hand already played
        if (isConsecutive(already_played, vectorized_set)) {
            score += 10;
        }

        //double scores set if already played
        if (isAlreadyPlayed(already_played, vectorized_set)) {
            score += tileSet.size();
        }
        else {
            already_played.push_back(vectorized_set);
        }
    }

    return score;
}

bool ScoreManager::isAlreadyPlayed(const std::vector<std::vector<int>>& already_played, const std::vector<int>& vectorized_set) {
    for (const auto& vec : already_played) {
        if (vectorized_set == vec) {
            return true;
        }
    }
    return false;
}

bool ScoreManager::isConsecutive(const std::vector<std::vector<int>>& already_played, const std::vector<int>& vectorized_set) {
    int length = vectorized_set.size() - 1;
    std::vector<int> before;
    std::vector<int> after;
    before.push_back(vectorized_set[0]);
    after.push_back(vectorized_set[0]);
    for (std::size_t i = 1; i < vectorized_set.size(); i++) {
        before.push_back(vectorized_set[i] - length);
        after.push_back(vectorized_set[i] + length);
    }
    for (const auto& vec : already_played) {
        if (before == vec || after == vec) {
            return true;
        }
    }
    return false;
}

bool ScoreManager::canBePrev(const std::vector<std::shared_ptr<TileSet::Tile>>& prev, const std::vector<std::shared_ptr<TileSet::Tile>>& curr) {
    bool result = false;
    //previous set, lacking any wild tiles, can easily be vectorized to compare with
    std::vector<int> vectorized_prev;
    TileSet::Tile::Suit suitysuit = prev[0]->getSuit();
    int suit = static_cast<int>(suitysuit);
    vectorized_prev.push_back(suit);
    for (const auto& tile : prev) {
        TileSet::Tile::Rank rankyrank = tile->getRank();
        int rank = static_cast<int>(rankyrank);
        vectorized_prev.push_back(rank);
    }

    //check if suits can match
    for (const auto& tile : curr) {
        TileSet::Tile::Suit fit_check = tile->getSuit();
        int fit = static_cast<int>(fit_check);
        //return false if not wild-suit or suit of prev
        if (!(fit == 0 || fit == vectorized_prev[0])) {
            return false;
        }
    }
    return result;
}

std::vector<std::vector<std::shared_ptr<TileSet::Tile>>> ScoreManager::makeKind(const std::vector<std::shared_ptr<TileSet::Tile>>& curr) {
    
    std::vector<std::vector<std::shared_ptr<TileSet::Tile>>> value;
    
    return value;
}

std::vector<std::vector<std::shared_ptr<TileSet::Tile>>> ScoreManager::makeStraight(const std::vector<std::shared_ptr<TileSet::Tile>>& curr) {
    std::vector<std::vector<std::shared_ptr<TileSet::Tile>>> value;
    
    return value;
}

bool ScoreManager::canBeKind(const std::vector<std::shared_ptr<TileSet::Tile>>& curr) {
    return false;
}

bool ScoreManager::hasWilds(const std::vector<std::shared_ptr<TileSet::Tile>>& curr) {
    return false;
}
