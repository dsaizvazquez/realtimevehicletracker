#include "udpsocket.hpp"
#include "../../src/tracking/udpThread/udpConn.hpp"
#include<unistd.h>

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


    float ground = 428.7925;
    float minZoom =6.83;

    Message message1 = {1, -52.9,  15.8, 548.792-ground,   1.337*minZoom, 0}; //38 s
    Message message2 ={2, -31.6,  -7.8, 548.748-ground,   7.042*minZoom,  0}; //66s
    Message message3 ={3, -13.5, 108.8, 548.817-ground,   11.565*minZoom, 0}; //11s



    // You should do an input loop so the program will not terminated immediately:
    string input;
    while (input != "exit")
    {
        //input.assign(message1.msg,sizeof(SharedData));
        //udpSocket.Send(input);
        usleep(38 * microsecond);
        udpSocket.Send(message2.msg,sizeof(SharedData));
        usleep(66 * microsecond);
        //udpSocket.Send(message3.msg,sizeof(SharedData));
        //usleep(11 * microsecond);


    }

    // Close the socket.
    udpSocket.Close();

    return 0;
}