

#include <iostream>

#include "bumblebee/BumbleBeeDB.hpp"


int main(int argc, char** argv){
    bumblebee::BumbleBeeDB db;
    db.parseArgs(argc, argv);
    db.run();

    return 0;
}