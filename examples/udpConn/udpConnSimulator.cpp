#include "udpsocket.hpp"
#include "../../src/tracking/udpThread/udpConn.hpp"
#include <unistd.h>
#include "yaml-cpp/yaml.h"
#include <spdlog/spdlog.h>
#include <iostream>

using namespace std;
unsigned int microsecond = 1000000;

int main()
{
    // Our constants:
    const string IP = "localhost";
    const uint16_t PORT = 8998;

    // Initialize socket.
    UDPSocket udpSocket(true); // "true" to use Connection on UDP. Default is "false".
    udpSocket.Connect(IP, PORT);

    YAML::Node configYAML = YAML::LoadFile("dronePos.yaml");
    float pitch=configYAML["pitch"].as<float>();
    float yaw=configYAML["yaw"].as<float>();
    float altitude=configYAML["altitude"].as<float>();
    float focalLength=configYAML["focalLength"].as<float>();
    Message message = {0,pitch,yaw,altitude,focalLength,0};
    spdlog::info("id: {}, pitch: {},yaw: {}, altitude:{}, focal: {}, timestamp: {}",message.data.id,
                                                        message.data.pitch,
                                                        message.data.yaw,
                                                        message.data.altitude,
                                                        message.data.focalLength,
                                                        message.data.timestamp);
    udpSocket.Send(message.msg,sizeof(SharedData));
    // Close the socket.
    udpSocket.Close();

    return 0;
}