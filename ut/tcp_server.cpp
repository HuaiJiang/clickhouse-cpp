#include "tcp_server.h"
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <iostream>
#include <clickhouse/protocol.h>
#include <thread>

namespace clickhouse{

LocalTcpServer::LocalTcpServer(int port):
     port_(port)
     ,serverSd_(-1)
    ,socket_(-1)
    , socket_input_(socket_)
    , buffered_input_(&socket_input_)
    , input_(&buffered_input_)
    , socket_output_(socket_)
    , buffered_output_(&socket_output_)
    , output_(&buffered_output_){};


LocalTcpServer::~LocalTcpServer()
{
    stop();
}
void LocalTcpServer::start()
{
   //setup a socket 
    sockaddr_in servAddr;
    bzero((char*)&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port_);
    serverSd_ = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSd_ < 0)
    {
        std::cerr << "Error establishing server socket" << std::endl;
        throw std::runtime_error("Error establishing server socket" );
    }
    int enable = 1;
    if (setsockopt(serverSd_, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        std::cerr<<"setsockopt(SO_REUSEADDR) failed" <<std::endl;
     //bind the socket to its local address
    int bindStatus = bind(serverSd_, (struct sockaddr*) &servAddr, 
        sizeof(servAddr));
    if(bindStatus < 0)
    {
        std::cerr << "Error binding socket to local address" <<std:: endl;
        throw std::runtime_error("Error binding socket to local address" );
    }
    listen(serverSd_, 3);
    std::thread thread(&LocalTcpServer::startImpl,this);
    thread.detach();
    
}

void LocalTcpServer::startImpl()
{
    while(1)
    {
        sockaddr_in newSockAddr;
        socklen_t newSockAddrSize = sizeof(newSockAddr);
        int newSd = accept(serverSd_, (sockaddr *)&newSockAddr, &newSockAddrSize);
        if(newSd < 0)
        {
            std::cerr << "Error accepting request from client!" << std::endl;
            throw std::runtime_error("Error accepting request from client" );

        }
        std::cout << "Connected with client!" << std::endl;
        SocketHolder s(newSd);
        socket_ = std::move(s);
        socket_input_ = SocketInput(socket_);
        socket_output_ = SocketOutput(socket_);
        buffered_input_.Reset();
        buffered_output_.Reset();
        uint64_t packet_type = 0;

        if (!input_.ReadVarint64(&packet_type)) {
            throw std::runtime_error("not able to read from client" );
        }
        if (packet_type == ServerCodes::Hello) {
            std::cout<<"got hello from client"<<std::endl;
        }else{
            std::cout<<"not get client hello close the connection"<<std::endl;
            shutdown(newSd,SHUT_WR);
        }

    }
    
}

void LocalTcpServer::stop()
{
    if(serverSd_>0)
    {
        shutdown(serverSd_,SHUT_RDWR);
        close(serverSd_);
        serverSd_=-1;
    }
        
}


}
