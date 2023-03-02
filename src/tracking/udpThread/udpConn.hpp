#ifndef CONN
#define CONN

#include <mutex>
#include <spdlog/spdlog.h>
#include <cstring>
#include "udpserver.hpp"


typedef struct SharedData 
{
    int id;
    float yaw;
    float pitch; 
    float altitude;
    std::uint16_t focalLength;
	long timestamp;
}SharedData;

typedef SharedData Packet; //change to SharedData when necessary

class UDPConn 
{
private:
    UDPServer server;
    Packet packet;
    std::mutex mtx;
public:
    Packet getPacket();
    void init(std::string IPv4, std::uint16_t port);
    void close(){
        server.Close();
    };
};

#endif