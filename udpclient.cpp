
/*************************************************************************
 > File Name: client.c
 > Author: SongLee
 ************************************************************************/
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<netdb.h>
#include<stdarg.h>
#include<string.h>

#include "global.h"

#define SERVER_PORT 8000
#define BUFFER_SIZE 1024
#define FILE_NAME_MAX_SIZE 512

int nbr=0;

int main()
{
 /* 服务端地址 */
 struct sockaddr_in server_addr;
 bzero(&server_addr, sizeof(server_addr));
 server_addr.sin_family = AF_INET;
 server_addr.sin_addr.s_addr = inet_addr("192.168.1.71");
 server_addr.sin_port = htons(SERVER_PORT);

 /* 创建socket */
 int client_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
 if(client_socket_fd < 0)
 {
  perror("Create Socket Failed:");
  exit(1);
 }

 /* 输入文件名到缓冲区 */
 char file_name[FILE_NAME_MAX_SIZE+1];
 bzero(file_name, FILE_NAME_MAX_SIZE+1);
 printf("Please Input File Name On Server:\t");

 char buffer[BUFFER_SIZE];
 bzero(buffer, BUFFER_SIZE);


 do{
     struct cmd_req *r=(struct cmd_req *)buffer;

     r->cmd = htons(1);
     r->attach = htons(nbr++);
     r->ul.x = htons(320/2-10);
     r->ul.y = htons(256/2-5);
     r->sz.cx = htons(10*2);
     r->sz.cy = htons(5*2);
     r->checksum = htons(-1);

     printf("SEND: %04x %04x %04x %04x %04x %04x\r\n",
            r->cmd,
            r->attach,
            r->ul.x,
            r->ul.y,
            r->sz.cx,
            r->sz.cy);

     /* 发送文件名 */
     if(sendto(client_socket_fd, buffer, sizeof(*r),0,(struct sockaddr*)&server_addr,sizeof(server_addr)) < 0)
     {
      perror("Send File Name Failed:");
      exit(1);
     }

     int len;
     socklen_t server_addr_length = sizeof(server_addr);
     if((len=recvfrom(client_socket_fd, buffer, BUFFER_SIZE,0,(struct sockaddr*)&server_addr, &server_addr_length)) == -1)
     {
      perror("Receive Data Failed:");
      exit(1);
     }

     if(len<sizeof (struct cmd_resp))
     {
         perror("Receive Data Failed: too small");
     }

     struct cmd_resp *p=(struct cmd_resp *)buffer;
     ushort  ScaleLow,ScaleHigh,AreaMin,AreaMax,AreaAvg;

     printf("RECV: %04x %04x %04x %04x %04x %04x %04x \r\n",
            p->cmd,
            p->attach,
            p->ScaleLow,
            p->ScaleHigh,
            p->AreaMin,
            p->AreaMax,
            p->AreaAvg);

     if(ntohs(p->cmd)==1)
     {
         ScaleLow = ntohs(p->ScaleLow);
         ScaleHigh = ntohs(p->ScaleHigh);
         AreaMin = ntohs(p->AreaMin);
         AreaMax = ntohs(p->AreaMax);
         AreaAvg = ntohs(p->AreaAvg);
     }

     printf("%d %d %d %d %d %.2f\r\n",ScaleLow,ScaleHigh,AreaMin,AreaMax,AreaAvg,AreaAvg*0.04-273.15);

     sleep(1);

  }while(1);

     close(client_socket_fd);
     return 0;
}





