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

/**
 * A class representing all of the tiles in the player's tileset as the game progresses
 */
class TileSet {
public:
    /**
     * A class representing a single tile
     */
    class Tile {
    private:
        /** This tile's texture */
        std::shared_ptr<cugl::graphics::Texture> _texture;
        
    public:
        /** An enum to represent tile suits */
        enum class Suit : int {
            WILD_SUIT,
            CRAK,
            BAMBOO,
            DOT
        };
        
        /** An enum to represent tile rank (1-9 + 10 for wild tiles) */
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
        
        /** The tile's rank */
        Tile::Rank _rank;
        /** The tile's suit */
        Tile::Suit _suit;
        /** Id for differentiating duplicate copies of a card from one another – gma's tiles have an _id of -1 */
        int _id;
        /** The position of the tile (row, col) in the pile */
        cugl::Vec2 pileCoord;
        /** Whether or not the tile is in the pile */
        bool inPile;
        /** Whether or not the tile is in the host's hand */
        bool inHostHand;
        /** Whether or not the tile is in the client's hand*/
        bool inClientHand;
        /** Boolean flag for if this tile has been discarded */
        bool discarded;
        /** Boolean flag for it this tile is the top tile of the discard pile */
        bool topTile;
        /** Whether the player has selected the tile */
        bool selected;
        /** Whether the player has selected the tile and it is in a set */
        bool selectedInSet;
        /** True if this tile has been played, false otherwise */
        bool played;
        /** If tile is in the current deck */
        bool inDeck;
        /** The tile's position (updated in player and pile methods) */
        cugl::Vec2 pos;
        /** The rectangle representing the tile's position used for selection handling */
        cugl::Rect tileRect;
        /** The scale of the tile */
        float _scale;
        
#pragma mark -
#pragma mark Tile Constructors
        /**
         * Allocates a tile by setting its number and suit
         *
         * A wild tile can either have a wild rank and/or a wild suit
         *
         * @param r     The rank
         * @param s     The suit
         */
        Tile(const Tile::Rank r, const Tile::Suit s);
        
#pragma mark -
#pragma mark Tile Gameplay Handling
        /**
         * Getter for accessing a tile's rank. Tiles with a wild rank returns WILD_RANK
         *
         * @return the associated rank for the tile it was called from.
         */
        Tile::Rank getRank() const { return _rank; }
        
        /**
         * Getter for accessing a tile's suit. Tiles with a wild suit return WILD_SUIT
         *
         * @return the associated suit for the tile it was called from.
         */
        Tile::Suit getSuit() const { return _suit; }
        
        /**
         * Method to update the tiles
         *
         * @param timestep      the time since the last update
         */
        void update(float timestep);
        
        /**
         * Returns a string representation for the current tile's rank
         *
         * @return a string representing the tile's rank
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
         *
         * @return a string representing the tile's suit
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
        
        static Tile::Rank toRank(std::string rank) {
            if (rank == "one" || rank == "1") {
                return Tile::Rank::ONE;
            } else if (rank == "two" || rank == "2") {
                return Tile::Rank::TWO;
            } else if (rank == "three" || rank == "3") {
                return Tile::Rank::THREE;
            } else if (rank == "four" || rank == "4") {
                return Tile::Rank::FOUR;
            } else if (rank == "five" || rank == "5") {
                return Tile::Rank::FIVE;
            } else if (rank == "six" || rank == "6") {
                return Tile::Rank::SIX;
            } else if (rank == "seven" || rank == "7") {
                return Tile::Rank::SEVEN;
            } else if (rank == "eight" || rank == "8") {
                return Tile::Rank::EIGHT;
            } else if (rank == "nine" || rank == "9") {
                return Tile::Rank::NINE;
            } else {
                throw std::invalid_argument("No valid rank");
            }
        }
        
        static Tile::Suit toSuit(std::string suit) {
            if (suit == "bamboo") {
                return Tile::Suit::BAMBOO;
            } else if (suit == "dot") {
                return Tile::Suit::DOT;
            } else if (suit == "crak") {
                return Tile::Suit::CRAK;
            } else {
                throw std::invalid_argument("No valid suit");
            }
        }
        
        static cugl::Vec2 toVector(std::string str){
            size_t commaPos = str.find(",");
            float x = std::stof(str.substr(1, commaPos));
            float y = std::stof(str.substr(commaPos + 1, str.length() - 1));
            return cugl::Vec2(x, y);
        }
        
        /**
         * Returns the string form of the current tile
         *
         * @return a string representing the rank and suit of the tile
         */
        std::string toString() const {
            return toStringRank() + " of " + toStringSuit();
        }
        
        /**
         * Sets the texture of this tile
         *
         * @param value     the texture to assign to this tile
         */
        void setTexture(const std::shared_ptr<cugl::graphics::Texture>& value){
            _texture = value;
        }
        
        /**
         * Sets the texture of a wild tile
         *
         * @param assets    the asset manager to get the texture from
         */
        void setWildTexture(const std::shared_ptr<cugl::AssetManager>& assets);
        
        
        /**
         * Method to get the tile texture
         *
         * @return the texture of the current tile
         */
        std::shared_ptr<cugl::graphics::Texture> getTileTexture(){
            return _texture;
        }
        
        /**
         * Overloading operator to directly compare two tiles
         */
        bool operator==(std::shared_ptr<Tile>& tile){
            if(this->toString() == tile->toString() && this->_id == tile->_id){
                return true;
            }
            return false;
        }
    };
    
public:
    /** Deck with all of the tiles */
    std::vector<std::shared_ptr<Tile>> deck;
    /** Vector with starting representation of deck */
    std::vector<std::shared_ptr<Tile>> startingDeck;
    /** Unsorted set containing tiles in the deck */
    std::map<std::string, std::shared_ptr<Tile>> tileMap;
    /** Grandma's favorite tiles */
    std::vector<std::shared_ptr<Tile>> grandmaTiles;
    /** Reference to texture for grandma tile text */
    std::shared_ptr<cugl::graphics::Texture> gmaTexture;
    /** Random Generator */
    cugl::Random rdTileSet;
     /** Wild tile set to draw from */
    std::vector<std::shared_ptr<Tile>> wildTiles;
    /** Number of wild tiles we have initilalized */
    int wildCount;
    /** Number of tiles we have initialized */
    int tileCount;
    /** The center of a tile */
    cugl::Vec2 _center;
    /** Pointer to next tile to be drawn */
    std::shared_ptr<Tile> nextTile;
    /** Set of tiles to be processed for networking */
    std::vector<std::shared_ptr<Tile>> tilesToJson;
    
#pragma mark -
#pragma mark Tileset Constructors
    
    /**
     * Initializes an empty deck.
     */
    TileSet() { rdTileSet.init(); }
    
    /**
     * Initializes deck to a **STARTING** representation of numbered tiles
     *
     * Only call if host
     */
    void initHostDeck();
    
    /**
     * Initializes deck to a **STARTING** representation of numbered tiles
     *
     * Only call if client
     */
    void initClientDeck(const std::shared_ptr<cugl::JsonValue>& deckJson);
    
    
#pragma mark -
#pragma mark Tileset Gameplay Handling
    
    /**
     * Shuffles the tileSet for random assignment
     *
     * **ALWAYS SHUFFLE BEFORE READING FROM DECK**
     */
    void shuffle() {
        rdTileSet.init();
        rdTileSet.shuffle(deck);
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
     * Sets the texture for all tiles in deck
     */
    void setAllTileTexture(const std::shared_ptr<cugl::AssetManager>& assets);
    
    /**
     * Sets the texture of a facedown tile
     *
     * @param assets    the asset manager to get the texture from
     */
    void setBackTextures(const std::shared_ptr<cugl::AssetManager>& assets);
    /**
     * Draws the tiles in the tileset to the screen
     */
    void draw(const std::shared_ptr<cugl::graphics::SpriteBatch>& batch, cugl::Size size, bool isHost);
    
    /**
     * Creates a cugl::JsonValue representation of the current state of the deck
     *
     * @Returns std::shared_ptr<cugl::JsonVaue>
     */
    const std::shared_ptr<cugl::JsonValue> toJson(std::vector<std::shared_ptr<Tile>> tiles);
    
    /**
     * Randomly generates a suit with type Tile::Suit
     *
     * Returns suit with type Tile::Suit
     */
    Tile::Suit randomSuit() {
        int randSuit = static_cast<int>(rdTileSet.getOpenUint64(0, 4));
        return static_cast<TileSet::Tile::Suit>(randSuit);
    };
    
    /**
     * Randomly generates a rank
     *
     * Returns a Tile::Rank
     */
    Tile::Rank randomNumber() {
        int randRank = static_cast<int>(rdTileSet.getOpenUint64(1, 11));
        return static_cast<TileSet::Tile::Rank>(randRank);
    };
    
    void clearTilesToJson(){
        tilesToJson.clear();
    }
    
    void setNextTile(std::shared_ptr<cugl::JsonValue>& nextTileJson);
    
    void updateDeck(const std::shared_ptr<cugl::JsonValue>& deckJson);
    
    std::vector<std::shared_ptr<Tile>> processTileJson(const std::shared_ptr<cugl::JsonValue>& tileJson);
    
    std::vector<std::shared_ptr<Tile>> processDeckJson(const std::shared_ptr<cugl::JsonValue>& deckJson);
    
    std::shared_ptr<cugl::JsonValue> mapToJson();
    
};

#endif /* __MJ_TILESET_H__ */
