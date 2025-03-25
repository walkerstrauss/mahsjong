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
#ifdef CU_TOUCH_SCREEN
_touchDown(false),
_touchKey(0)
#else
_mouseDown(false),
_mouseKey(0)
#endif
{
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
    
#ifdef CU_TOUCH_SCREEN
    Touchscreen* touch = Input::get<Touchscreen>();
    if (touch) {
        touch->addBeginListener(_touchKey, [=,this](const cugl::TouchEvent& event, bool focus) {
            this->touchPressCB(event, focus);
        });
        touch->addEndListener(_touchKey, [=,this](const cugl::TouchEvent& event, bool focus) {
            this->touchReleaseCB(event, focus);
        });
        touch->addMotionListener(_touchKey, [=,this](const cugl::TouchEvent& event, const Vec2 previous, bool focus) {
            this->dragCB(event, previous, focus);
        });
        _active = true;
    }
#else
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
#endif
    
    Keyboard* keys = Input::get<Keyboard>();
    if (keys){
        _keyboardKey = keys->acquireKey();
        keys->addKeyUpListener(_keyboardKey,[=,this](const cugl::KeyEvent& event, bool focus){
            this->keyUpCB(event,focus);
        });
        keys->addKeyDownListener(_keyboardKey, [=,this](const cugl::KeyEvent& event, bool focus){
            this->keyDownCB(event,focus);
        });
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
#ifdef CU_TOUCH_SCREEN
    _prevDown = _currDown;
    _currDown = _touchDown;

    _prevPos = _currPos;
#else
    _prevDown = _currDown;
    _currDown = _mouseDown;

    _prevPos = _currPos;
    _currPos = _mousePos;
#endif

    _prevKeyDown = _keyDown;
    if (!_keyDown) {
        _keyPressed = KeyCode::UNKNOWN;
    }
}

#pragma mark -
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

#pragma mark Touch Callbacks

void InputController::touchPressCB(const cugl::TouchEvent& event, bool focus) {
    if (!_touchDown && _touchKey != event.touch) {
        _touchDown = true;
        _touchKey = event.touch;
        _currPos = event.position;
        _wasDragging = false; 
        
        CULog("TOUCH PRESS detected at (%f, %f)", event.position.x, event.position.y);
    }
}

void InputController::touchReleaseCB(const cugl::TouchEvent& event, bool focus) {
    if (_touchDown && _touchKey == event.touch) {
        _touchDown = false;
        _touchKey = 0;
        if(_dragging == true){
            _dragging = false;
            _wasDragging = true;
        }
        
        CULog("TOUCH RELEASE detected at (%f, %f)", event.position.x, event.position.y);
    }
}

void InputController::dragCB(const cugl::TouchEvent& event, const Vec2 previous, bool focus) {
    if (_touchDown) {
        _currPos = event.position;
        _dragging = true;
        CULog("TOUCH DRAG detected at (%f, %f)", event.position.x, event.position.y);
    }
}

/**
 * Reads the input for this player and converts the result into game logic
 *
 * TODO: Please add additional input logic as seen necessary
 */
void InputController::readInput(){
}
