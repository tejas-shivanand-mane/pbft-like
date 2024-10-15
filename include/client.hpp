#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <zmq.hpp>

class Client {
public:
    Client(int id, int leader_id);
    void send_request(const std::string &request);

private:
    int client_id;
    int leader_id;
    zmq::context_t context;
    zmq::socket_t client_socket;
};

#endif // CLIENT_HPP
