#include "udpConn.hpp"



void UDPConn::init(std::string IPv4, std::uint16_t port){

    server.onRawMessageReceived = [&](const char* message, int length, std::string ipv4, std::uint16_t port) {

        std::lock_guard<std::mutex> lck {mtx};
        Message messagePacket;
        memcpy(&messagePacket, message, length * sizeof(char));
        spdlog::info(messagePacket.data.id);
        packet = messagePacket.data;

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
    std::lock_guard<std::mutex> lck {mtx};
    return packet;

}
