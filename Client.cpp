#include "Client.hpp"

void Client::run() {
    init();
    while (keepRunning) {
        while (hasToConnect) {
            ConnectionID=myCommunicator.connectTo(IPAddress,Port,sf::Time::Zero);
            if(ConnectionID>0) {
                std::cout<<"Connected as a client!"<<std::endl;
                std::cout<<"Connection ID is "<<ConnectionID<<std::endl;
                hasToConnect=false;
            }
        }
        readInputs();
        std::cout<<"Client: Trying to upload to connection ID "<<ConnectionID<<std::endl;
        myCommunicator.putPacketToUpLoad(ConnectionID,UploadPacket);
        if (myCommunicator.checkNewPackageDownloadFrom(ConnectionID,DownloadPacket)) {
            printContent(DownloadPacket);
        }
    }
}

void Client::printContent(sf::Packet& myDownPacket) {
    networkprotocol::HeaderType myHeader;
    if (myDownPacket>>myHeader) {
        if (myHeader==static_cast<networkprotocol::HeaderType>(networkprotocol::FromServer::ServerMessage)) {
            std::string myMessage;
            myDownPacket>>myMessage;
            std::cout<<"Server message: "<<myMessage;
        } else if (myHeader==static_cast<networkprotocol::HeaderType>(networkprotocol::FromServer::KillClient)) {
            std::cout<<"Disconnected by server! "<<std::endl;
        }

    }
}
void Client::readInputs() {

    if (requestNewEntryField) {
        std::cout<<"Entre com o seu cabecalho: "<<std::endl;
        static std::string temp;
        networkprotocol::HeaderType UpHeader;
        std::string UpMessage;
        std::getline(std::cin, temp);
        std::cin.clear();
        if (temp=="KillServer") {
            UpHeader=static_cast<networkprotocol::HeaderType>(networkprotocol::FromClient::KillServer);
        }
        if (temp=="GoBack") {
            UpHeader=static_cast<networkprotocol::HeaderType>(networkprotocol::FromClient::Back);
            UpMessage="Hello!";
        }
        std::cout<<"Entre com a sua mensagem: "<<std::endl;
        std::getline(std::cin, UpMessage);
        std::cin.clear();
        UploadPacket<<UpHeader<<UpMessage;
    }
}


void Client::init() {
    std::cout<<"Enter server port to connect to: "<<std::endl;
    std::cin>>Port;
    std::cin.clear();
    std::cout<<"Enter server addres to connect to: "<<std::endl;
    std::cin>>IPAddress;
    std::cin.clear();
}
