
#include <iostream>

#include "bumblebee/BumbleBeeDB.hpp"
#include "bumblebee/common/ErrorHandler.hpp"


int main(int argc, char** argv){
    try {
        bumblebee::BumbleBeeDB db;
        db.parseArgs(argc, argv);
        db.run();
    } catch (const bumblebee::BumbleBeeException& e) {
        return e.code();
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        return ERROR_GENERIC_CODE;
    }

    return 0;
}