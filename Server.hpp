#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <list>
#include <SFML/Network.hpp>
#include "PostMan.hpp"
#include "NetworkProtocol.hpp"

#include "Generic.hpp"

//Game specific:
#include "GameController.hpp"

struct Server {

    struct SVConnInfo {
        std::string alias;
        sf::Packet DownPacket;
        GameController GController;
    };

    PostMan myCommunicator;

    Timer timer1;
    Timer timer2;

    sf::Packet DownPacket;

    std::vector<int> ConnectionsVector;
    std::map<int, SVConnInfo> SVConnectionsMap;

    std::map<int,sf::Packet> ClientIDToDownPacket;

    int ServerPort;
    bool keepRunning=true;

    void run();
    void readInputs();
    void shutdown();
    void init();

    void VectorToMap();
};

#endif // SERVER_H
