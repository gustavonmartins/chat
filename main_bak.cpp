#include <iostream>
#include <SFML/Network.hpp>

const unsigned short PORT = 5000;


std::string msgSend;

sf::TcpSocket socket;
sf::Mutex globalMutex;
bool quit = false;

void SendAndReceive(void)
{
    static std::string oldMsg;
    while(!quit)
    {
        sf::Packet packetSend;
        globalMutex.lock();
        packetSend << msgSend;
        globalMutex.unlock();

        socket.send(packetSend);

        std::string msg;
        sf::Packet packetReceive;

        socket.receive(packetReceive);
        if(packetReceive >> msg)
        {
            if(oldMsg != msg)
                if(!msg.empty())
                {
                    std::cout << msg << std::endl;
                    oldMsg = msg;
                }
        }
    }
}
void Server(void)
{
    sf::TcpListener listener;
    listener.setBlocking(false);
    listener.listen(PORT);
    listener.accept(socket);
    std::cout << "New client connected: " << socket.getRemoteAddress() << std::endl;
}
bool Client(void)
{
    std::cout << "Enter IP address to connect to: " << std::endl;
    std::string IPADDRESS;//change to suit your needs
    std::cin>>IPADDRESS;
    if(socket.connect(IPADDRESS, PORT) == sf::Socket::Done)
    {
        std::cout << "Connected\n";
        return true;
    }
    return false;
}
void GetInput(void)
{
    std::string UserText;
    std::cout << "\nEnter \"exit\" to quit or message to send: ";
    std::getline(std::cin,UserText);
    //std::cin >> s;
    if(UserText == "exit")
        quit = true;
    globalMutex.lock();
    msgSend = UserText;
    globalMutex.unlock();
}
int main(int argc, char* argv[])
{
    sf::Thread* thread = 0;

    char who;
    std::cout << "Do you want to be a server (s) or a client (c) ? ";
    std::cin  >> who;

    if(who == 's')
    {
        Server();
        std::cout<<"Server testing"<<std::endl;
    }

    else
        Client();

    thread = new sf::Thread(&SendAndReceive);
    thread->launch();

    while(!quit)
    {
        GetInput();
    }
    if(thread)
    {
        thread->wait();
        delete thread;
    }
    return 0;
}
