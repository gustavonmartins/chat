#include "PostMan.hpp"

int PostMan::connectTo(std::string _IPAddress, int _Port, sf::Time Timeout=sf::Time::Zero) {
    std::cout<<"Tryig to connect... Socket is blocking and will unblocked after sucessfull connection"<<std::endl;
    sf::TcpSocket::Status status=SocketToServer.connect(_IPAddress,_Port, Timeout);
    if (status==sf::Socket::Done) {
        std::cout<<"Connected! "<<std::endl;
        SocketToServer.setBlocking(false);
        ConnectionsMap.insert(std::make_pair(HighestConnection,ConnectionInfo(&SocketToServer)));
        ActiveConnectionsMap.insert(std::make_pair(HighestConnection,ConnectionInfo(&SocketToServer)));

        ++HighestConnection;
        return HighestConnection;
    } else if (status==sf::Socket::NotReady) {
        std::cout<<"NotReady! "<<std::endl;
        return 0;
    } else if (status==sf::Socket::Partial) {
        std::cout<<"Partial! "<<std::endl;
        return 0;
    } else if (status==sf::Socket::Disconnected) {
        std::cout<<"Disconnected! "<<std::endl;
        return 0;
    } else if (status==sf::Socket::Error) {
        std::cout<<"Error! "<<std::endl;
        return 0;
    } else {
        std::cout<<"Trying to connect socket produced an unknown error "<<std::endl;
        return 0;
    }
}

int PostMan::listenToNewConnection(int Port) {
    static bool alreadyListening=false;

    if (!alreadyListening) {

        sf::TcpListener::Status StatusOfNewListening=ConnectionListener.listen(Port);
        if (StatusOfNewListening==sf::TcpListener::Done) {
            std::cout<<"I am listening!"<<std::endl;
            ConnectionListener.setBlocking(false);
            alreadyListening=true;
        } else {
            std::cout<<"Listening failed!"<<std::endl;
            alreadyListening=false;
            return false;
        }
    }

    ListOfSocketsToClients.emplace_back();

    sf::TcpSocket::Status StatusOfAlreadyListening=ConnectionListener.accept(ListOfSocketsToClients.back());
    if (StatusOfAlreadyListening==sf::TcpListener::Done) {
        ++HighestConnection;

        ListOfSocketsToClients.back().setBlocking(false);
        int PortOfNewestClient=ConnectionListener.getLocalPort();
        sf::IpAddress IPOfNewestClients=ListOfSocketsToClients.back().getRemoteAddress();
        std::cout<<"Somebody has been connected with ID "<<HighestConnection<<" from IP "<<IPOfNewestClients.toString()<<" on port "<<PortOfNewestClient<<std::endl;

        sf::TcpSocket* myPointer=nullptr;
        myPointer=&(ListOfSocketsToClients.back());
        ConnectionsMap.insert(std::make_pair(HighestConnection,ConnectionInfo(myPointer)));
        ActiveConnectionsMap.insert(std::make_pair(HighestConnection,ConnectionInfo(myPointer)));
        return HighestConnection;
    } else {
        //std::cout<<"Nobody found to connect or error"<<std::endl;
        ListOfSocketsToClients.pop_back();
        return 0;
    }
}

bool PostMan::uploadPacket(int ConnectionID, sf::Packet* packet) {
    if (packet->getDataSize()>0) {
        //std::cout<<"Uploader: Searching if following connection ID exists: "<<ConnectionID<<std::endl;
        auto it=ActiveConnectionsMap.find(ConnectionID);
        if (it==ActiveConnectionsMap.end()) {
            std::cout<<"Postman uploader: Connection ID not found while uploading"<<std::endl;
            return false;
        } else {
            sf::TcpSocket* _socketPointer=nullptr;
            _socketPointer=(it->second).PointerToTCPSocket;
            sf::TcpSocket::Status status=_socketPointer->send(*packet);
            if (status==sf::Socket::Done) {
                //std::cout<<"Uploader: You were lucky your packet was sent at first attempt!"<<std::endl;
                packet->clear();
                return true;
            } else if (status==sf::Socket::Partial) {
                std::cout<<"Postman uploader: Packet only partially sent! Trying again..."<<std::endl;
                while (_socketPointer->send(*packet)==sf::Socket::Partial) {
                    std::cout<<"Postman uploader: Packet still not fully sent! Trying again..."<<std::endl;
                }
                std::cout<<"Postman uploader: Packet finally sent!"<<std::endl;
                packet->clear();
                return true;
            } else if (status==sf::Socket::NotReady) {
                std::cout<<"Postman uploader: Socket not ready. Dont know what to do!"<<std::endl;
                return false;

            } else if (status==sf::Socket::Disconnected) {
                std::cout<<"Postman uploader:: Duuh!! Socket disconnected. Removing connection from my active list"<<std::endl;
                //ActiveConnectionsMap.erase(ConnectionID);
                ActiveConnectionsMap.erase(ConnectionID);
                return false;

            } else if (status==sf::Socket::Error) {
                std::cout<<"Postman uploader: Socket error"<<std::endl;
                return false;

            } else {
                std::cout<<"Postman uploader: Socket is really fucked! Unlisted error :("<<std::endl;
                return false;
            }
        }
    } else {
        std::cout<<"Uploader: Package is empty. Not sending"<<std::endl;
        return 0;
    }
}

bool PostMan::uploadPacketToAllConnected(sf::Packet* packet) {
    bool status;
    bool finalstatus=true;
    for(auto& kv : ActiveConnectionsMap) {
        int thisConnectionID=kv.first;
        sf::Packet currentPacket=*packet; //yes, a copy is needed here. Dont want to touch the origial before everybody got it
        //std::cout<<"Uploading packet with size (before): "<<packet.getDataSize()<<std::endl;
        status=uploadPacket(thisConnectionID,&currentPacket);
        if(status==false) {
            std::cout<<"PostMan uploadPacket: Packet could not be uploaded to connection ID "<<thisConnectionID<<std::endl;
            finalstatus=false;
        }
    }
    if (finalstatus==true) {
        //std::cout<<"Uploading packet with size (after): "<<packet.getDataSize()<<std::endl;
        packet->clear();
    }
    return finalstatus;
}

bool PostMan::predownloadPackage(int ConnectionID,sf::Packet* packet) {
    auto it=ActiveConnectionsMap.find(ConnectionID);
    if (it==ActiveConnectionsMap.end()) {
        std::cout<<"Downloader: Connection ID not found while downloading"<<std::endl;
        return false;
    } else {
        predownloadPackage(it, packet);
    }
}

bool PostMan::predownloadPackage(std::map<int, ConnectionInfo>::iterator& it,sf::Packet* packet) {
    int ConnectionID=it->first;

    sf::TcpSocket* _socketPointer=nullptr;
    _socketPointer=(it->second).PointerToTCPSocket;

    sf::TcpSocket::Status status=_socketPointer->receive(*packet);

    if (status==sf::TcpSocket::Done) {
        //std::cout<<"Downloader: Download suceeded at first attempt"<<std::endl;
        //std::cout<<"Size of packet received from ClientID "<<ConnectionID<<": "<<packet.getDataSize()<<std::endl;
        return true;
    } else if (status==sf::Socket::Partial) {
        std::cout<<"Postman downloader: Packet only partially downloaded! Waiting..."<<std::endl;
        while (_socketPointer->receive(*packet)==sf::Socket::Partial) {
            std::cout<<"Postman downloader: Packet still not fully downloaded! Trying again..."<<std::endl;
        }
        std::cout<<"Postman downloader: Packet finally downloaded!"<<std::endl;
        return true;
    } else if (status==sf::Socket::NotReady) {
        //std::cout<<"Postman downloader: Socket not ready. Probably no data to receive!"<<std::endl;
        return false;

    } else if (status==sf::Socket::Disconnected) {
        it=ActiveConnectionsMap.erase(it);
        std::cout<<"Postman downloader: Duuh!! ClientID "<<ConnectionID<<" is disconnecte and is now removed from my list."<<std::endl;
        return false;

    } else if (status==sf::Socket::Error) {
        std::cout<<"Postman downloader: Socket error"<<std::endl;
        return false;

    } else {
        std::cout<<"Postman downloader: Socket is really fucked! Unlisted error :("<<std::endl;
        return false;
    }
}

bool PostMan::downloadPackages() {
    bool status;
    bool finalstatus=true;
    //for(std::map<int, ConnectionInfo>::value_type& kv : ActiveConnectionsMapExtended) {

    for(auto kv = ActiveConnectionsMap.begin(); kv != ActiveConnectionsMap.end();) {
        auto kvold=kv;
        status=predownloadPackage(kv,&((kv->second).DownPacket));
        //std::cout<<"PostMan::checkNewPackageDownload from connection "<<kv->first<<" size received: "<<(kv->second).DownPacket.getDataSize()<<std::endl;
        if(kv!=kvold) {}
        else {
            ++kv;
        }

        if(status==false) {
            //std::cout<<"PostMan checkNewPackageDownload: Packet could not be downloaded from connection ID "<<thisConnectionID<<std::endl;
            finalstatus=false;
        }
    }
    return finalstatus;
}

void PostMan::debug_checkDownSizes() {

    for (auto currentKVit=ActiveConnectionsMap.cbegin(); currentKVit!=ActiveConnectionsMap.cend(); ++currentKVit) {
        //std::cout<<"On connection: "<<currentKVit->first<<" size of downpacket is: "<<(currentKVit->second).DownPacket.getDataSize()<<std::endl;
    }

}

std::vector<int> PostMan::getActiveConnections(bool extraUpdate) {
    std::vector<int> MarkToErase;
    std::vector<int> result;

    if(extraUpdate) {
        for(auto& kv : ActiveConnectionsMap) {
            sf::TcpSocket* mySocketPointer=nullptr;
            int myConnectionID=kv.first;
            mySocketPointer=(kv.second).PointerToTCPSocket;
            if(mySocketPointer->getRemoteAddress()==sf::IpAddress::None) {
                MarkToErase.emplace_back(myConnectionID);
            }
        }

        for (auto& thisConnectionID:MarkToErase) {
            ActiveConnectionsMap.erase(thisConnectionID);
        }
    }

    for(auto& kv : ActiveConnectionsMap) {
        result.emplace_back(kv.first);
    }
    return result;
}

bool PostMan::transferDownloadedPackages(std::map<int,sf::Packet*>& TransferToThisHologramMap) {

    for(std::map<int, ConnectionInfo>::iterator kv = ActiveConnectionsMap.begin(); kv != ActiveConnectionsMap.end(); ++kv) {
        int currentConnectionID=kv->first;

        sf::Packet* currentPacketPointer=nullptr;
        currentPacketPointer=&(kv->second.DownPacket);

        auto currentCombination=TransferToThisHologramMap.find(currentConnectionID);
        if(currentCombination==TransferToThisHologramMap.end()) {
            std::cout<<"PostMan downloader packages transfer: Your hologram doesnt contain necessary ID ("<<currentConnectionID<<") to receive data! Aborting operation"<<std::endl;
            return false;
        } else {
            //std::cout<<"PostMan: Trying to transfer to ID: "<<currentConnectionID<<std::endl;
            *(currentCombination->second)=*currentPacketPointer;
        }
    }
    return true;

}
