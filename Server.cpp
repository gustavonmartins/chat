#include "Server.hpp"
#include <algorithm>


void Server::run() {
    init();
    while (keepRunning) {
        if (timer1.getTimerApproval(2000,false)) {
            int ConnectionID;
            ConnectionID=myCommunicator.listenToNewConnection(ServerPort);
            if(ConnectionID>0) {
                SVConnectionsMap.insert(std::make_pair(ConnectionID,SVConnInfo()));
            }
        }
        if (timer2.getTimerApproval(10,false)) {
            ConnectionsVector=myCommunicator.refreshActiveConnections();
            std::vector<int> MarkToErase;
            for (auto& kv:SVConnectionsMap) {
                int currentConnectionID=kv.first;
                if(std::find(ConnectionsVector.begin(), ConnectionsVector.end(),currentConnectionID) == ConnectionsVector.end()) {
                    MarkToErase.emplace_back(currentConnectionID);
                }
            }
            for (auto& thisConnectionID:MarkToErase) {
                SVConnectionsMap.erase(thisConnectionID);
            }
            myCommunicator.checkNewPackageDownload(ClientIDToDownPacket);








            for(auto& currentPair:ClientIDToDownPacket) {
                sf::Packet curentPacket;
                GameController currentGameController;

                curentPacket=currentPair.second;

                //std::cout<<"Size of packet being disasembled: "<<curentPacket.getDataSize()<<std::endl;

                networkprotocol::dissasemble(curentPacket,currentGameController);
                if(currentGameController.GoBack==true) {
                    std::cout<<"GoBack"<<std::endl;
                }
                if(currentGameController.GoForward==true) {
                    std::cout<<"GoForward"<<std::endl;
                }
                if(currentGameController.GoLeft==true) {
                    std::cout<<"GoLeft"<<std::endl;
                }
                if(currentGameController.GoRight==true) {
                    std::cout<<"GoRight"<<std::endl;
                }
                if(currentGameController.LookAround!=0) {
                    std::cout<<"LookAround: "<<currentGameController.LookAround<<std::endl;
                }
                if(currentGameController.PrepareAction2==true) {
                    std::cout<<"PrepareAction2"<<std::endl;
                }
                if(currentGameController.PrepareAction==true) {
                    std::cout<<"PrepareAction"<<std::endl;
                }
                if(currentGameController.RequestAction2==true) {
                    std::cout<<"RequestAction2"<<std::endl;
                }

                if(currentGameController.RequestAction==true) {
                    std::cout<<"RequestAction"<<std::endl;
                }

                if(currentGameController.RequestJump==true) {
                    std::cout<<"RequestJump"<<std::endl;
                }

                if(currentGameController.SelectNext==true) {
                    std::cout<<"SelectNext"<<std::endl;
                }
            }







        }
    }
}

void Server::init() {
    std::cout<<"Enter server port: "<<std::endl;
    std::cin>>ServerPort;
}

void Server::readInputs() {}
