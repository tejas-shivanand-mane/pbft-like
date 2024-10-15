#include "client.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <client_id> <leader_id>\n";
        return 1;
    }

    int client_id = std::stoi(argv[1]);
    int leader_id = std::stoi(argv[2]);

    Client client(client_id, leader_id);

    while (true) {
        std::string request;
        std::cout << "Enter request (or 'exit' to quit): ";
        std::getline(std::cin, request);
        if (request == "exit") break;

        client.send_request(request);
    }

    return 0;
}
