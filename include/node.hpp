#ifndef NODE_HPP
#define NODE_HPP

#include <string>
#include <vector>
#include <zmq.hpp>

class Node {
public:
    Node(int id, int total);
    void start();

private:
    int node_id;
    int total_nodes;
    zmq::context_t context;
    zmq::socket_t receiver_socket;      // Receives messages from clients/nodes
    zmq::socket_t publisher_socket;     // Publishes messages to other nodes
    std::vector<zmq::socket_t> node_sockets;  // Connections to other nodes

    void process_request(const std::string &message);
    void process_node_message(const std::string &message);
    void broadcast(const std::string &message);

    // **Add this line to declare the listen_to_nodes function**
    void listen_to_nodes();  // Function to listen for inter-node messages
};

#endif // NODE_HPP
