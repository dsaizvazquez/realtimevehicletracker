#include <atomic>
#include <spdlog/spdlog.h>

#include "udpserver.hpp"

typedef struct vector3f
{
  float x; /* pitch */
  float y; /* roll  */
  float z; /* yaw   */
} vector3f;

typedef struct SharedData
{
    int id;
    vector3f Gimbal; 
    float altitude;
    std::uint16_t focalLength;
	long timestamp;
}SharedData;

typedef std::string Packet; //change to SharedData when necessary

class UDPConn 
{
private:
    UDPServer server;
    std::atomic<Packet> packet;
public:
    Packet getPacket();
    void init(std::string IPv4, std::uint16_t port);
};