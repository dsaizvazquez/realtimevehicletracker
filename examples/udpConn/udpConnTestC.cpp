#include "udpsocket.hpp"
#include "../../src/tracking/udpThread/udpConn.hpp"

#include <iostream>

using namespace std;

union Message {
    SharedData data;
    char msg[sizeof(SharedData)];
};

int main()
{
    // Our constants:
    const string IP = "localhost";
    const uint16_t PORT = 8998;
    // Initialize socket.
    UDPSocket udpSocket(true); // "true" to use Connection on UDP. Default is "false".
    udpSocket.Connect(IP, PORT);

    Message message = {7,12,12,40.1,30,2921834791827349618};

    // You should do an input loop so the program will not terminated immediately:
    string input;
    getline(cin, input);
    while (input != "exit")
    {
        udpSocket.Send(message.msg,sizeof(SharedData));
        getline(cin, input);
    }

    // Close the socket.
    udpSocket.Close();

    return 0;
}