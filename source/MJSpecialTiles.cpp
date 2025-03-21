//
//  MJSpecialTiles.cpp
//  Mahsjong
//
//  Created by Mariia Tiulchenko on 04.03.2025.
//


#include "MJSpecialTiles.h"
using namespace cugl;
using namespace cugl::graphics;
using namespace cugl::audio;


ActionTile::ActionTile(ActionType type, TileSet::Tile::Suit suit)
    : TileSet::Tile(TileSet::Tile::Rank::ACTION, suit),
    _type(type)  // Initialize _type properly
{

}

std::vector<std::shared_ptr<ActionTile>> ActionTile::actionTiles = {
    std::make_shared<ActionTile>(ActionTile::ActionType::DENIER, TileSet::Tile::Suit::SPECIAL),
    std::make_shared<ActionTile>(ActionTile::ActionType::SEER, TileSet::Tile::Suit::SPECIAL),
    std::make_shared<ActionTile>(ActionTile::ActionType::JUGGERNAUT, TileSet::Tile::Suit::SPECIAL),
    std::make_shared<ActionTile>(ActionTile::ActionType::CLAIRVOYANT, TileSet::Tile::Suit::SPECIAL)
};

std::string ActionTile::toStringSuit() const {
    switch (_type) {
    case ActionTile::ActionType::DENIER:
        return "denier";
        break;
    case ActionTile::ActionType::SEER:
        return "seer";
        break;
    case ActionTile::ActionType::JUGGERNAUT:
        return "juggernaut";
        break;
    case ActionTile::ActionType::CLAIRVOYANT:
        return "clairvoyant";
        break;
    default:
        return "no valid action type";
    }
}
std::string ActionTile::toStringRank() const {
    return "action type";
}

std::string ActionTile::toString() const {
    return toStringRank() + " of " + toStringSuit();
}

void ActionTile::addActionToDeck(std::shared_ptr<TileSet> tileset) {

    //MAKE THIS RANDOM
    for (int a = 1; a < 5; a++) {
        cugl::Random rd;
        rd.init();
        int maxIndex = actionTiles.size() - 1;
        int indexOfTile = static_cast<int>(rd.getOpenUint64(0, maxIndex + 1));
        ActionTile::ActionType type = actionTiles[indexOfTile]->getType();
        TileSet::Tile::Suit suit = actionTiles[indexOfTile]->getSuit();
        std::shared_ptr<ActionTile> newTile = std::make_shared<ActionTile>(type, suit);
        newTile->idAction = a;
        tileset->deck.emplace_back(newTile);
        tileset->tileCount += 1;
    }
}

void ActionTile::applyAction(std::shared_ptr<Pile>& pile) {
    pile->createPile();
}



////////////////////////////////////////COMMAND TILES




CommandTile::CommandTile(CommandType type, TileSet::Tile::Suit suit)
   : TileSet::Tile(TileSet::Tile::Rank::COMMAND, suit),
   _type(type)
{}


//better command methods


std::vector<std::shared_ptr<CommandTile>> CommandTile::_commandTiles = {
    std::make_shared<CommandTile>(CommandTile::CommandType::TWO_HEADED_SNAKE, TileSet::Tile::Suit::SPECIAL),
    std::make_shared<CommandTile>(CommandTile::CommandType::FEI, TileSet::Tile::Suit::SPECIAL),
    std::make_shared<CommandTile>(CommandTile::CommandType::RED_TILE, TileSet::Tile::Suit::SPECIAL),
};

void CommandTile::addCommandToDeck(std::shared_ptr<TileSet> tileset) {

    //MAKE THIS RANDOM
    for (int a = 1; a < 5; a++) {
        cugl::Random rd;
        rd.init();
        int maxIndex = _commandTiles.size() - 1;
        int indexOfTile = static_cast<int>(rd.getOpenUint64(0, maxIndex + 1));
        CommandTile::CommandType type = _commandTiles[indexOfTile]->getType();
        TileSet::Tile::Suit suit = _commandTiles[indexOfTile]->getSuit();
        std::shared_ptr<CommandTile> newTile = std::make_shared<CommandTile>(type, suit);
        newTile->idCommand = a;
        tileset->deck.emplace_back(newTile);
        tileset->tileCount += 1;
    }
}

/**
 Returns a type of the command tile.
 */
std::string CommandTile::toStringRank() const {
    
    switch (_type) {
        case CommandType::TWO_HEADED_SNAKE:
            return "TWO_HEADED_SNAKE";
            break;
        case CommandType::FEI:
            return "FEI";
            break;
        case CommandType::RED_TILE:
            return "RED_TILE";
            break;
        default:
            return "UNKNOWN_COMMAND";
            break;
    }
}









/**
 Initializes command tiles and inserts them to the deck. 4 of each kind.
 */
//void CommandTile::initCommandTiles(std::shared_ptr<TileSet> tileSet){
//   for(int i = 1; i<5; ++i){
        
//      std::shared_ptr<CommandTile> snakeTile = std::make_shared<CommandTile>(CommandTile::CommandType::TWO_HEADED_SNAKE);
//    snakeTile->idCommand = i;
//    tileSet->deck.emplace_back(snakeTile);
        
        
//       std::shared_ptr<CommandTile> feiTile = std::make_shared<CommandTile>(CommandTile::CommandType::FEI);
//      feiTile->idCommand = i;
//       tileSet->deck.emplace_back(feiTile);
        
        
//   std::shared_ptr<CommandTile> redTile = std::make_shared<CommandTile>(CommandTile::CommandType::RED_TILE);
//   redTile->idCommand = i;
//   tileSet->deck.emplace_back(redTile);
        
        // update the number of tiles in the pile.
//   tileSet->tileCount += 3;
//    }
//}


//void CommandTile::setAllTextures(const std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<TileSet> tileSet){
//   for(const auto& commandtile: tileSet->deck){
//       if(commandtile->toStringRank() == "command type"){
//          std::string commandTileTexture = commandtile->toString();
//         std::shared_ptr<Texture> texture = assets->get<Texture>(commandTileTexture);
            
//         if(texture){
//             commandtile->setTexture(assets->get<Texture>(commandTileTexture));
//        }
//        else{
//             CULog("Missing texture for command tile: %s",commandTileTexture.c_str());
//       }
//     }
// }
//}


/**
 Draws commandTiles in the pile.
 */
//void CommandTile::draw(const std::shared_ptr<cugl::graphics::SpriteBatch>& batch, std::shared_ptr<TileSet> tileSet){
//    for(const auto& it : tileSet->deck){
//       if (it->toStringRank() == "command type") {
            
//         Tile curr = (*it);
            // if command tile isn't in the pile
//         if(!curr.inPile){
                //            continue;
//        }
            // if command tile is in the pile
//       Vec2 pos = curr.pos;
//         if(it->selected){
                //           pos.y = curr.pos.y + 10;
//       }
//      Vec2 origin = Vec2(curr.getTileTexture()->getSize().width/2, curr.getTileTexture()->getSize().height/2);
            
//     Affine2 trans;
//        trans.scale(curr._scale);
//      trans.translate(pos);
            
//      Size textureSize(350.0, 415.0);
//     Vec2 rectOrigin(pos - (textureSize * curr._scale)/2);
//     it->tileRect = cugl::Rect(rectOrigin, textureSize * curr._scale);
            
//     batch->draw(curr.getTileTexture(), origin, trans);
// }
//  }
//}

/**
 Remove 2 selected tiles.
 TODO: Requires the user to select and discard 2 tiles first
 */
bool CommandTile::discard_two_tiles(Hand& hand, std::shared_ptr<DiscardPile>& discard){
    
    // global attribute -- tiles that the player selected.
    std::vector<std::shared_ptr<TileSet::Tile>>& selectedTiles = hand._selectedTiles;
    
    // if player doesn't select exactly 2 tiles
    if(selectedTiles.size()!=2){
        return false;
    }
    
    // remove two selected tiles from the hand
    for(std::shared_ptr<TileSet::Tile> tile: selectedTiles){
        auto it = hand._tiles.begin();
        while(it != hand._tiles.end()){
            if (*it == tile) {
                
                (*it)->discarded = true;
//                (*it)->inHand = false;
                discard->addTile(*it);
                
                hand._tiles.erase(it);
                            
            } else {
                ++it;
            }
        }
    }

    selectedTiles.clear();
    
    return true;
}

/**
 A player looses all action tiles.
 */
bool CommandTile::lose_actions(Hand& hand, std::shared_ptr<DiscardPile> discard) {
    // Vector to store action tiles to be removed
    std::vector<std::shared_ptr<TileSet::Tile>> actionTilesToRemove;

    for (const std::shared_ptr<TileSet::Tile>& tile : hand._tiles) {
        if (tile->toStringRank() == "action") {
            actionTilesToRemove.push_back(tile);
        }
    }

    for (const std::shared_ptr<TileSet::Tile>& tile : actionTilesToRemove) {
        auto it = std::find(hand._tiles.begin(), hand._tiles.end(), tile);
        if (it != hand._tiles.end()) {
            (*it)->discarded = true;
//            (*it)->inHand = false;
            discard->addTile(*it);
            hand._tiles.erase(it);
        }
    }

    return !actionTilesToRemove.empty();  // Return true if action tiles were removed
}



/**
 Trade a tile with your opponent.
 From the opponent's side - just takes the random
 before calling requires selectedTiles to be non-empty
 */
bool CommandTile::trade_a_tile(Player& player, Player& opponent){ // , NetworkController& network
    
    Hand& playerHand = player.getHand();
    Hand& opponentHand = opponent.getHand();
    
    if (playerHand._selectedTiles.empty() || opponentHand._selectedTiles.empty()) {
        return false;
    }
    
    // confirm that a player chooses a single tile to exchange.
    if (playerHand._selectedTiles.size() != 1 ||
        opponentHand._selectedTiles.size() != 1) {
            return false;
        }
    
    // remove the tile from the player's hand
    for(std::shared_ptr<TileSet::Tile> tile: playerHand._selectedTiles){
        auto it = playerHand._tiles.begin();
        while(it != playerHand._tiles.end()){
            if (*it == tile) {
                                
                playerHand._tiles.erase(it);
                            
            } else {
                ++it;
            }
        }
    }
    
    // remove the tile from the opponent's hand
    for(std::shared_ptr<TileSet::Tile> tile: opponentHand._selectedTiles){
        auto it = opponentHand._tiles.begin();
        while(it != opponentHand._tiles.end()){
            if (*it == tile) {
                                
                opponentHand._tiles.erase(it);
                            
            } else {
                ++it;
            }
        }
    }
    
    // exchange the tiles.
    playerHand._tiles.push_back(opponentHand._selectedTiles[0]);
    opponentHand._tiles.push_back(playerHand._selectedTiles[0]);
    
    // clear both _selectedTiles.
    playerHand._selectedTiles.clear();
    opponentHand._selectedTiles.clear();
    
    return true;
    
}
