//
//  MJTileSet.h
//  Mahsjong
//
//  Created by Patrick Choo on 2/21/25.
//

#ifndef __MJ_TILESET_H__
#define __MJ_TILESET_H__

#include <cugl/cugl.h>
#include <random>

class TileSet {
public:
    class Tile {
    public:
        /**
         * An enum class representing the suit that each tile can possibly have
         */
        enum class Suit : int {
            WILD_SUIT,
            BAMBOO,
            DOT,
            CRAK
        };
        
        enum class Rank : int {
            ONE = 1,
            TWO = 2,
            THREE = 3,
            FOUR = 4,
            FIVE = 5,
            SIX = 6,
            SEVEN = 7,
            EIGHT = 8,
            NINE = 9,
            WILD_RANK = 10
        };
        
    #pragma mark Tile
        
    private:
        /**
         * Fields for drawing a tile
         */
        std::shared_ptr<cugl::graphics::Texture> _texture;
        
    public:
        Tile::Rank _rank;
        Tile::Suit _suit;
        int _id; // Differentiating duplicate copies of a card from one another; gma's tiles have an _id of -1
        
        /**
         * Fields representing the state of where the current tile is
         *
         * Can be in hand, in pile, or in discarded pile
         */
        bool inPile;
        bool inHand;
        bool discarded;
        
        /**
         * Fields representing the current state of play the tile is in
         */
        bool selected; // For when the player clicks on tile
        bool selectedInSet; // For when the player selects this tile in a valid set
        bool played; // For when the player plays card
        
        cugl::Vec2 pos;
        float _scale;
        
        cugl::Rect tileRect;
        
    public:
        /**
         * Allocates a tile by setting its number and suit
         *
         * A wild tile can either have a wild rank and/or a wild suit
         *
         * @param r     The rank
         * @param s     The suit
         */
        Tile(const Tile::Rank r, const Tile::Suit s);
        
        
        /**
         * Getter for accessing a tile's rank.
         *
         * Returns the associated rank for the tile it was called from.
         * Tiles with a wild rank returns WILD_RANK
         */
        Tile::Rank getRank() const { return _rank; }
        
        /**
         * Getter for accessing a tile's suit.
         *
         * Returns the associated suit for the tile it was called from.
         * Tiles with a wild suit return WILD_SUIT
         */
        Tile::Suit getSuit() const { return _suit; }
        
        /**
         * Randomly generates a suit with type Tile::Suit
         *
         * Returns suit with type Tile::Suit
         */
        static Tile::Suit randomSuit() {
            cugl::Random rd;
            rd.init();
            int randSuit = static_cast<int>(rd.getOpenUint64(0, 4));
            return static_cast<Tile::Suit>(randSuit);
        };
        
        /**
         * Randomly generates a rank
         *
         * Returns a Tile::Rank
         */
        static Tile::Rank randomNumber() {
            cugl::Random rd;
            rd.init();
            int randRank = static_cast<int>(rd.getOpenUint64(1, 11));
            return static_cast<Tile::Rank>(randRank);
        };
        
        /**
         * Animates the tiles one frame at a time
         *
         * **NOT NECCESSARY FOR DIGITAL PROTOTYPE**
         */
        void update(cugl::Size size);
        
        /**
         * Returns a string representation for the current tile's rank
         */
        std::string toStringRank() const {
            switch(_rank) {
                case Tile::Rank::ONE:
                    return "one";
                    break;
                case Tile::Rank::TWO:
                    return "two";
                    break;
                case Tile::Rank::THREE:
                    return "three";
                    break;
                case Tile::Rank::FOUR:
                    return "four";
                    break;
                case Tile::Rank::FIVE:
                    return "five";
                    break;
                case Tile::Rank::SIX:
                    return "six";
                    break;
                case Tile::Rank::SEVEN:
                    return "seven";
                    break;
                case Tile::Rank::EIGHT:
                    return "eight";
                    break;
                case Tile::Rank::NINE:
                    return "nine";
                    break;
                case Tile::Rank::WILD_RANK:
                    return "wild rank";
                    break;
                default:
                    return "no valid rank";
            }
        };
        
        /**
         * Returns a string representation for the current tile's suit
         */
        std::string toStringSuit() const{
            switch(_suit){
                case Tile::Suit::BAMBOO:
                    return "bamboo";
                    break;
                case Tile::Suit::DOT:
                    return "dot";
                    break;
                case Tile::Suit::CRAK:
                    return "crak";
                    break;
                case Tile::Suit::WILD_SUIT:
                    return "wild suit";
                    break;
                default:
                    return "no valid suit";
            }
        }
        
        /**
         * Returns the string form of the current tile
         */
        std::string toString() const {
            return toStringRank() + " of " + toStringSuit();
        }
        
        void setTexture(const std::shared_ptr<cugl::graphics::Texture>& value){
            _texture = value;
        }
                
        std::shared_ptr<cugl::graphics::Texture> getTileTexture(){
            return _texture;
        }
    };

    
#pragma mark The Set
    
public:
    /** Deck with all of the tiles */
    std::vector<std::shared_ptr<Tile>> deck;
    
    /** Grandma's favorite tiles */
    std::vector<std::shared_ptr<Tile>> grandmaTiles;
    /** Reference to texture for grandma tile text */
    std::shared_ptr<cugl::graphics::Texture> gmaTexture;
    
    /**
     * **VERSION 2 **
     * Wild tile set to draw from
     */
    std::vector<std::shared_ptr<Tile>> wildTiles;
    
    int wildCount; //Use this when assigning id's to wild tiles
    int tileCount;
    
    cugl::Vec2 _center; //Center of a tile

    /**
     * Initializes the **STARTING** representation of the deck.
     *
     * When creating a new level, call shuffle to reshuffle the tileSet and create
     * the pile and hand by iterating through the tileSet
     */
    TileSet();
    
    /**
     * Shuffles the tileSet for random assignment
     *
     * **ALWAYS SHUFFLE BEFORE READING FROM DECK**
     */
    void shuffle() {
        cugl::Random rd;
        rd.init();
        rd.shuffle(deck);
    }
    
    /**
     * Prints the current deck
     */
    void printDeck(){
        for(const auto& it : deck){
            CULog("%s", it->toString().c_str());
        }
    }
    
    /**
     * Generates 3 random unique grandma tiles
     */
    void generateGrandmaTiles();
    
    void printGrandmaTiles() {
        for(const auto& it : grandmaTiles){
            CULog("%s", it->toString().c_str());
        }
    }

    
    /**
     * **VERSION 1**
     * Generates a random wild tile after scoring a grandma tile
     */
    std::shared_ptr<TileSet::Tile> generateWildTile() {
        cugl::Random rd;
        rd.init();
        int rank = static_cast<int>(rd.getOpenUint64(1, 11));
        
        std::shared_ptr<Tile> wildTile = std::make_shared<Tile>(static_cast<Tile::Rank>(rank), Tile::Suit::WILD_SUIT);
        wildTile->_id = wildCount;
        wildCount += 1;
        
        return wildTile;
    }
    
    /**
     * **VERSION 2**
     * Generates a set of wild tiles
     */
    void generateWildSet() {
        for(int i = 0; i < 11; i++){
            for(int j = 0; j < 3; j++){
                std::shared_ptr<Tile> newTile = std::make_shared<Tile>(static_cast<Tile::Rank>(i), Tile::Suit::WILD_SUIT);
                newTile->_id = wildCount;
                wildCount += 1;
                
                wildTiles.emplace_back(newTile);
            }
        }
    }
    
    /**
     * **VERSION 2**
     *
     * Picks a random tile from the wild tiles set by shuffling then choosing the first element.
     * Removes the chosen wild tile from wild tile set.
     */
    std::shared_ptr<Tile> pickWildTile(){
        if(wildTiles.empty()){
            throw std::runtime_error("no wild tiles!");
        }
        
        cugl::Random rd;
        rd.init();
        rd.shuffle(wildTiles);

        std::shared_ptr<Tile> currTile = wildTiles.front();
        wildTiles.erase(wildTiles.begin());
        
        return currTile;
    };
    
    /**
     * Sets the texture for all tiles in deck
     */
    void setAllTileTexture(const std::shared_ptr<cugl::AssetManager>& assets);
    
    /**
     * Draws an individual tile
     */
    void draw(const std::shared_ptr<cugl::graphics::SpriteBatch>& batch, cugl::Size size);
};

#endif /* __MJ_TILESET_H__ */
