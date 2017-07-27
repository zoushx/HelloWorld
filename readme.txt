make -f Makefile.noGEV

Makefile.noGEV:

CC=g++
FLAGS=-Wall -pedantic -g -D__STDC_CONSTANT_MACROS -std=c++11
ULINKS=-lBasicUsageEnvironment -lUsageEnvironment -lliveMedia -lgroupsock -lx264 -lswscale -lswresample -lavformat -lavdevice -lavcodec -lavutil -lavfilter -lpostproc

INCPATH=/home/zou/work/RSTPServer/FFmpegLive555Lib/Rtsp/include/
LIBPATH=/home/zou/work/RSTPServer/FFmpegLive555Lib/Rtsp/lib/


INCLUDES=-I$(INCPATH)ffmpeg \
-I$(INCPATH)x264 \
-I$(INCPATH)live555/include/BasicUsageEnvironment \
-I$(INCPATH)live555/include/UsageEnvironment \
-I$(INCPATH)live555/include/liveMedia \
-I$(INCPATH)live555/include/groupsock


LIBS= -L$(LIBPATH)ffmpeg \
-L$(LIBPATH)x264


all:	FFmpegDecoder.o FFmpegH264Encoder.o FFmpegH264Source.o  LiveRTSPServer.o LiveServerMediaSubsession.o main.o 
		g++ -o spyPanda  FFmpegDecoder.o  FFmpegH264Encoder.o  FFmpegH264Source.o  LiveRTSPServer.o  LiveServerMediaSubsession.o  main.o $(LIBPATH)libliveMedia.a $(LIBPATH)libgroupsock.a $(LIBPATH)libBasicUsageEnvironment.a $(LIBPATH)libUsageEnvironment.a $(LIBS) -lswscale -lswresample -lavformat -lavdevice -lavcodec -lavutil -lavfilter -lpostproc -lx264 -pthread  -lrt -lm

FFmpegDecoder.o:	FFmpegDecoder.cpp
		$(CC) $(FLAGS) $(INCLUDES) FFmpegDecoder.cpp -c

FFmpegH264Encoder.o:	FFmpegH264Encoder.cpp
		$(CC) $(FLAGS) $(INCLUDES) FFmpegH264Encoder.cpp -c

FFmpegH264Source.o: FFmpegH264Source.cpp
		$(CC) $(FLAGS) $(INCLUDES) FFmpegH264Source.cpp -c

LiveRTSPServer.o: LiveRTSPServer.cpp
	$(CC) $(FLAGS) $(INCLUDES) LiveRTSPServer.cpp -c

LiveServerMediaSubsession.o: LiveServerMediaSubsession.cpp
	$(CC) $(FLAGS) $(INCLUDES) LiveServerMediaSubsession.cpp -c

main.o:	main.cpp
		$(CC) $(FLAGS) $(INCLUDES) main.cpp -c

once:
		$(CC) $(FLAGS)  $(INCLUDES) $(ULINKS) *.c *.cpp -o FFmpegRTSPServer

clean:
	rm -f *.o spyPanda


因为是动态链接ffmpeg中的swscale swresample avformat avdevice avcodec avutil avfilter postproc 和h264中的x264，所以在运行
./spyPanda
时候要制定动态链接库的路径：
export LD_LIBRARY_PATH=/home/zou/work/RSTPServer/FFmpegLive555Lib/Rtsp/lib/ffmpeg://home/zou/work/RSTPServer/FFmpegLive555Lib/Rtsp/lib/x264/:$LD_LIBRARY_PATH

每启动一个终端都要执行:
export LD_LIBRARY_PATH=/home/zou/work/RSTPServer/FFmpegLive555Lib/Rtsp/lib/ffmpeg://home/zou/work/RSTPServer/FFmpegLive555Lib/Rtsp/lib/x264/:$LD_LIBRARY_PATH


Live555 多路视频服务

服务端：考虑到服务端重启或多宿，为socket设置SO_REUSEADDR基本成为一个定律
客户端：客户端很少有必要bind端口，不bind时内核自动为你分配可用的端口

如果服务端的程序关闭后，端口不能马上释放掉，需要等一会才能小时，在这之间再启动服务程序是起不来的，但是可以用这个函数，边面这种情况，服务程序关闭后，可以马上再起一次，不会冲突了。
setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(&opt));

