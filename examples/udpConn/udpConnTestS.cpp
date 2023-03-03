#include <iostream>
#include "../../src/tracking/udpThread/udpConn.hpp"

using namespace std;

int main()
{
    // Initialize server socket..
    UDPConn udpServer;

    
    udpServer.init("127.0.0.1",8998);
    // You should do an input loop so the program will not terminated immediately:
    string input;
    getline(cin, input);
    while (input != "exit")
    {
        getline(cin, input);
        cout<<udpServer.getPacket().altitude<<endl;
    }

    udpServer.close();

    return 0;
}