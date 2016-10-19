#ifndef COMMUNICATOR_HPP_INCLUDED
#define COMMUNICATOR_HPP_INCLUDED

#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <SFML/Network.hpp>

#include "NetworkProtocol.hpp"

class PostMan {

public:

    struct ConnectionInfo {
        sf::TcpSocket*  PointerToTCPSocket;
        sf::Packet      UpPacket;
        sf::Packet      DownPacket;

        ConnectionInfo  (sf::TcpSocket* socketAddress): PointerToTCPSocket(socketAddress) {}

        ConnectionInfo()=default;
    };


    int                 connectTo                   (std::string, int, sf::Time);
    bool                transferDownloadedPackages            (std::map<int,sf::Packet*>&);
    std::vector<int>    getActiveConnections        (bool);
    int                 listenToNewConnection       (int);
    bool                downloadPackages         ();
    bool                predownloadPackage          (int,sf::Packet*);
    bool                uploadPacket                (int,sf::Packet*);
    bool                uploadPacketToAllConnected  (sf::Packet*);



    template <typename T>
    void resizeMapByActiveConnection(std::map<int,T>& ToBeResized, bool extraUpdate) {
        std::vector<int> Resizer;
        Resizer=getActiveConnections(extraUpdate);
        for (auto& item:Resizer) {
            if(ToBeResized.find(item)==ToBeResized.end()) {
                ToBeResized.emplace(std::make_pair(item,T()));
            }
        }
        for(auto it=ToBeResized.begin(); it!=ToBeResized.end();) {
            if(std::find(Resizer.cbegin(),Resizer.cend(),it->first)==Resizer.cend() && it->first!=0) {
                it=ToBeResized.erase(it);
            } else {
                ++it;
            }
        }
    }

    void resizeMapByActiveConnection2(std::map<int,sf::Packet*>& ToBeResized, bool extraUpdate) {
        std::vector<int> Resizer;
        Resizer=getActiveConnections(extraUpdate);
        for (auto& item:Resizer) {
            if(ToBeResized.find(item)==ToBeResized.end()) {
                ToBeResized.emplace(std::make_pair(item,nullptr));
            }
        }
        for(auto it=ToBeResized.begin(); it!=ToBeResized.end();) {
            if(std::find(Resizer.cbegin(),Resizer.cend(),it->first)==Resizer.cend() && it->first!=0) {
                it=ToBeResized.erase(it);
            } else {
                ++it;
            }
        }
    }

    void debug_checkDownSizes();

private:

    bool                            predownloadPackage (std::map<int, ConnectionInfo>::iterator&,sf::Packet*);

    std::map<int, ConnectionInfo>   ActiveConnectionsMap;
    std::map<int, ConnectionInfo>   ConnectionsMap;
    sf::TcpListener                 ConnectionListener;
    int                             HighestConnection=0;
    std::list<sf::TcpSocket>        ListOfSocketsToClients;
    sf::TcpSocket                   SocketToServer;
};
#endif // COMMUNICATOR_HPP_INCLUDED
