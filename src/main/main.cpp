

#include <iostream>

#include "bumblebee/BumbleBeeDB.h"
#include "bumblebee/common/type/Graph.h"

using namespace std;

int main(int argc, char** argv){
    bumblebee::BumbleBeeDB db;
    db.parseArgs(argc, argv);
    db.printArgs();
    db.run();

    return 0;
}