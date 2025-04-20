////
////  MJScoreManager.cpp
////  Mahsjong
////
////  Created by Patrick Choo on 2/18/25.
////
//
//#include "MJScoreManager.h"
//#include "MJTileSet.h"
//
///**
// * This is the class handling all scoring in regards to player sets
// */
//#pragma mark -
//#pragma mark Constructors
///**
// * Initializes ScoreManager object handles sets that players play
// *
// * @param played_sets  The vector of tile sets played by the player
// */
//ScoreManager::ScoreManager(const std::vector<std::vector<std::shared_ptr<TileSet::Tile>>>& played_sets) {
//    _playedHand = played_sets;
//}
//
//#pragma mark -
//#pragma mark Gameplay Handling
///**
// * Method to calculate the score of the played sets
// *
// * @return the total score from all the played set
// */
//int ScoreManager::calculateScore() {
//    int score = 0;
//    std::vector<std::vector<int>> already_played;
//    for (const auto& tileSet : _playedHand) {
//        score += tileSet.size();
//        std::vector<std::shared_ptr<TileSet::Tile>> knownSet = makeKind(tileSet);
//        
//        //make a vector that is easy to compare to verify if set has been played already
//        std::vector<int> vectorized_set;
//        TileSet::Tile::Suit suitysuit = knownSet[0]->getSuit();
//        int suit = static_cast<int>(suitysuit);
//        vectorized_set.push_back(suit);
//
//        for (const auto& tile : tileSet) {
//            TileSet::Tile::Rank rankyrank = tile->getRank();
//            int rank = static_cast<int>(rankyrank);
//            vectorized_set.push_back(rank);
//        }
//        std::sort(vectorized_set.begin() + 1, vectorized_set.end());
//
//        //add combo if set is is consecutive or successive to hand already played
//        if (isConsecutive(already_played, vectorized_set)) {
//            score += 10;
//        }
//
//        //double scores set if already played
//        if (isAlreadyPlayed(already_played, vectorized_set)) {
//            score += tileSet.size();
//        } else {
//            already_played.push_back(vectorized_set);
//        }
//    }
//    return score;
//}
//
///**
// * Method to check if this set has already been played
// *
// * @param already_played    vector of sets already played
// * @param vectorized_set    vector representing the set
// * @return true if set has already been played, false otherwise
// */
//bool ScoreManager::isAlreadyPlayed(const std::vector<std::vector<int>>& already_played, const std::vector<int>& vectorized_set) {
//    for (const auto& vec : already_played) {
//        if (vectorized_set == vec) {
//            return true;
//        }
//    }
//    return false;
//}
//
///**
// * Method to convert current played hand to set made up of non-wild tiles
// *
// * @param playedHand    the hand played
// */
//void ScoreManager::unWildHand(std::vector<std::vector<std::shared_ptr<TileSet::Tile>>> playedHand) {
//    std::vector<std::vector<std::shared_ptr<TileSet::Tile>>> new_hand;
//    std::vector<std::shared_ptr<TileSet::Tile>> new_set;
//    std::vector<std::shared_ptr<TileSet::Tile>> prev;
//    //handle the first hand if it is composed of wild tiles
//    for (std::size_t i = 0; i < playedHand.size(); i++) {
//        if (i == 0){
//            new_set = makeKnownSuits(playedHand[0]);
//            int tally = numJacks(new_set);
//            if (tally > 0) {
//                if (canBeKind(new_set)) {
//                    new_set = makeKind(new_set);
//                }
//                else {
//                    new_set = makeStraight(playedHand[0]);
//                }
//            }
//            prev = new_set;
//        }
//        else{
//            if (canBePrev(prev, playedHand[i])) {
//                new_set = prev;
//            }
//            else if (canBeKind(playedHand[i])) {
//                new_set = makeKind(playedHand[i]);
//            }
//            else {
//                new_set = makeStraight(playedHand[i]);
//            }
//        }
//        new_hand.push_back(new_set);
//    }
//    _playedHand = new_hand;
//    return;
//}
//
///**
// * Method to check if a vector of tiles is consecutive
// *
// * @param already_played    vector of sets already played
// * @param vectorized_set    vector representing the set
// * @return true if these tiles are consecutive, and false otherwise
// */
//bool ScoreManager::isConsecutive(const std::vector<std::vector<int>>& already_played, const std::vector<int>& vectorized_set) {
//    int length = vectorized_set.size() - 1;
//    std::vector<int> before;
//    std::vector<int> after;
//    before.push_back(vectorized_set[0]);
//    after.push_back(vectorized_set[0]);
//    for (std::size_t i = 1; i < vectorized_set.size(); i++) {
//        before.push_back(vectorized_set[i] - length);
//        after.push_back(vectorized_set[i] + length);
//    }
//    for (const auto& vec : already_played) {
//        if (before == vec || after == vec) {
//            return true;
//        }
//    }
//    return false;
//}
//
///**
// * Method to check if this set can be the previous set using wild tiles
// *
// * @param prev the vector of tiles representing previous set
// * @param curr the vector of tiles representing current set
// * @return true if the current set can be the previous set, and false otherwise
// */
//bool ScoreManager::canBePrev(const std::vector<std::shared_ptr<TileSet::Tile>>& prev, const std::vector<std::shared_ptr<TileSet::Tile>>& curr) {
//    bool result = false;
//    //previous set, lacking any wild tiles, can easily be vectorized to compare with
//    std::vector<int> vectorized_prev;
//    TileSet::Tile::Suit suitysuit = prev[0]->getSuit();
//    int suit = static_cast<int>(suitysuit);
//    vectorized_prev.push_back(suit);
//    for (const auto& tile : prev) {
//        TileSet::Tile::Rank rankyrank = tile->getRank();
//        int rank = static_cast<int>(rankyrank);
//        vectorized_prev.push_back(rank);
//    }
//    std::sort(vectorized_prev.begin() + 1, vectorized_prev.end());
//
//    //check if suits can match
//    for (const auto& tile : curr) {
//        TileSet::Tile::Suit fit_check = tile->getSuit();
//        int fit = static_cast<int>(fit_check);
//        //return false if not wild-suit or suit of prev
//        if (!(fit == 0 || fit == vectorized_prev[0])) {
//            return false;
//        }
//    }
//    return result;
//}
//
///**
// * Makes a three or four of a kind from the vector of tiles
// *
// * @param curr the vector of tiles representing current set
// * @return vector of tiles representing a three or four of a kind
// */
//std::vector<std::shared_ptr<TileSet::Tile>> ScoreManager::makeKind(const std::vector<std::shared_ptr<TileSet::Tile>>& curr) {
//    std::vector<std::shared_ptr<TileSet::Tile>> result = makeKnownSuits(curr);
//    return result;
//}
//
///**
// * Makes a straight from the vector of tiles
// *
// * @param curr the vector of tiles representing current set
// * @return vector of tiles representing a straight
// */
//std::vector<std::shared_ptr<TileSet::Tile>> ScoreManager::makeStraight(const std::vector<std::shared_ptr<TileSet::Tile>>& curr) {
//    std::vector<std::shared_ptr<TileSet::Tile>> result = makeKnownSuits(curr);
//    return result;
//}
//
///**
// * Method to check if these tiles can be a three or four of a kind
// *
// * @param curr   vector of tiles representing current set being scored
// * @return true if these tiles can be a three or four of a kind, and false otherwise
// */
//bool ScoreManager::canBeKind(const std::vector<std::shared_ptr<TileSet::Tile>>& curr) {
//    return false;
//}
//
/////**
//// * Method that makes known suits for scoring
//// *
//// * @param curr the current set of tiles to score
//// * @return a vector of tiles
//// */
////std::vector<std::shared_ptr<TileSet::Tile>> ScoreManager::makeKnownSuits(const std::vector<std::shared_ptr<TileSet::Tile>>& curr) {
////    std::vector<std::shared_ptr<TileSet::Tile>> result;
////    TileSet::Tile::Suit suit = TileSet::Tile::Suit::WILD_SUIT;
////    for (const auto& tile : curr) {
////        if (tile->getSuit() != TileSet::Tile::Suit::WILD_SUIT) {
////            suit = tile->getSuit();
////            break;
////        }
////    }
////    if (suit == TileSet::Tile::Suit::WILD_SUIT) {
////        suit = TileSet::Tile::Suit::BAMBOO;
////    }
////    for (const auto& tile : curr) {
////        TileSet::Tile new_tile = TileSet::Tile(tile->getRank(), suit);
////        result.push_back(std::make_shared<TileSet::Tile>(tile->getRank(), suit));
////    }
////    return result;
////}
//
/////**
//// * Method that returns the number of jacks in the current set
//// *
//// * @param curr  the current vector of tiles being scored
//// * @return the number of jacks in this vector of tiles
//// */
////int ScoreManager::numJacks(const std::vector<std::shared_ptr<TileSet::Tile>>& curr) {
////    int tally = 0;
////    for (const auto& tile : curr) {
////        if (tile->getRank() == TileSet::Tile::Rank::WILD_RANK) {
////            tally += 1;
////        }
////    }
////    return tally;
////}
