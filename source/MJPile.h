//
//  MJPile.h
//  Mahsjong
//
//  Created by Patrick Choo on 2/18/25.
//

#ifndef __MJ_PILE_H__
#define __MJ_PILE_H__

#include <cugl/cugl.h>
#include "MJTileSet.h"
#include "MJInputController.h"

/**
 * This is the class intializing and handling the pile.
 *
 * TODO: Please implement pile functionality
 */

class Pile {
private:

	//Number of possible tiles in pile
	int _pileSize;

public:

	//Tiles in the pile
	std::vector<std::vector<std::shared_ptr<TileSet::Tile>>> _pile;

	//Tiles drawn from pile
	std::vector<std::shared_ptr<TileSet::Tile>> _draw;

	//Where we get our deck
	std::shared_ptr<TileSet> _tileSet;

	//Stores the location of our pair in the pile
	std::vector<cugl::Vec2> _pairs;

public:

	//How many tiles can be in pile
	int getPileSize();

	//How many tiles are currently visible
	int getVisibleSize();

	//Init the first layer of pile with all of the tiles. Returns false if we ran out of tiles in deck
	bool initPile(int size, std::shared_ptr<TileSet> tileSet);

	bool createPile();

	//Return a vector of tiles to be collected from pile and deck if need be
	std::vector<std::shared_ptr<TileSet::Tile>> tilesDrawn(int number_of_tiles);

	//Return a vector of tiles that were selected as a pair
	std::vector<std::shared_ptr<TileSet::Tile>> pairTile();

	//Draws the pile
	void draw(const std::shared_ptr<cugl::graphics::SpriteBatch>& batch, cugl::Size size, cugl::Vec2 position);

};

#endif /* __MJ_PILE_H__ */



