

#include <iostream>

#include "bumblebee/BumbleBeeDB.h"
#include "bumblebee/common/type/Graph.h"

using namespace std;

int main(int argc, char** argv){
    bumblebee::BumbleBeeDB db;
    db.parseArgs(argc, argv);
    db.printArgs();
    db.run();
    bumblebee::Graph g(true);
    // g.addEdge("b","a",1);
    g.addEdge("a","b",1);
    g.addEdge("a","c",1);
    // g.addEdge("c","b",1);
    g.addEdge("c","e",1);
    g.addEdge("b","d",1);
    g.addEdge("d","x",1);
    std::cout << g.getDotFormat("test") << std::endl;

    auto sc = g.calculateStrongComponent();
    auto ts = g.calculateTopologicalSort();

    return 0;
}