#include "node.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <node_id> <total_nodes>\n";
        return 1;
    }

    int node_id = std::stoi(argv[1]);
    int total_nodes = std::stoi(argv[2]);

    Node node(node_id, total_nodes);
    node.start();

    return 0;
}
