#include "MyLoRa.h"
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class NTOUSocket
{
    private:
        int s = 0 ;
        struct sockaddr_in info;

    public:

        NTOUSocket()
        {
            s = socket(AF_INET,SOCK_STREAM,0);
            if(s != -1)
            {
                this->info.sin_family = AF_INET;
                this->info.sin_addr.s_addr = inet_addr("127.0.0.1");
                this->info.sin_port = htons(8080); 
                while(connect(s,(struct sockaddr*)&(this->info),sizeof(info)));
            }
        }

        void SendToSocketServer(unsigned char* data , size_t size)
        {
            send(this->s,data,size,0);
        }
};

int main()
{
    MyLoRa L(0x02,0x01,11);
    NTOUSocket NSocket ;
    while(true)
    {
        while(!L.getPacket(true));
        L.PrintPacket(L.backData,11,"Main Func Get :");
        NSocket.SendToSocketServer(L.backData,11);
        //L.SendACK(false); // 修改時間2022-06-15--13-43
    }
}


