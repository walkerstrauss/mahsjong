//
//  MJDiscardUIScene.h
//  Mahsjong
//
//  Created by Walker Strauss on 3/8/25.
//

#ifndef __MJ_DISCARD_UI_SCENE_H__
#define __MJ_DISCARD_UI_SCENE_H__

#include <cugl/cugl.h>
#include "MJTileSet.h"
#include "MJPlayer.h"

using namespace cugl;
using namespace cugl::scene2;
using namespace cugl::netcode;
using namespace std;

/**
 * This is the class responsible for drawing the discard UI to the game scene
 */
class DiscardUIScene : public cugl::scene2::Scene2 {
public:
    enum Choice {
        BACK,
        NONE
    };
    
protected:
    /** Asset manager for this game mode */
    std::shared_ptr<cugl::AssetManager> _assets;
    /** Vector of scene nodes representing labels in the tileset UI table */
    std::vector<std::shared_ptr<cugl::scene2::Label>> _labels;
    /** Reference to scene node for UI scene */
    std::shared_ptr<scene2::SceneNode> _tilesetui;
    
public:
    /** Button to exit the discard UI */
    std::shared_ptr<scene2::Button> backBtn;
    /** Key for the listener for the back button for discard UI */
    Uint32 backBtnKey;
    /** Field representing our choice from the tileset UI scene */
    Choice choice;
    /** Input contorller */
    InputController input;
    
#pragma mark -
#pragma mark Constructors
    /**
     * Method to create discard pile with default init values
     */
    DiscardUIScene(): cugl::scene2::Scene2(){}
    /**
     * Method to initialize the discard UI scene
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets);
    
    /**
     * Method to dispose of all (non-static) resources allocated to this mode.
     */
    void dispose() override;
    
#pragma mark -
#pragma mark Gameplay Handling
    /**
     * Method to reset the discard UI scene
     */
    void reset() override;
    
    virtual void setActive(bool value) override;
    
    /**
     * The method called to update the discard UI scene
     *
     * @param timestep The amount of time (in seconds) since the last frame
     */
    void update(float timestep) override;
    
    /**
     * Method to get the index of this tile's associated label in the discard UI vector of labels
     *
     * @param tile  the tile whose label we need in _labels
     * @return an int representing the index of this tile's discard UI label
     */
    int getLabelIndex(std::shared_ptr<TileSet::Tile> tile);
    
    /**
     * Method to increment discard UI label corresponding to tile passed as argument
     *
     * @param tile  the tile to increment in the discard UI
     * @return true if update was successful, and false otherwise
     */
    bool incrementLabel(std::shared_ptr<TileSet::Tile> tile);
    
    /**
     * Method to decrement discard UI label corresponding to tile passed as argument
     *
     * @param tile  the tile to increment in the discard UI
     * @return true if update was successful, and false otherwise
     */
    bool decrementLabel(std::shared_ptr<TileSet::Tile> tile);
    
    /**
     * Method to "select" label for discard UI command tile drawing handling.
     * If label text is "0", no tile is selected and empty vector is returned.
     *
     * @param mousePos  the current position of the input for selection
     * @return a vector containing one tile is successful, and an empty vector otherwise
     */
    std::vector<std::shared_ptr<TileSet::Tile>> selectTile(cugl::Vec2& currPos);
    
    
};

#endif /*__MJ_DISCARD_UI_SCENE_H__*/
