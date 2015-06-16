#include "rdma_client.h"
using namespace std;

RDMAClient::RDMAClient()
{
    channel = 0;
    memory  = 0;
    size    = 0;
}

bool RDMAClient::createChannel(string serverIp, int port)
{
    if (channel != 0)
    {
        cerr<<"Channel has been created"<<endl;
        return false;
    }

    try
    {
        channel = new ClientSocket(serverIp, port);
    }
    catch ( SocketException& e )
    {
        std::cerr<< "RDMAClient::createChannel(), cannot create channel, Exception was caught:" << e.description() << "\n";
        return false;
    }
    return true;
}

bool RDMAClient::registerMemory(uchar *m, int s)
{
    memory = m;
    size   = s;
}

bool RDMAClient::connect()
{
   *channel << CONNECT_REQUEST; 
   
   string msg;
   *channel >> msg;
   
   if (msg == CONNECT_REQUEST_ACK)
   {
       cout<<"RDMAClient::connect(), receive CONNECT_REQUEST_ACK"<<endl;
       *channel << CONNECT_ESTABLISHED ;
   }
   else
   {
       cerr<<"RDMAClient::connect(), receive unkonwn msg: "<<msg<<endl;
       return false;
   }

   *channel >> msg;
   if (msg == CONNECT_ESTABLISHED_ACK)
   {
       cout<<"RDMAClient::connect(), receive CONNECT_ESTABLISHED_ACK"<<endl;
   }
   else
   {
       cerr<<"RDMAClient::connect(), receive unkonwn msg: "<<msg<<endl;
       return false;
   }

   return true;
}

bool RDMAClient::read(int localOffset, int remoteOffset, int remoteSize)
{
    if (localOffset + remoteSize > size)    
    {
        cerr<<"RDMAClient::read(), localSize too large"<<endl;
        return false;
    }

    *channel << EVENT_READ ;
    *channel << int2str(remoteOffset) ;
    *channel << int2str(remoteSize) ;

    string msg;
    *channel >> msg;
    if (msg == EVENT_READ_START)
    {
        cout<<"RDMAClient::read(), got EVENT_READ_START, ready to receive..."<<endl;
    }
    else if (msg == EVENT_READ_FAIL_ACK)
    {
        cerr<<"RDMAClient::read(), got EVENT_READ_FAIL_ACK giveup"<< endl;
        return false;
    }

    uchar* mem = memory + localOffset;

    std::string data_str;
    for (int i = 0; i < remoteSize; i++)
    {
        *channel >> data_str;
        *mem = str2uchar(data_str);
        mem++;
    }
    
    *channel >> msg;
    if (msg == EVENT_READ_ACK)
    {
        cout<<"RDMAClient::read(), got EVENT_READ_ACK"<<endl;
    }
    else
    {
        cerr<<"RDMAClient::read(), got unkonwn msg: "<<msg<< endl;
        return false;
    }
   
    return true;
}

bool RDMAClient::write(int localOffset, int remoteOffset, int remoteSize)
{
    if (localOffset + remoteSize > size)    
    {
        cerr<<"RDMAClient::write(), localSize too large"<<endl;
        return false;
    }

    *channel << EVENT_WRITE ;
    string msg;
    *channel >> msg;
    if (msg != ACK)
    {
        cerr<<"RDMAClient::write()1, got unkonw msg: "<<msg<<endl;
        return false;
    }

    *channel << int2str(remoteOffset);
    *channel >> msg;
    if (msg != ACK)
    {
        cerr<<"RDMAClient::write()2, got unkonw msg: "<<msg<<endl;
        return false;
    }

    *channel << int2str(remoteSize) ;

    cout<<"RDMAClient::write(), send EVENT_WRITE ... \t"
        << int2str(remoteOffset) <<"\t"
        << int2str(remoteSize)  <<"\t"
        << endl;

    *channel >> msg;
    if (msg == EVENT_WRITE_START)
    {
        cout<<"RDMAClient::write(), got EVENT_READ_START, ready to send..."<<endl;
    }
    else if (msg == EVENT_WRITE_FAIL_ACK)
    {
        cerr<<"RDMAClient::write(), got EVENT_READ_FAIL_ACK giveup"<< endl;
        return false;
    }

    uchar* mem = memory + localOffset;

    for (int i = 0; i < remoteSize; i++)
    {
        cout<<"send: "<< uchar2str( *mem ) << endl;
        *channel << uchar2str( *mem );
        mem++;

        if (i < remoteSize - 1)
        {
            *channel >> msg;
            if (msg != ACK)
            {
                cerr<<"RDMAClient::write() for loop, got unkonw msg: "<<msg<<endl;
                return false;
            }
        }
    }
    
    *channel >> msg;
    if (msg == EVENT_WRITE_ACK)
    {
        cout<<"RDMAClient::write(), got EVENT_WRITE_ACK"<<endl;
    }
    else
    {
        cerr<<"RDMAClient::write(), got unkonwn msg: "<<msg<< endl;
        return false;
    }
   
    return true;
}

RDMAClient::~RDMAClient()
{
    delete channel;
    delete memory;
}

bool RDMAClient::disconnect()
{
    *channel << DISCONNECT;
    string msg;
    *channel >> msg;
    if (msg == DISCONNECT_ACK)
    {
        cout<<"RDMAClient::disconnect, "<<endl;
        return true;
    }
    else
    {
        cerr<<"RDMAClient::disconnect, receive unkonw msg: "<<msg<<endl;
        return false;
    }
}
