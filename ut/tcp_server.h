#pragma once
#include <clickhouse/base/socket.h>
#include <clickhouse/base/coded.h>

namespace clickhouse {
class LocalTcpServer {

public:
    LocalTcpServer(int port);
    ~LocalTcpServer();
public:
    void start();
    void stop();
private:
    void startImpl();
private:
    int port_;
    bool IPV6;
    int serverSd_;
    SocketHolder socket_;

    SocketInput socket_input_;
    BufferedInput buffered_input_;
    CodedInputStream input_;

    SocketOutput socket_output_;
    BufferedOutput buffered_output_;
    CodedOutputStream output_;


};




}