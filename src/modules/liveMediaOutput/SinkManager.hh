/*
 *  SinkManager.hh - Class that handles multiple server sessions dynamically.
 *  Copyright (C) 2014  Fundació i2CAT, Internet i Innovació digital a Catalunya
 *
 *  This file is part of liveMediaStreamer.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Authors:  David Cassany <david.cassany@i2cat.net>,
 *            
 */
#ifndef _SINK_MANAGER_HH
#define _SINK_MANAGER_HH

#include "QueueSource.hh"
#include "../../Filter.hh"
#include "../../IOInterface.hh"

#include <BasicUsageEnvironment.hh>
#include <liveMedia.hh>
#include <Groupsock.hh>
#include <map>
#include <string>

#define RTSP_PORT 8554
#define MAX_VIDEO_FRAME_SIZE 256*1024
#define MANUAL_CLIENT_SESSION_ID 1
#define TTL 255
#define INITIAL_SERVER_PORT 6970

class Connection;

class SinkManager : public TailFilter {
private:
    SinkManager(int readersNum = MAX_READERS);
    ~SinkManager();
    
public:
    static SinkManager* getInstance();
    static void destroyInstance();
    
    

    bool addSession(std::string id, std::vector<int> readers, 
                    std::string info = "", std::string desc = "");
    bool addConnection(int reader, std::string ip, unsigned int port);
    
    ServerMediaSession* getSession(std::string id); 
    bool publishSession(std::string id);
    bool removeSession(std::string id);
    bool removeSessionByReaderId(int readerId);
    bool deleteReader(int id);
    
    void stop();
    
    UsageEnvironment* envir() {return env;}
      
private: 
    void initializeEventMap();
    void addSessionEvent(Jzon::Node* params, Jzon::Object &outputNode);
    Reader *setReader(int readerID, FrameQueue* queue);
    
    bool processFrame(bool removeFrame = false);
    
    ServerMediaSubsession *createSubsessionByReader(int readerId);
    ServerMediaSubsession *createVideoMediaSubsession(VCodecType codec, int readerId);
    ServerMediaSubsession *createAudioMediaSubsession(ACodecType codec, 
                                                      unsigned channels,
                                                      unsigned sampleRate,
                                                      SampleFmt sampleFormat, int readerId);
    void createVideoQueueSource(VCodecType codec, Reader *reader, int readerId);
    void createAudioQueueSource(ACodecType codec, Reader *reader, int readerId);
    void doGetState(Jzon::Object &filterNode);
   
    static SinkManager* mngrInstance;
    std::map<std::string, ServerMediaSession*> sessionList;
    std::map<int, StreamReplicator*> replicas;
    std::map<int, Connection*> connections;
    UsageEnvironment* env;
    uint8_t watch;
    
    RTSPServer* rtspServer;
};

class Connection {
    
public:
    void startPlaying();
    void stopPlaying();
    ~Connection();
    
protected:
    Connection(UsageEnvironment* env, std::string ip, 
               unsigned port, FramedSource *source);
    static void afterPlaying(void* clientData);
    
    
    UsageEnvironment* fEnv;
    std::string fIp;
    unsigned fPort;
    FramedSource *fSource;
    
    
    struct in_addr destinationAddress;
    RTPSink *sink;
    RTCPInstance* rtcp;
    Groupsock *rtpGroupsock;
    Groupsock *rtcpGroupsock;
};

class VideoConnection : public Connection{   
public:
    VideoConnection(UsageEnvironment* env, 
                    std::string ip, unsigned port, 
                    FramedSource *source, VCodecType codec);

private:
    VCodecType fCodec;
};

// class AudioConnection {   
// private:
//     AudioConnection(UsageEnvironment* env, std::string ip, unsigned port, 
//                     FramedSource *source, ACodecType codec,
//                     unsigned channels, unsigned sampleRate,
//                     SampleFmt sampleFormat);
//     
//     ACodecType fCodec;
//     unsigned fChannels;
//     unsigned fSampleRate;
//     SampleFmt fSampleFormat;
// };

#endif
