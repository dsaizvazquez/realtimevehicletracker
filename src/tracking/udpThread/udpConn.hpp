#ifndef CONN
#define CONN

#include <mutex>
#include <spdlog/spdlog.h>
#include <cstring>
#include "udpserver.hpp"

typedef struct UdpConnConfiguration
{
	std::string IPv4 = "127.0.0.1";
    std::uint16_t port = 8888;
    
}UdpConnConfiguration;



#pragma pack(push,1)
typedef struct SharedData 
{
    int id;
    float pitch;
    float yaw; 
    float altitude;
    float focalLength;
	long timestamp;
}SharedData;
#pragma pack(pop)



typedef SharedData Packet; //change to SharedData when necessary

union Message {
    Packet data;
    unsigned char * msg ;
};

class UDPConn 
{
private:
    UDPServer server;
    Packet packet{0,0,0,6,50,0};
    std::mutex mtx;
public:
    Packet getPacket();
    void init(std::string IPv4, std::uint16_t port);
    void init(UdpConnConfiguration *config);

    void close(){
        server.Close();
    };
};

#endif