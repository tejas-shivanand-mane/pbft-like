#include "node.hpp"
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>
#include <chrono>

// Constructor for the node class
Node::Node(int id, int total)
    : node_id(id), total_nodes(total), context(1), receiver_socket(context, ZMQ_REP), publisher_socket(context, ZMQ_PUB) {

    // Bind to a specific port for client communication (e.g., 5550 + node_id)
    std::ostringstream address;
    address << "tcp://*:" << 5550 + id;
    receiver_socket.bind(address.str());

    // Publisher socket for broadcasting to other nodes
    publisher_socket.bind("tcp://*:" + std::to_string(6000 + id));

    // Initialize sockets to subscribe to all other nodes
    for (int i = 0; i < total; ++i) {
        if (i != node_id) {
            zmq::socket_t sub_socket(context, ZMQ_SUB);
            std::ostringstream node_address;
            node_address << "tcp://127.0.0.1:" << 6000 + i;

            // Connect to each node's publisher socket
            sub_socket.connect(node_address.str());

            // Subscribe to all messages from this node
            sub_socket.setsockopt(ZMQ_SUBSCRIBE, "", 0);

            node_sockets.push_back(std::move(sub_socket));
        }
    }
}

// Start the main event loop for the PBFT node
void Node::start() {
    std::cout << "Node " << node_id << " is up and running." << std::endl;

    // Start a separate thread to listen for node-to-node messages
    std::thread listener_thread(&Node::listen_to_nodes, this);

    while (true) {
        zmq::message_t request;

        // Receive request from client or another node
        receiver_socket.recv(&request);

        std::string message(static_cast<char*>(request.data()), request.size());
        std::cout << "Node " << node_id << " received message: " << message << std::endl;

        // Parse and process the message based on type
        if (message.find("REQUEST") == 0) {
            std::cout << "Node " << node_id << " processing client request..." << std::endl;
            process_request(message);
        }

        // Reply to sender (for simplicity, sending a basic ACK message)
        std::string reply = "ACK from Node " + std::to_string(node_id);
        zmq::message_t reply_msg(reply.size());
        memcpy(reply_msg.data(), reply.c_str(), reply.size());
        receiver_socket.send(reply_msg);
    }

    // Join the listener thread before exiting
    listener_thread.join();
}

// Listen for messages from other nodes in a separate thread
void Node::listen_to_nodes() {
    while (true) {
        for (auto &socket : node_sockets) {
            zmq::message_t message;
            if (socket.recv(message, zmq::recv_flags::dontwait)) {
                std::string msg(static_cast<char*>(message.data()), message.size());
                std::cout << "Node " << node_id << " received broadcast message: " << msg << std::endl;
                process_node_message(msg);
            }
        }

        // Sleep for a short duration to avoid busy-waiting
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

// Process a client request
void Node::process_request(const std::string &message) {
    std::cout << "Node " << node_id << " processing client request: " << message << std::endl;

    // Parse client request
    std::istringstream iss(message);
    std::string type, client_id, request;
    std::getline(iss, type, '|');
    std::getline(iss, client_id, '|');
    std::getline(iss, request, '|');

    // Create a PRE-PREPARE message
    std::string pre_prepare_message = "PRE-PREPARE|" + std::to_string(node_id) + "|" + client_id + "|" + request;
    std::cout << "Node " << node_id << " broadcasting PRE-PREPARE message: " << pre_prepare_message << std::endl;

    broadcast(pre_prepare_message);
}

// Process messages from other nodes
void Node::process_node_message(const std::string &message) {
    std::cout << "Node " << node_id << " processing node-to-node message: " << message << std::endl;

    std::istringstream iss(message);
    std::string type, sender_id, client_id, request;
    std::getline(iss, type, '|');
    std::getline(iss, sender_id, '|');
    std::getline(iss, client_id, '|');
    std::getline(iss, request, '|');

    if (type == "PRE-PREPARE") {
        std::cout << "Node " << node_id << " received PRE-PREPARE from Node " << sender_id << std::endl;

        // Broadcast a PREPARE message
        std::string prepare_message = "PREPARE|" + std::to_string(node_id) + "|" + client_id + "|" + request;
        std::cout << "Node " << node_id << " broadcasting PREPARE message: " << prepare_message << std::endl;
        broadcast(prepare_message);
    } else if (type == "PREPARE") {
        std::cout << "Node " << node_id << " received PREPARE from Node " << sender_id << std::endl;

        // After receiving enough PREPARE messages, broadcast a COMMIT message
        std::string commit_message = "COMMIT|" + std::to_string(node_id) + "|" + client_id + "|" + request;
        std::cout << "Node " << node_id << " broadcasting COMMIT message: " << commit_message << std::endl;
        broadcast(commit_message);
    }
}

// Broadcast a message to all other nodes
void Node::broadcast(const std::string &message) {
    zmq::message_t zmq_msg(message.size());
    memcpy(zmq_msg.data(), message.c_str(), message.size());
    publisher_socket.send(zmq_msg, zmq::send_flags::none);

    std::cout << "Node " << node_id << " broadcasted message: " << message << std::endl;
}
