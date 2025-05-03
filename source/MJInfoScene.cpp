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
#include "MJAudioController.h"

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
    //AudioController::getInstance().init(_assets);
    
    _infoscene = _assets->get<SceneNode>("info");
    Size dimen = getSize();
    _infoscene->setContentSize(dimen);
    _infoscene->getChild(0)->setContentSize(dimen);
    _infoscene->doLayout();
    
    choice = NONE;
    getNodes();
    addListeners();
    currPage = NO_PAGE;
    nextPage = TURN;
    _pageChanged = true;
    addChild(_infoscene);
    return true;
}

void InfoScene::setActive(bool value){
    return;
}

void InfoScene::setButtonActive(std::shared_ptr<Button> button, bool value){
    if (button == nullptr){
        return;
    }
    if (value && !button->isActive()){
        button->activate();
    } else if (!value && button->isActive()) {
        button->deactivate();
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
    _close1 = std::dynamic_pointer_cast<Button>(_assets->get<SceneNode>("info.infoscene.menu1.buttonClose"));
    
    _eachTurn2 = std::dynamic_pointer_cast<Button>(_assets->get<SceneNode>("info.infoscene.menu2.menuButton.eachTurn"));
    _howWin2 = std::dynamic_pointer_cast<Button>(_assets->get<SceneNode>("info.infoscene.menu2.menuButton.win"));
    _howWin2->setVisible(false);
    _celestial2 = std::dynamic_pointer_cast<Button>(_assets->get<SceneNode>("info.infoscene.menu2.menuButton.special"));
    _close2 = std::dynamic_pointer_cast<Button>(_assets->get<SceneNode>("info.infoscene.menu2.buttonClose"));
    
    _eachTurn3 = std::dynamic_pointer_cast<Button>(_assets->get<SceneNode>("info.infoscene.menu3.menuButton.eachTurn"));
    _howWin3 = std::dynamic_pointer_cast<Button>(_assets->get<SceneNode>("info.infoscene.menu3.menuButton.win"));
    _celestial3 = std::dynamic_pointer_cast<Button>(_assets->get<SceneNode>("info.infoscene.menu3.menuButton.special"));
    _celestial3->setVisible(false);
    _close3 = std::dynamic_pointer_cast<Button>(_assets->get<SceneNode>("info.infoscene.menu3.buttonClose"));
    
    _infoTurn = _assets->get<SceneNode>("info.infoscene.infoTurn");
    _infoWin = _assets->get<SceneNode>("info.infoscene.infoWin");
    _infoTile = _assets->get<SceneNode>("info.infoscene.infoTile");
}

void InfoScene::addListeners(){
    _howWin1->addListener([this](const std::string&name, bool down){
        if (!down){
            nextPage = WIN;
            _pageChanged = true;
            AudioController::getInstance().playSound("Select", false);
        }
    });
    _celestial1->addListener([this](const std::string&name, bool down){
        if (!down){
            nextPage = TILE;
            _pageChanged = true;
            AudioController::getInstance().playSound("Select", false);
            
        }
    });
    _close1->addListener([this](const std::string&name, bool down){
        if (!down){
            choice = BACK;
            AudioController::getInstance().playSound("Done", false);
        }
    });
    _eachTurn2->addListener([this](const std::string&name, bool down){
        if (!down){
            nextPage = TURN;
            _pageChanged = true;
            AudioController::getInstance().playSound("Select", false);
        }
    });
    _eachTurn2->setVisible(false);
    _celestial2->addListener([this](const std::string&name, bool down){
        if (!down){
            nextPage = TILE;
            _pageChanged = true;
            AudioController::getInstance().playSound("Select", false);
        }
    });
    _close2->addListener([this](const std::string&name, bool down){
        if (!down){
            choice = BACK;
            AudioController::getInstance().playSound("Done", false);
        }
    });
    _close2->setVisible(false);
    _eachTurn3->addListener([this](const std::string&name, bool down){
        if (!down){
            nextPage = TURN;
            _pageChanged = true;
            AudioController::getInstance().playSound("Select", false);
        }
    });
    _eachTurn3->setVisible(false);
    _howWin3->addListener([this](const std::string&name, bool down){
        if (!down){
            nextPage = WIN;
            _pageChanged = true;
            AudioController::getInstance().playSound("Select", false);
        }
    });
    _howWin3->setVisible(false);
    _close3->addListener([this](const std::string&name, bool down){
        if (!down){
            choice = BACK;
            AudioController::getInstance().playSound("Done", false);
        }
    });
    _close3->setVisible(false);
}

void InfoScene::switchPage(){
    if (currPage == nextPage){
        nextPage = NO_PAGE;
        return;
    } else if (nextPage == NO_PAGE){
        return;
    }
    switch (currPage){
        case TURN:
            _menu1->setVisible(false);
            _infoTurn->setVisible(false);
            setButtonActive(_eachTurn1, false);
            setButtonActive(_howWin1, false);
            setButtonActive(_celestial1, false);
            setButtonActive(_close1, false);
            break;
        case WIN:
            _menu2->setVisible(false);
            _infoWin->setVisible(false);
            setButtonActive(_eachTurn2, false);
            setButtonActive(_howWin2, false);
            setButtonActive(_celestial2, false);
            setButtonActive(_close2, false);
            break;
        case TILE:
            _menu3->setVisible(false);
            _infoTile->setVisible(false);
            setButtonActive(_eachTurn3, false);
            setButtonActive(_howWin3, false);
            setButtonActive(_celestial3, false);
            setButtonActive(_close3, false);
            break;
        case NO_PAGE:
            _menu2->setVisible(false);
            _infoWin->setVisible(false);
            setButtonActive(_eachTurn2, false);
            setButtonActive(_howWin2, false);
            setButtonActive(_celestial2, false);
            setButtonActive(_close2, false);
            _menu3->setVisible(false);
            _infoTile->setVisible(false);
            setButtonActive(_eachTurn3, false);
            setButtonActive(_howWin3, false);
            setButtonActive(_celestial3, false);
            setButtonActive(_close3, false);
            break;
    }
    switch (nextPage) {
        case TURN:
            _menu1->setVisible(true);
            _infoTurn->setVisible(true);
            setButtonActive(_eachTurn1, true);
            setButtonActive(_howWin1, true);
            setButtonActive(_celestial1, true);
            setButtonActive(_close1, true);
            currPage = TURN;
            nextPage = NO_PAGE;
            break;
        case WIN:
            _menu2->setVisible(true);
            _infoWin->setVisible(true);
            setButtonActive(_eachTurn2, true);
            setButtonActive(_howWin2, true);
            setButtonActive(_celestial2, true);
            setButtonActive(_close2, true);
            currPage = WIN;
            nextPage = NO_PAGE;
            break;
        case TILE:
            _menu3->setVisible(true);
            _infoTile->setVisible(true);
            setButtonActive(_eachTurn3, true);
            setButtonActive(_howWin3, true);
            setButtonActive(_celestial3, true);
            setButtonActive(_close3, true);
            currPage = TILE;
            nextPage = NO_PAGE;
            break;
        default:
            break;
    }
}
