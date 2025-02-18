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
private:
    /**
     * TODO: Please fill with input states
     */
    
public:
    /**
     * TODO: Please fill with methods you may need for state reading
     */
    
    
    /** Creates a new input controller with the default settings */
    InputController();
     
    /** Disposes the input controller */
    ~InputController() {}
    
    /** Reads input from player and converts the result into game logic */
    void readInput();
};

#endif /* __MJ_INPUT_CONTROLLER_H__ */
