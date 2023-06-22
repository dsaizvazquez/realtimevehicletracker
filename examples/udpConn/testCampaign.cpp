#include "udpsocket.hpp"
#include "../../src/tracking/udpThread/udpConn.hpp"
#include <unistd.h>
#include "yaml-cpp/yaml.h"
#include <spdlog/spdlog.h>
#include <iostream>

using namespace std;
unsigned int microsecond = 1000000;

int main(int argc, char * argv[])
{
    YAML::Node configYAML = YAML::LoadFile("dronePos.yaml");
    int id = 0;
    float pitch = configYAML["pitch"].as<float>();
    float yaw = configYAML["yaw"].as<float>();
    float altitude = configYAML["altitude"].as<float>();
    float focalLength = configYAML["focalLength"].as<float>();
    uint16_t PORT = 8998;


    int c;
    while ((c = getopt (argc, argv, "p:y:a:l:o:")) != -1){
        switch(c) // note the colon (:) to indicate that 'b' has a parameter and is not a switch
        {
        case 'p':
            pitch=std::stod(optarg);
            break;
        case 'y':
            yaw=std::stod(optarg);
            break;
        case 'a':
            altitude=std::stod(optarg);
            break;
        case 'l':
            id=std::stoi(optarg);
            break;
        case 'o':
            PORT=std::stoi(optarg);
            break;
        }
    }

    
    // Our constants:
    const string IP = "localhost";

    // Initialize socket.
    UDPSocket udpSocket(true); // "true" to use Connection on UDP. Default is "false".
    udpSocket.Connect(IP, PORT, [](int errorCode, std::string errorMessage){
        exit(1);
    });


    Message message = {id,pitch,yaw,altitude,focalLength,0};
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