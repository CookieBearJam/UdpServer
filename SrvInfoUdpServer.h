//
// Created by shirley on 2022/8/28.
//


/**
 * 此头文件定义查询用户业务信息的服务类
 * */


#ifndef JXL_SRVINFOUDPSERVER_H
#define JXL_SRVINFOUDPSERVER_H

#pragma once

#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <mutex> // 互斥变量头文件，暂时还没用，当前操作只有读

#include <thread>

#include "SrvInfo.h"

using namespace std;


#define BUFSIZE 2049
#define IP4MAXLEN 16    // 点分十进制表示的ip字符串最大长度（含有\0）

class SrvInfoUdpServer {
private:
    int serverID;
    int portNo;
    int serv_sock{};
public:
    // 静态成员变量计数
    static int serverCount;

    // 类内“信号量”，用于接收ctrl+c指令
    static bool terminated;

    // 同步变量：用户业务表
    static SrvInfoList srvInfoList;


    // 从文件中加载静态变量内容
    static int initSrvList(const string &filepath);

    explicit SrvInfoUdpServer(int _port = 8081) : portNo(_port) {
        // todo:check how many ports can be used, start from 8081
        portNo = _port + serverCount;
        serverID = ++serverCount;
        cout << "[Child]Create a server, id: " << serverID << " ,listening on local port [" << portNo << "]." << endl;
    }

    // 成功初始化返回0，否则返回-1
    int initServer();

    void startServer() const;

    ~SrvInfoUdpServer();
};


#endif //JXL_SRVINFOUDPSERVER_H
