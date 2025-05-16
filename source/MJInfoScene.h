//
//  MJInfoScene.h
//  Mahsjong
//
//  Created by Walker Strauss on 4/21/25.
//

#ifndef __MJ_INFO_SCENE_H__
#define __MJ_INFO_SCENE_H__

#include <cugl/cugl.h>
#include <vector>

using namespace cugl;
using namespace cugl::scene2;
using namespace cugl::graphics;

class InfoScene : public Scene2 {
protected:
    
    std::shared_ptr<AssetManager> _assets;
    std::shared_ptr<SceneNode> _infoscene;
    
    std::shared_ptr<Button> _eachTurn1;
    std::shared_ptr<Button> _howWin1;
    std::shared_ptr<Button> _celestial1;
    std::shared_ptr<Button> _close1;
    
    std::shared_ptr<Button> _eachTurn2;
    std::shared_ptr<Button> _howWin2;
    std::shared_ptr<Button> _celestial2;
    std::shared_ptr<Button> _close2;
    
    std::shared_ptr<Button> _eachTurn3;
    std::shared_ptr<Button> _howWin3;
    std::shared_ptr<Button> _celestial3;
    std::shared_ptr<Button> _close3;
    
    std::shared_ptr<SceneNode> _menu1;
    std::shared_ptr<SceneNode> _menu2;
    std::shared_ptr<SceneNode> _menu3;
    
    std::shared_ptr<SceneNode> _infoTurn;
    std::shared_ptr<SceneNode> _infoWin;
    std::shared_ptr<SceneNode> _infoTile;
    
    bool _pageChanged;
public:
    enum Choice {
        NONE,
        BACK
    };

    enum PrevScene {
        GAMER,
        TUTORIALER,
        NEITHER
    };
    
    enum InfoPage {
        NO_PAGE,
        TURN,
        WIN,
        TILE
    };
    
    Choice choice;
    PrevScene scene;
    InfoPage currPage;
    InfoPage nextPage;
#pragma mark Constructors
#pragma mark -
    InfoScene() : Scene2 (){}
    
    bool init(const std::shared_ptr<AssetManager>& assets);
#pragma mark Gameplay Handling
#pragma mark -
    void update(float timestep) override{
        if (_pageChanged){
            switchPage();
            _pageChanged = false;
        }
    }
    void setButtonActive(std::shared_ptr<Button> button, bool value);
    void getNodes();
    void addListeners();
    void switchPage();
};
#endif
