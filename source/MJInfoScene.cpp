//
//  MJInfoScene.cpp
//  Mahsjong
//
//  Created by Walker Strauss on 4/21/25.
//

#include <cugl/cugl.h>
#include <iostream>
#include <sstream>
#include "MJInfoScene.h"

using namespace cugl;
using namespace cugl::graphics;
using namespace cugl::scene2;

bool InfoScene::init(const std::shared_ptr<AssetManager>& assets){
    if (assets == nullptr){
        return false;
    } else if (!Scene2::initWithHint(0,720)){
        return false;
    }
    
    _assets = assets;
    
    _infoscene = _assets->get<SceneNode>("info");
    Size dimen = getSize();
    _infoscene->setContentSize(dimen);
    _infoscene->getChild(0)->setContentSize(dimen);
    _infoscene->doLayout();
    
    choice = NONE;
    getNodes();
    addListeners();
    
    return true;
}

void InfoScene::setActive(bool value){
    return;
}

void InfoScene::render(){
    if (_batch == nullptr){
        CULog("no sprite batch for the information scene");
        return;
    }
    
    return;
}

void InfoScene::setButtonActive(std::shared_ptr<Button> button, bool value){
    if (button == nullptr){
        return;
    }
    if (button->isActive() != value){
        if (value){
            button->activate();
        } else {
            button->deactivate();
        }
    }
    
    button->setVisible(value);
}

void InfoScene::getNodes(){
    _menu1 = _assets->get<SceneNode>("info.infoscene.menu1");
    _menu2 = _assets->get<SceneNode>("info.infoscene.menu2");
    _menu3 = _assets->get<SceneNode>("info.infoscene.menu3");
    
    _eachTurn1 = std::dynamic_pointer_cast<Button>(_assets->get<SceneNode>("info.infoscene.menu1.menuButton.eachTurn"));
    _howWin1 = std::dynamic_pointer_cast<Button>(_assets->get<SceneNode>("info.infoscene.menu1.menuButton.win"));
    _celestial1 = std::dynamic_pointer_cast<Button>(_assets->get<SceneNode>("info.infoscene.menu1.menuButton.special"));
    _close1 = std::dynamic_pointer_cast<Button>(_assets->get<SceneNode>("info.infoscene.menu1.closeButton"));
    
    _eachTurn2 = std::dynamic_pointer_cast<Button>(_assets->get<SceneNode>("info.infoscene.menu1.menuButton.eachTurn"));
    _howWin2 = std::dynamic_pointer_cast<Button>(_assets->get<SceneNode>("info.infoscene.menu1.menuButton.win"));
    _celestial2 = std::dynamic_pointer_cast<Button>(_assets->get<SceneNode>("info.infoscene.menu1.menuButton.special"));
    _close2 = std::dynamic_pointer_cast<Button>(_assets->get<SceneNode>("info.infoscene.menu1.closeButton"));
    
    _eachTurn3 = std::dynamic_pointer_cast<Button>(_assets->get<SceneNode>("info.infoscene.menu1.menuButton.eachTurn"));
    _howWin3 = std::dynamic_pointer_cast<Button>(_assets->get<SceneNode>("info.infoscene.menu1.menuButton.win"));
    _celestial3 = std::dynamic_pointer_cast<Button>(_assets->get<SceneNode>("info.infoscene.menu1.menuButton.special"));
    _close3 = std::dynamic_pointer_cast<Button>(_assets->get<SceneNode>("info.infoscene.menu1.closeButton"));
    
    _infoTurn = _assets->get<SceneNode>("info.infoscene.infoTurn");
    _infoWin = _assets->get<SceneNode>("info.infoscene.infoWin");
    _infoTile = _assets->get<SceneNode>("info.infoscene.infoTile");
}

void InfoScene::addListeners(){
    _howWin1->addListener([this](const std::string&name, bool down){
        if (!down){
            
        }
    });
    _celestial1->addListener([this](const std::string&name, bool down){
        if (!down){
            
        }
    });
    _close1->addListener([this](const std::string&name, bool down){
        if (!down){
            
        }
    });
    
    _eachTurn2->addListener([this](const std::string&name, bool down){
        if (!down){
            
        }
    });
    _celestial2->addListener([this](const std::string&name, bool down){
        if (!down){
            
        }
    });
    _close2->addListener([this](const std::string&name, bool down){
        if (!down){
            
        }
    });
    
    _eachTurn3->addListener([this](const std::string&name, bool down){
        if (!down){
            
        }
    });
    _howWin3->addListener([this](const std::string&name, bool down){
        if (!down){
            
        }
    });
    _close3->addListener([this](const std::string&name, bool down){
        if (!down){
            
        }
    });
}

void InfoScene::switchPage(InfoPage page){
    switch (currPage){
        case TURN:
            _menu1->setVisible(false);
            setButtonActive(_eachTurn1, false);
            setButtonActive(_howWin1, false);
            setButtonActive(_celestial1, false);
            setButtonActive(_close1, false);
            break;
        case WIN:
            _menu2->setVisible(false);
            setButtonActive(_eachTurn2, false);
            setButtonActive(_howWin2, false);
            setButtonActive(_celestial2, false);
            setButtonActive(_close2, false);
            break;
        case TILE:
            _menu3->setVisible(false);
            setButtonActive(_eachTurn3, false);
            setButtonActive(_howWin3, false);
            setButtonActive(_celestial3, false);
            setButtonActive(_close3, false);
            break;
        default:
            break;
    }
    switch (page) {
        case TURN:
            _menu1->setVisible(true);
            setButtonActive(_eachTurn1, true);
            setButtonActive(_howWin1, true);
            setButtonActive(_celestial1, true);
            setButtonActive(_close1, true);
            currPage = TURN;
            break;
        case WIN:
            _menu2->setVisible(true);
            setButtonActive(_eachTurn2, true);
            setButtonActive(_howWin2, true);
            setButtonActive(_celestial2, true);
            setButtonActive(_close2, true);
            currPage = WIN;
            break;
        case TILE:
            _menu1->setVisible(true);
            setButtonActive(_eachTurn3, true);
            setButtonActive(_howWin3, true);
            setButtonActive(_celestial3, true);
            setButtonActive(_close3, true);
            currPage = TILE;
            break;
        default:
            break;
    }
}
