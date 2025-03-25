////
////  MJSpecialTiles.h
////  Mahsjong
////
////  Created by Mariia Tiulchenko on 04.03.2025.
////
//
//#define MJ_SPECIAL_TILES_H
//#include "MJTileSet.h"
//#include "MJPile.h"
//#include "MJPlayer.h"
//#include "MJDiscardPile.h"
//
//
//class ActionTile : public TileSet::Tile {
//public:
//    enum class ActionType : int {
//        DENIER,
//        SEER,
//        JUGGERNAUT,
//        CLAIRVOYANT
//    };
//protected:
//    ActionType _type;
//
//    static std::vector<std::shared_ptr<ActionTile>> actionTiles;
//
//
//
//public:
//    /**
//     * Constructor for an ActionTile.
//     *
//     * @param s The suit that defines action of the tile.
//     */
//    ActionTile(ActionType type, TileSet::Tile::Suit suit);
//
//    int idAction;
//
//    bool isActionTile = true;
//
//    bool actionInUse = false;
//
//    ActionType getType() const { return _type; }
//
//    /**
//    * Returns a string representation of the action suit.
//    *
//    * @return A string describing the action tile suit.
//    */
//    std::string toStringSuit() const;
//
//    std::string toStringRank() const;
//
//    void addActionToDeck(std::shared_ptr<TileSet> tileset);
//
//    /**
//    * Returns a full string representation of the action tile.
//    *
//    * @return A string describing the tile.
//    */
//    std::string toString() const;
//
//    /**
//    *Creates new top layer that is flipped over. Should be called when player has made pair after playing Clairvoyant action tile.
//    *
//    * @param: h    The player's hand
//    * @param: p    The drawing pile
//    */
//    void applyAction(std::shared_ptr<Pile>& pile);
//};
//
//
//class CommandTile: public TileSet::Tile{
//    
//private:
//    std::shared_ptr<cugl::graphics::Texture> _texture;
//    
//public:
//    enum class CommandType : int {
//        TWO_HEADED_SNAKE,
//        FEI,
//        RED_TILE
//    };
//    
//    // command tile type.
//    CommandType _type;
//    
//    // identifies if the tile is a command tile.
//    bool isCommandTile = true;
//    
//    //
//    int idCommand;
//    
//    // vector of all command tiles
//    static std::vector<std::shared_ptr<CommandTile>> _commandTiles;
//    
//    // chosen tile for red_tile command tile.
//    std::shared_ptr<TileSet::Tile> _chosenTile;
//    
//    CommandType getType() const { return _type; }
//    
//public:
//    
//    CommandTile(CommandType s, TileSet::Tile::Suit suit);
//    
//    /**
//     Initialize command tiles and iserts them to the deck. It sets 4 of each type of tiles = 12 command tiles in total.
//     */
//    //static void initCommandTiles(std::shared_ptr<TileSet> tileSet);
//    
//    /**
//     
//     */
//    //void setTexture(const std::shared_ptr<cugl::graphics::Texture>& value){
//     //   _texture = value;
//   // }
//    
//    /**
//     
//     */
//    //static void setAllTextures(const std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<TileSet> tileSet);
//    
//    //static void draw(const std::shared_ptr<cugl::graphics::SpriteBatch>& batch, std::shared_ptr<TileSet> tileSet);
//
//    
//    std::string toStringRank() const;
//    
//    // COMMAND TILES TYPES
//    
//    /**
//     Removes 2 selected tiles of the player's choice.
//     Requires the user to select 2 tiles first.
//     */
//    static bool discard_two_tiles(Hand& hand, std::shared_ptr<DiscardPile>& discard); // TWO_HEADED_SNAKE
//    
//
//    /**
//     TODO: implement this
//     Game gives you a particular pair to put into the hand.
//     Might need to discard a tile.
//     */
//    bool bonus_pair(Player& player);
//    
//    
//    /**
//     TODO: implement this
//     If you have 3 sets made, discard a set from the hand.
//     */
//    void disard_a_set();
//    
//
//    /**
//     Lose all Action tiles.
//     */
//    static bool lose_actions(Hand& hand, std::shared_ptr<DiscardPile> discard);  // FEI
//    
//    
//    /**
//     Trade a tile with your opponent.
//     */
//    static bool trade_a_tile(Player& player, Player& opponent); // RED_TILE
//    
//    static void addCommandToDeck(std::shared_ptr<TileSet> tileset);
//
//};
