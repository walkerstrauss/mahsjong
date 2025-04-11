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
protected:
    /** Asset manager for this game mode */
    std::shared_ptr<cugl::AssetManager> _assets;
    /** Vector of scene nodes representing labels for each tile type */
    std::vector<std::shared_ptr<cugl::scene2::Label>> _labels;

public:
    /** Button to exit the discard UI – for now in game scene*/
    std::shared_ptr<scene2::Button> backBtn;
    /** Key for the back button listener – for now in game scene*/
    Uint32 backBtnKey;
    /** Input controller */
    InputController input;

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

    /**
     * Increments the label corresponding to the given tile.
     *
     * @param tile The tile to increment
     * @return true if successful
     */
    bool incrementLabel(std::shared_ptr<TileSet::Tile> tile);

    /**
     * Decrements the label corresponding to the given tile.
     *
     * @param tile The tile to decrement
     * @return true if successful
     */
    bool decrementLabel(std::shared_ptr<TileSet::Tile> tile);

    /**
     * Selects a tile from the discard UI by position.
     *
     * @param currPos The current input position
     * @return A vector with one tile if selected, or empty if none
     */
    std::vector<std::shared_ptr<TileSet::Tile>> selectTile(cugl::Vec2& currPos);
};

#endif /* __MJ_DISCARD_UI_NODE_H__ */
