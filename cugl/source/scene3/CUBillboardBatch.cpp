//
//  CUBillboardBatch.cpp
//  Cornell University Game Library (CUGL)
//
//  This module provides an implementation of Scene3Batch for drawing (and
//  batching) BillboardNode objects. It is only designed for those types of
//  nodes.
//
//  This class uses our standard shared-pointer architecture.
//
//  1. The constructor does not perform any initialization; it just sets all
//     attributes to their defaults.
//
//  2. All initialization takes place via init methods, which can fail if an
//     object is initialized more than once.
//
//  3. All allocation takes place via static constructors which return a shared
//     pointer.
//
//  CUGL MIT License:
//      This software is provided 'as-is', without any express or implied
//      warranty.  In no event will the authors be held liable for any damages
//      arising from the use of this software.
//
//      Permission is granted to anyone to use this software for any purpose,
//      including commercial applications, and to alter it and redistribute it
//      freely, subject to the following restrictions:
//
//      1. The origin of this software must not be misrepresented; you must not
//      claim that you wrote the original software. If you use this software
//      in a product, an acknowledgment in the product documentation would be
//      appreciated but is not required.
//
//      2. Altered source versions must be plainly marked as such, and must not
//      be misrepresented as being the original software.
//
//      3. This notice may not be removed or altered from any source distribution.
//
//  Author: Walker White
//  Version: 7/3/24 (CUGL 3.0 reorganization)
//
#include <cugl/scene3/CUBillboardBatch.h>
#include <cugl/scene3/CUBillboardShader.h>

using namespace cugl;
using namespace cugl::graphics;
using namespace cugl::scene3;

/**
 * Draws this entry with the specified shader
 *
 * @param shader    The shader to draw with
 */
void BillboardBatch::Entry::draw(const std::shared_ptr<BillboardShader>& shader) {
    auto texture   = node->getTexture();
    auto gradient = node->getGradient();
    int type = texture == nullptr ? 0 : 1;
    if (gradient != nullptr) {
        type |= 2;
    }
    shader->setStyle(type);
    
    if (texture != nullptr) {
        shader->setTexture(texture);
        texture->bind();
    } else {
        // Apple work around (else get a no-texture warning)
        texture = Texture::getBlank();
        shader->setTexture(texture);
        texture->bind();
    }
    
    shader->setModelMatrix(node->getModelMatrix());
    shader->setTextureOffset(node->getTextureOffset());
    shader->setGradient(gradient);
    node->getSpriteMesh()->draw(shader);
    
    // Guaranteed to be non-null
    texture->unbind();
}

/**
 * Initializes a new billboard batch with the given key and priority.
 *
 * This method can fail is the shader fails to compile.
 *
 * @param key       The batch key
 * @param priority  The batch priority
 *
 * @return true if initialization was successful.
 */
bool BillboardBatch::init(Uint32 key, Uint32 priority) {
    if (!Scene3Batch::init(key,priority)) {
        return false;
    }
    _shader = BillboardShader::alloc();
    return _shader != nullptr;
}

/**
 * Appends a scene node to this batch for drawing.
 *
 * The scene node will be ignored if it not an instance of {@link ObjNode}.
 * Nodes will be drawn in the order appended.
 *
 * @param node      The node to draw
 * @param transform The global transform
 */
void BillboardBatch::append(const std::shared_ptr<SceneNode>& node, const Mat4& transform) {
    std::shared_ptr<BillboardNode> obj = std::dynamic_pointer_cast<BillboardNode>(node);
    if (obj == nullptr || obj->getSpriteMesh() == nullptr) {
        return;
    }
    _entries.emplace_back(obj,transform);
}

/**
 * Draws all appended nodes.
 *
 * Nodes will be drawn in the order appended.
 *
 * @param camera    The camera to draw with
 */
void BillboardBatch::flush(const std::shared_ptr<Camera>& camera) {
    
    // Compute distances and sort
    for(auto it = _entries.begin(); it != _entries.end(); ++it) {
        it->distance = (camera->getPosition()-it->node->getWorldPosition()).lengthSquared();
    }
    std::sort(_entries.begin(), _entries.end());

    Vec3 up  = camera->getUp();
    Vec3 dir = camera->getDirection();
    Vec3 rig = dir.cross(up);

    _shader->bind();
    _shader->setCameraRight(rig);
    _shader->setCameraUp(up);
    _shader->setPerspective(camera->getCombined());
    _shader->enableCulling(true);
    _shader->enableDepthTest(true);
    _shader->enableDepthWrite(true);

    Mat4 normalmat;
    for(auto it = _entries.begin(); it != _entries.end(); ++it) {
        it->draw(_shader);
    }
    
    _shader->unbind();
    _entries.clear();
}
