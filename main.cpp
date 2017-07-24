//
//  main.cpp
//  FFmpegRTSPServer
//
//  Created by Mina Saad on 9/22/15.
//  Copyright (c) 2015 Mina Saad. All rights reserved.
//

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>


#include "LiveRTSPServer.h"
#include "FFmpegH264Encoder.h"
#include "FFmpegDecoder.h"

#define MAX_SOURCE_NBR  2

MESAI::FFmpegH264Encoder * encoder[MAX_SOURCE_NBR];
MESAI::LiveRTSPServer * server[MAX_SOURCE_NBR];
MESAI::FFmpegDecoder * decoder[MAX_SOURCE_NBR];

int UDPPort[MAX_SOURCE_NBR];
int HTTPTunnelPort[MAX_SOURCE_NBR];
pthread_t thread1[MAX_SOURCE_NBR];
pthread_t thread2[MAX_SOURCE_NBR];
pthread_t thread3[MAX_SOURCE_NBR];

int quit_flag;
void sighandle(int sig);


void * runServer(void * server)
{

    ((MESAI::LiveRTSPServer * ) server)->run();


    pthread_exit(NULL);
}

void * runEncoder(void * encoder)
{
    ((MESAI::FFmpegH264Encoder * ) encoder)->run();


    pthread_exit(NULL);
}

void onFrame(uint8_t * data)
{
    MESAI::swap_t * mp=(MESAI::swap_t *)data;

    for(int i=0;i<MAX_SOURCE_NBR;i++)
    {
        if(mp->decoder==decoder[i])
        {
            encoder[i]->SendNewFrame(mp->data);
        }
    }
}

void * playMedia(void * decoder)
{
    ((MESAI::FFmpegDecoder * )decoder)->playMedia();
    ((MESAI::FFmpegDecoder * )decoder)->finalize();

    pthread_exit(NULL);
}

int main(int argc, const char * argv[])
{
    quit_flag=0;

    signal(SIGINT, sighandle);


    for(int i=0;i<MAX_SOURCE_NBR;i++)
    {
//        if(argc>=2)
//            decoder[i] = new MESAI::FFmpegDecoder(i==0?argv[1]:"2.flv");
//        if(argc>=3)
//            UDPPort[i] = atoi(argv[2])+i;
//        if(argc>=4)
//            HTTPTunnelPort[i] = atoi(argv[3])+i;

        char fname[32];
        sprintf(fname,"%d.flv",i+1);

        decoder[i] = new MESAI::FFmpegDecoder(fname);
        UDPPort[i] = 10000+i;

        decoder[i]->intialize();
        decoder[i]->setOnframeCallbackFunction(onFrame);
        encoder[i] = new MESAI::FFmpegH264Encoder();
        encoder[i]->SetupVideo(i==0?"dummy0.avi":"dummy1.avi",decoder[i]->width,decoder[i]->height,decoder[i]->frameRate,decoder[i]->GOP,decoder[i]->bitrate);
        server[i] = new MESAI::LiveRTSPServer(encoder[i], UDPPort[i], HTTPTunnelPort[i]);

        pthread_attr_t attr1;
        pthread_attr_init(&attr1);
        pthread_attr_setdetachstate(&attr1, PTHREAD_CREATE_DETACHED);
        int rc1 = pthread_create(&thread1[i], &attr1,  runServer, server[i]);

        if (rc1){
            //exception
            return -1;
        }

        pthread_attr_t attr2;
        pthread_attr_init(&attr2);
        pthread_attr_setdetachstate(&attr2, PTHREAD_CREATE_DETACHED);
        int rc2 = pthread_create(&thread2[i], &attr2,  runEncoder, encoder[i]);

        if (rc2){
            //exception
            return -1;
        }

        pthread_attr_t attr3;
        pthread_attr_init(&attr3);
        pthread_attr_setdetachstate(&attr3, PTHREAD_CREATE_DETACHED);
        int rc3 = pthread_create(&thread3[i], &attr3,  playMedia, decoder[i]);

        if (rc3){
            //exception
            return -1;
        }
    }

    while(!quit_flag)
    {
        usleep(500);
    }

    return 0;
}



void sighandle(int sig)
{
     printf("捕获到信号%d\r\n", sig);
     /*释放资源*/

     quit_flag=1;

     for(int i=0;i<MAX_SOURCE_NBR;i++)
     {
         server[i]->quit = 1;
     }

//     exit(0);
}
