#include <iostream>
#include "Server.hpp"
#include "Client.hpp"

int main() {
    std::string role;
    std::cout << "Do you want to be a server (s) or a client (c) ? :";
    std::cin  >> role;

    if(role == "s") {
        std::cout<<"Creating a server..."<<std::endl;
        Server server;
        server.run();
    }

    else {
        std::cout<<"Creating a client..."<<std::endl;
        Client client;
        client.run();
    }
    return 0;
}
