#include <clickhouse/base/socket.h>
#include <contrib/gtest/gtest.h>
#if !defined(_win_)
#   include <errno.h>
#   include <fcntl.h>
#   include <netdb.h>
#   include <signal.h>
#   include <unistd.h>
#   include <arpa/inet.h>
#endif
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include "tcp_server.h"
#include <thread>
using namespace clickhouse;


TEST(Socketcase, connectnoerror) {
   int port=9977;
   NetworkAddress addr("localhost",std::to_string(port));
   
   LocalTcpServer server(port);
   server.start();
   std::this_thread::sleep_for(std::chrono::seconds(1));
   SocketConnect(addr);
   server.stop();
}


TEST(Socketcase, connecterror) {
   int port=9978;
   NetworkAddress addr("localhost",std::to_string(port));
   try{
      SocketConnect(addr);
      FAIL();
   }catch(const std::system_error& e)
   {
      ASSERT_NE(EINPROGRESS,e.code().value());
   }
   
 
}