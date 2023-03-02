#include "udpsocket.hpp"
#include "../../src/tracking/udpThread/udpConn.hpp"

#include <iostream>

using namespace std;

int main()
{
    // Our constants:
    const string IP = "localhost";
    const uint16_t PORT = 8888;

    // Initialize socket.
    UDPSocket udpSocket(true); // "true" to use Connection on UDP. Default is "false".
    udpSocket.Connect(IP, PORT);


    // Send String:
    SharedData data = {0,12,12,40.1,30,2921834791827349618};
    //udpSocket.SendTo("ABCDEFGH", IP, PORT); // If you want to connectless
    // If you want to use std::string:
    /*
    udpSocket.onMessageReceived = [&](string message, string ipv4, uint16_t port) {
        cout << ipv4 << ":" << port << " => " << message << endl;
    };
    */

    // You should do an input loop so the program will not terminated immediately:
    string input;
    getline(cin, input);
    while (input != "exit")
    {
        
        char* const buf = reinterpret_cast<char*>(&data);
        cout<<buf<<endl;
        udpSocket.Send(buf);
        getline(cin, input);
    }

    // Close the socket.
    udpSocket.Close();

    return 0;
}