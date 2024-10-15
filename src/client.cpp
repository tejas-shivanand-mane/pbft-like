#include "client.hpp"
#include <iostream>
#include <sstream>

Client::Client(int id, int leader_id)
    : client_id(id), leader_id(leader_id), context(1), client_socket(context, ZMQ_REQ) {
    
    // Connect to the leader node (Node 0 in this example)
    std::ostringstream leader_address;
    leader_address << "tcp://127.0.0.1:" << 5550 + leader_id;
    client_socket.connect(leader_address.str());
    client_socket.setsockopt(ZMQ_RCVTIMEO, 3000);  // 3 seconds

}

void Client::send_request(const std::string &request) {
    std::ostringstream message_stream;
    message_stream << "REQUEST|" << client_id << "|" << request;
    
    std::string message = message_stream.str();
    std::cout << "Client " << client_id << " sending request: " << message << std::endl;

    zmq::message_t zmq_msg(message.size());
    memcpy(zmq_msg.data(), message.c_str(), message.size());
    client_socket.send(zmq_msg);




    // Wait for a reply from the node
    zmq::message_t reply;


    try {
        client_socket.recv(reply, zmq::recv_flags::none);
        std::string reply_str(static_cast<char*>(reply.data()), reply.size());
        std::cout << "Client " << client_id << " received reply: " << reply_str << std::endl;
    } catch (zmq::error_t &e) {
        std::cerr << "No reply received or timeout occurred." << std::endl;
    }



//    client_socket.recv(&reply);
//    std::string reply_str(static_cast<char*>(reply.data()), reply.size());
//    std::cout << "Client " << client_id << " received reply: " << reply_str << std::endl;

}
