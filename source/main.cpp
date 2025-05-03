//
//  main.cpp
//  Mahsjong
//
//  Created by Patrick Choo on 2/17/25.
//

#include "MJApp.h"

using namespace cugl;
 
/** Our main entry point for Mah's Jong
 
    This class creates the application and runs until its done.
    Do not modify anything below the marked line */

int main(int argc, char * argv[]){
    MahsJongApp app;
    
    app.setName("Mah's Jong");
    app.setHighDPI(true);
    app.setFPS(60.0f);
    
//    // Can adjust this for screen size
//    app.setDisplaySize(1560, 720);
//
    // DO NOT MODIFY ANYTHING BELOW THIS LINE
    if(!app.init()){
        return 1;
    }
    
    app.onStartup();
    while (app.step());
    app.onShutdown();
    
    exit(0); //For mobile shutdown
    return 0;
}
