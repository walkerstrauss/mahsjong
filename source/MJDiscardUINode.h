//
//  MJDiscardUINode.h
//  Mahsjong
//
//  Created by Walker Strauss on 4/11/25.
//

#ifndef __MJ_DISCARD_UI_NODE_H__
#define __MJ_DISCARD_UI_NODE_H__

#include <cugl/cugl.h>
#include "MJTileSet.h"
#include "MJPlayer.h"

using namespace cugl;
using namespace cugl::scene2;
using namespace std;

/**
 * This node is responsible for displaying the discard UI within the game scene.
 */
class DiscardUINode : public cugl::scene2::SceneNode {
public:
    /** The state of the DiscardUINode */
    enum State {
        /** Idle State */
        IDLE,
        /** DiscardUI active */
        ON,
        /** DiscardUI not active */
        OFF
    };
protected:
    /** Asset manager for this game mode */
    std::shared_ptr<cugl::AssetManager> _assets;
    /** Vector of scene nodes representing labels for each tile type */
    std::vector<std::shared_ptr<cugl::scene2::Label>> _labels;
    /** Scene node for the tile images */
    std::shared_ptr<SceneNode> _tilesNode;
    /** The current state of DiscardUINode */
    State _state;

public:
    /** Button to exit the discard UI – for now in game scene*/
    std::shared_ptr<scene2::Button> backBtn;
    /** Key for the back button listener – for now in game scene*/
    Uint32 backBtnKey;
    /** Input controller */
    InputController input;
    /** The root scene node */
    std::shared_ptr<SceneNode> _root;

#pragma mark -
#pragma mark Constructors

    DiscardUINode() : cugl::scene2::SceneNode() {}
    
    /**
     * Initializes the discard UI node from assets.
     *
     * @param assets The asset manager for loading UI layout and resources
     * @return true if initialization was successful
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets);

    /**
     * Disposes of any resources allocated to this node.
     */
    void dispose() override;

#pragma mark -
#pragma mark UI Updates

    /**
     * Resets the discard UI labels and internal state.
     */
    void reset();

    /**
     * Updates the UI (if needed per frame).
     *
     * @param timestep Time elapsed since last frame
     */
    void update(float timestep);

    /**
     * Gets the index of the label corresponding to the given tile.
     *
     * @param tile The tile whose label we want
     * @return The index in the _labels vector
     */
    int getLabelIndex(std::shared_ptr<TileSet::Tile> tile);
    
    /** Returns the suit and rank of the tile at the given index */
    std::pair<TileSet::Tile::Suit, TileSet::Tile::Rank> tileFromIndex(int index) {
        TileSet::Tile::Suit suit = static_cast<TileSet::Tile::Suit>(index / 9 + 1);
        TileSet::Tile::Rank rank = static_cast<TileSet::Tile::Rank>(index % 9 + 1);
        return std::make_pair(suit, rank);
    }

    /**
     * Increments the label corresponding to the given tile.
     *
     * @param tile The tile to increment
     * @return true if successful
     */
    bool incrementLabel(int index);

    /**
     * Decrements the label corresponding to the given tile.
     *
     * @param tile The tile to decrement
     * @return true if successful
     */
    bool decrementLabel(int index);
    
    /**
     * Updates the label with the given discard pile
     */
    void updateLabels(std::vector<std::shared_ptr<TileSet::Tile>> discardPile);


    
    /** Gets the current state of this scene node */
    State getState() {
        return _state;
    }
    
    /** Sets the current state of this scene node */
    void setState(State state) {
        _state = state;
    }
    
    /** Sets this scene node as active */
    void setDiscardUIActive(bool active);
    
    /** Returns the index of the tile at the given mouse position in the discard ui*/
    int getClickedTile(const Vec2& mousePos) const {
        Vec2 localPos = _tilesNode->worldToNodeCoords(mousePos);

        for (int i = 1; i <= 27; ++i) {
            auto tileNode = _tilesNode->getChildByName(std::to_string(i));
            int count = std::stoi(_labels[i - 1]->getText());
            Rect bounds = tileNode->getBoundingBox();
            CULog("localPos: %s", localPos.toString().c_str());
            CULog("bounds: %s", bounds.toString().c_str());

            if (bounds.contains(localPos) && count > 0) {
                return i - 1;
            }
        }
        return -1;
    }

};

#endif /* __MJ_DISCARD_UI_NODE_H__ */
