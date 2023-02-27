#include "udpConn.hpp"

void UDPConn::init(std::string IPv4, std::uint16_t port){
    server.onRawMessageReceived = [&](const char* message, int length, std::string ipv4, std::uint16_t port) {

        //TODO IMPLEMENT PACKAGE
        packet.store(message);
        
        // ACK if necessary
        //server.SendTo(message, length, ipv4, port);
    };
    

    // Bind the server to a port.
    server.Bind(port, [](int errorCode, std::string errorMessage) {
        // BINDING FAILED:
        spdlog::error("Error code: {}, {}",errorCode,errorMessage);
    });
}

Packet UDPConn::getPacket(){
    return packet.load();
}
