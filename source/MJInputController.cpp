//
//  MJInputController.cpp
//  Mahsjong
//
//  Created by Patrick Choo on 2/18/25.
//

#include <cugl/cugl.h>
#include "MJInputController.h"

using namespace cugl;


InputController::InputController() :
_active(false),
_currDown(false),
_prevDown(false),
_mouseDown(false),
_mouseKey(0) {
}

/**
 * Initializes the control to support mouse or touch.
 *
 * This method attaches all of the listeners. It tests which
 * platform we are on (mobile or desktop) to pick the right
 * listeners.
 *
 * This method will fail (return false) if the listeners cannot
 * be registered or if there is a second attempt to initialize
 * this controller
 *
 * @return true if the initialization was successful
 */

bool InputController::init() {
    Mouse* mouse = Input::get<Mouse>();
    if (mouse) {
        mouse->setPointerAwareness(Mouse::PointerAwareness::DRAG);
        _mouseKey = mouse->acquireKey();
        mouse->addPressListener(_mouseKey,[=,this](const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
            this->buttonDownCB(event,clicks,focus);
        });
        mouse->addReleaseListener(_mouseKey,[=,this](const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
            this->buttonUpCB(event,clicks,focus);
        });
        mouse->addDragListener(_mouseKey,[=,this](const cugl::MouseEvent& event, const Vec2 previous, bool focus) {
            this->motionCB(event,previous,focus);
        });
        _active = true;
    }
    
    return _active;
}

/**
 * Disposes this input controller, deactivating all listeners.
 *
 * As the listeners are deactived, the user will not be able to
 * monitor input until the controller is reinitialized with the
 * {@link #init} method.
 */
void InputController::dispose() {
    if (_active) {
        Mouse* mouse = Input::get<Mouse>();
        mouse->removePressListener(_mouseKey);
        mouse->removeReleaseListener(_mouseKey);
        mouse->removeDragListener(_mouseKey);
        mouse->setPointerAwareness(Mouse::PointerAwareness::BUTTON);
        _active = false;
    }
}

/**
 * Updates the input controller for the latest frame.
 */
void InputController::update() {
    _prevDown = _currDown;
    _currDown = _mouseDown;
    
    _prevPos = _currPos;
    _currPos = _mousePos;
}

#pragma mark Mouse Callbacks
/**
 * Call back to execute when a mouse button is first pressed.
 *
 * This function will record a press only if the left button is pressed.
 *
 * @param event     The event with the mouse information
 * @param clicks    The number of clicks (for double clicking)
 * @param focus     Whether this device has focus (UNUSED)
 */
void InputController::buttonDownCB(const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
    if (!_mouseDown && event.buttons.hasLeft()) {
        _mouseDown = true;
        _mousePos = event.position;
    }
}

/**
 * Call back to execute when a mouse button is first released.
 *
 * This function will record a release for the left mouse button.
 *
 * @param event     The event with the mouse information
 * @param clicks    The number of clicks (for double clicking)
 * @param focus     Whether this device has focus (UNUSED)
 */
void InputController::buttonUpCB(const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
    if (_mouseDown && event.buttons.hasLeft()) {
        _mouseDown = false;
    }
}

/**
 * Call back to execute when the mouse moves.
 *
 * This input controller sets the pointer awareness only to monitor a mouse
 * when it is dragged (moved with button down), not when it is moved. This
 * cuts down on spurious inputs. In addition, this method only pays attention
 * to drags initiated with the left mouse button.
 *
 * @param event     The event with the mouse information
 * @param previous  The previously reported mouse location
 * @param focus     Whether this device has focus (UNUSED)
 */
void InputController::motionCB(const cugl::MouseEvent& event, const Vec2 previous, bool focus) {
    if (_mouseDown) {
        _mousePos = event.position;
    }
}

/**
 * Reads the input for this player and converts the result into game logic
 *
 * TODO: Please add additional input logic as seen necessary
 */

void InputController::readInput(){
}
