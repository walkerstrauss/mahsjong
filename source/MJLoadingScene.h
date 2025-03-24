//
//  MJLoadingScene.h
//  Mahsjong
//
//  Created by Walker Strauss on 3/23/25.
//

#ifndef __MJ_LOADING_SCENE_H__
#define __MJ_LOADING_SCENE_H__

using namespace cugl;
using namespace cugl::scene2;
using namespace cugl::graphics;
using namespace std;

class LandingScene : public cugl::scene2::LoadingScene {
public:
    std::shared_ptr<scene2::SceneNode> after;
    
    LandingScene() : cugl::scene2::LoadingScene() {}
    
    bool fixScene(){
        if (!Scene2::initWithHint(0,720)){
            return false;
        }
        after = _assets->get<scene2::SceneNode>("load.after");
        after->setContentSize(getSize());
        after->doLayout();
        after->setPosition((Application::get()->getDisplayWidth() - 1280) / 2, after->getPosition().y);
        return true;
    }
    
    void render() override{
        _batch->begin(getCamera()->getCombined());
        std::shared_ptr<graphics::Texture> temp = Texture::getBlank();
        _batch->draw(temp, Color4(0,0,0,255), Rect(Vec2::ZERO, cugl::Application().get()->getDisplaySize()));
        scene2::LoadingScene::render();
        _batch->end();
    }
};

#endif
