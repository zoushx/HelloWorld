
/*************************************************************************
 > File Name: server.c
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

int check(UCHAR *buff,int buflen)
{
    return 0;
}

void* udp_server(void* p_para)
{
     /* 创建UDP套接口 */
     struct sockaddr_in server_addr;
     bzero(&server_addr, sizeof(server_addr));
     server_addr.sin_family = AF_INET;
     server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
     server_addr.sin_port = htons(SERVER_PORT);

     /* 创建socket */
     int server_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
     if(server_socket_fd == -1)
     {
      perror("Create Socket Failed:");
      exit(1);
     }

     /* 绑定套接口 */
     if(-1 == (bind(server_socket_fd,(struct sockaddr*)&server_addr,sizeof(server_addr))))
     {
      perror("Server Bind Failed:");
      exit(1);
     }

     /* 数据传输 */
     while(1)
     {
          /* 定义一个地址，用于捕获客户端地址 */
          struct sockaddr_in client_addr;
          socklen_t client_addr_length = sizeof(client_addr);

          /* 接收数据 */
          char buffer[BUFFER_SIZE];
          bzero(buffer, BUFFER_SIZE);
          int len;
          if((len=recvfrom(server_socket_fd, buffer, BUFFER_SIZE,0,(struct sockaddr*)&client_addr, &client_addr_length)) == -1)
          {
           perror("Receive Data Failed:");
           exit(1);
          }

          if(len<sizeof (struct cmd_req))
          {
              perror("Receive Data Failed: too small");
          }

          struct cmd_req *r=(struct cmd_req *)buffer;
          struct cmd_resp *p=(struct cmd_resp *)buffer;

          printf("RECV: %04x %04x %04x %04x %04x %04x\r\n",
                 r->cmd,
                 r->attach,
                 r->ul.x,
                 r->ul.y,
                 r->sz.cx,
                 r->sz.cy);

          if(ntohs(r->cmd)==1)
          {
              CPoint ul;
              CSize sz;
              ul.x = ntohs(r->ul.x);
              ul.y = ntohs(r->ul.y);
              sz.cx = ntohs(r->sz.cx);
              sz.cy = ntohs(r->sz.cy);

              CalcArea(m_SrcIamge,ul,sz);

              p->ScaleLow = htons(m_ScaleLow);
              p->ScaleHigh = htons(m_ScaleHigh);
              p->AreaMin = htons(m_AreaMin);
              p->AreaMax = htons(m_AreaMax);
              p->AreaAvg = htons(m_AreaAvg);

              p->checksum = -1;

              printf("SEND: %04x %04x %04x %04x %04x %04x %04x \r\n",
                     p->cmd,
                     p->attach,
                     p->ScaleLow,
                     p->ScaleHigh,
                     p->AreaMin,
                     p->AreaMax,
                     p->AreaAvg);

              if(sendto(server_socket_fd, buffer, sizeof(*p),0,(struct sockaddr*)&client_addr,sizeof(client_addr)) < 0)
              {
               perror("Send File Name Failed:");
               exit(1);
              }
          }
     }

     close(server_socket_fd);
}
