#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>

#include <SFML/Network.hpp>
#include <SFML/System.hpp>

#include "PostMan.hpp"
#include "NetworkProtocol.hpp"

struct Client {

    sf::Packet UploadPacket;
    sf::Packet DownloadPacket;

    int ConnectionID=0;

    PostMan myCommunicator;

    bool keepRunning=true;
    bool hasToConnect=true;
    int Port;
    std::string IPAddress;

    bool requestNewEntryField=true;

    void run();
    void readInputs();
    void tryToConnect(std::string const &, unsigned int const &);
    void init();
    void printContent(sf::Packet&);
};

#endif // CLIENT_H
