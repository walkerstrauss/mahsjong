//
//  MJInputController.h
//  Mahsjong
//
//  Created by Patrick Choo on 2/18/25.
//

#ifndef __MJ_INPUT_CONTROLLER_H__
#define __MJ_INPUT_CONTROLLER_H__

/**
 * Device-independent input manager
 *
 * This class currently only supports the keyboard for control. As we progress
 * throughout the project, we will need to implement mobile controls
 */

class InputController {
    
protected:
    /** Whether the input device was successfully initialized */
    bool _active;
    /** The current touch/mouse position */
    cugl::Vec2 _currPos;
    /** The previous touch/mouse position */
    cugl::Vec2 _prevPos;
    /** Initial click/touch position  */
    cugl::Vec2 _initialPosition;

    /** Whether there is an active button/touch press */
    bool _currDown;
    /** Whether there was an active button/touch press last frame*/
    bool _prevDown;
    
    Uint64 _touchKey;
    /** The key for the mouse listeners */
    Uint32 _mouseKey;
    /** The key for the keyboard listeners */
    Uint32 _keyboardKey;
    /** The mouse position (for mice-based interfaces) */
    cugl::Vec2 _mousePos;
    /** Whether the (left) mouse button is down */
    bool _mouseDown;
    /** Whether the screen is being touched */
    bool _touchDown;
    /** Whether there is a key down */
    bool _keyDown;
    /** Whether a key was pressed last frame */
    bool _prevKeyDown;
    /** Key pressed */
    cugl::KeyCode _keyPressed;
    /** Previous key presesd */
    cugl::KeyCode _prevKeyPressed;
    
#pragma mark Input Control
public:
    
    /** Creates a new input controller with the default settings */
    InputController();
     
    /** Disposes the input controller */
    ~InputController() { dispose(); }
    
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
    bool init();
    
    /**
     * Disposes this input controller, deactivating all listeners.
     *
     * As the listeners are deactived, the user will not be able to
     * monitor input until the controller is reinitialized with the
     * {@link #init} method.
     */
    void dispose();
    
    /**
     * Updates the input controller for the latest frame.
     *
     * It might seem weird to have this method given that everything
     * is processed with call back functions.  But we need some way
     * to synchronize the input with the animation frame.  Otherwise,
     * how can we know what was the touch location *last frame*?
     * Maybe there has been no callback function executed since the
     * last frame. This method guarantees that everything is properly
     * synchronized.
     */
    void update();
    
#pragma mark Attributes
    
    /**
     * Returns true if this control is active.
     *
     * An active control is one where all of the listeners are attached
     * and it is actively monitoring input. An input controller is only
     * active if {@link #init} is called, and if {@link #dispose} is not.
     *
     * @return true if this control is active.
     */
    bool isActive() const { return _active; }
    
    /**
     * Returns the current mouse/touch position
     *
     * @return the current mouse/touch position
     */
    const cugl::Vec2& getPosition() const {
        return _currPos;
    }

    /**
     * Returns the previous mouse/touch position
     *
     * @return the previous mouse/touch position
     */
    const cugl::Vec2& getPrevious() const {
        return _prevPos;
    }
    
    /**
     * Return true if the user initiated a press this frame.
     *
     * A press means that the user is pressing (button/finger) this
     * animation frame, but was not pressing during the last frame.
     *
     * @return true if the user initiated a press this frame.
     */
    bool didPress() const {
        return !_prevDown && _currDown;
    }
    
    /**
     * Return true if the user initiated a release this frame.
     *
     * A release means that the user was pressing (button/finger) last
     * animation frame, but is not pressing during this frame.
     *
     * @return true if the user initiated a release this frame.
     */
    bool didRelease() const {
        return !_currDown && _prevDown;
    }
    
    /**
     * Return true if the user is actively pressing this frame.
     *
     * This method only checks that a press is active or ongoing.
     * It does not care when the press was initiated.
     *
     * @return true if the user is actively pressing this frame.
     */
    bool isDown() const {
        return _currDown;
    }
    
    /**
     * Method to get the Keycode for the key pressed
     *
     * @return the keycode for the pressed key
     */
    cugl::KeyCode getKeyPressed() const {
        return _keyPressed;
    }
    
    /**
     * Method that returns whether or not there is a key down this frame
     *
     * @return true if there is a key down this frame, and false otherwise
     */
    bool getKeyDown() {
        return _keyDown;
    }
    
    /**
     * Method that returns whether or not there was a key down last frame
     *
     * @return true if there was a key down last frame, and false otherwise
     */
    bool getPrevKeyDown() {
        return _prevKeyDown;
    }
    
    /** Returns the initial click/touch position */
    cugl::Vec2 getInitialPosition() {
        return _initialPosition;
    }
    
    /**
     * Method to get the keycode of the key pressed last frame. UNKNOWN if
     * no key was pressed last frame.
     *
     * @return the keycode of the key presesed last frame
     */
    cugl::KeyCode getPrevKeyPressed() const {
        return _prevKeyPressed;
    }
    
    /** Reads input from player and converts the result into game logic */
    void readInput();
    
    
#pragma mark -
#pragma mark Mouse Callbacks
private:
    /**
     * Call back to execute when a mouse button is first pressed.
     *
     * This function will rpecord a press only if the left button is pressed.
     *
     * @param event     The event with the mouse information
     * @param clicks    The number of clicks (for double clicking)
     * @param focus     Whether this device has focus (UNUSED)
     */
    void buttonDownCB(const cugl::MouseEvent& event, Uint8 clicks, bool focus);

    /**
     * Call back to execute when a mouse button is first released.
     *
     * This function will record a release for the left mouse button.
     *
     * @param event     The event with the mouse information
     * @param clicks    The number of clicks (for double clicking)
     * @param focus     Whether this device has focus (UNUSED)
     */
    void buttonUpCB(const cugl::MouseEvent& event, Uint8 clicks, bool focus);

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
    void motionCB(const cugl::MouseEvent& event, const cugl::Vec2 previous, bool focus);
    
#pragma mark Touch Callbacks
    void touchPressCB(const cugl::TouchEvent& event, bool focus);
    
    void touchReleaseCB(const cugl::TouchEvent& event, bool focus);
    
    void dragCB(const cugl::TouchEvent& event, const cugl::Vec2 previous, bool focus);

#pragma mark Keyboard Callbacks
    /**
     * Call back to execute when a key is first released
     *
     * This function records a release of any key
     *
     * @param event     The event with the keyboard information
     * @param focus     Whether this device has focus
     */
    void keyUpCB(const cugl::KeyEvent& event, bool focus) {
        if (event.keycode == _keyPressed){
            _keyDown = false;
            _keyPressed = cugl::KeyCode::UNKNOWN;
        }
    };
    
    /**
     * Call back to execute when a key is first pressed
     *
     * This function records a pressing of any key
     *
     * @param event     The event with the keyboard information
     * @param focus     Whether this device has focus
     */
    void keyDownCB(const cugl::KeyEvent& event, bool focus) {
        _keyDown = true;
        _keyPressed = event.keycode;
    };
};

#endif /* __MJ_INPUT_CONTROLLER_H__ */
