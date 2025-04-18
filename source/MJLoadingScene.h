//
//  MJLoadingScene.h
//  Mahsjong
//
//  Created by Mariia Tiulchenko on 13.04.2025.
//

#ifndef __LOADING_SCENE_H__
#define __LOADING_SCENE_H__
#include <cugl/cugl.h>

using namespace cugl;
using namespace cugl::scene2;
using namespace cugl::netcode;
using namespace std;


/**
 * A custom loading scene that adjusts positioning.
 */
class OurLoadingScene : public cugl::scene2::LoadingScene {
    
public:
    std::shared_ptr<cugl::scene2::SceneNode> _landingScene;
    
public:
    
    OurLoadingScene() : cugl::scene2::LoadingScene() {}

    /**
     * Initializes the loading scene.
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, const std::string& directory) {

        if (!LoadingScene::init(assets, directory)) {
            return false;
        }
        
        _landingScene = assets->get<scene2::SceneNode>("load");
        
        return true;
    }

    /**
     * Resizes the loading scene and repositions its elements.
     *
     * This mimics the procedure in your menu scene.
     */
    void resizeScene();
    
    void renderAfter();

};

#endif // __LOADING_SCENE_H__
