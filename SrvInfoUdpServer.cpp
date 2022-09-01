//
// Created by shirley on 2022/8/29.
//

#include "SrvInfoUdpServer.h"

// Important:初始化静态成员变量，不允许放在头文件
int SrvInfoUdpServer::serverCount = 0;

bool SrvInfoUdpServer::terminated = false;

SrvInfoList SrvInfoUdpServer::srvInfoList{};

/**
 * 初始化业务表，从文件中加载
 * */
int SrvInfoUdpServer::initSrvList(const string &filepath) {
    switch (srvInfoList.init(filepath, 0)) {
        case 0:
            cout << "SrvList initialized success!" << endl;
            for (int i = 0; i < srvInfoList.size; i++) {
                srvInfoList.list[i].printSrvInfo();
            }
            return 0;
        case 1:
            cout << "SrvList initialized with default size[" << DEFAULT_USER_CNT << "]!" << endl;
            return 1;
        default:
            cout << "Load failed!" << endl;
            return -1;
    }
}


/**
 * 初始化方法：创建监听客户端的socket，绑定地址和端口等信息
 */
int SrvInfoUdpServer::initServer() {
    struct sockaddr_in serv_addr{};

    serv_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (serv_sock < 0) {
        cout << "create socket() error" << endl;
        return -1;
    }

    bzero(&serv_addr, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // 监听来自所有地址的报文，todo：控制只监听ED的报文
    serv_addr.sin_port = htons(portNo);

    // todo:？？第三个参数的sizeof sockaddr 和sizeof(sockaddr_in)哪个更合适？
    if (bind(serv_sock, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr)) == -1) {
        cout << "bind() error" << endl;
        close(serv_sock);
        return -1;
    } else {
        cout << "Binding to port:" << portNo << endl;
    }
    return 0;
}

void SrvInfoUdpServer::startServer() const {
    struct sockaddr_in client_addr{};
    socklen_t sin_size = sizeof(struct sockaddr_in);
    char rcvBuf[BUFSIZE]; // 收到、发出的报文
//    char reqSrcIP[IP4MAXLEN], reqDstIP[IP4MAXLEN];
    uint32_t reqSrcIP, reqDstIP;
    std::string cli, echo_string; // 客户端信息、返回的json串

    // 收到和发出的报文总长度
    ssize_t rcvLen, sendLen;
    auto *foundItem = new SrvInfo();

    Json::Reader JsonReader;
    Json::Value JsonRoot, reqContent;
    int reqType;    // 请求报文类型

    while (!terminated) {
        memset(rcvBuf, 0, BUFSIZE);

        JsonRoot.clear();
        reqContent.clear();

        rcvLen = recvfrom(serv_sock, rcvBuf, BUFSIZE, 0, (struct sockaddr *) &client_addr, &sin_size);
        if (rcvLen < 0) {
            perror("Recvfrom() error");
            exit(-1);
        } else {
            // 发送端的ip地址，将四字节整型转成点分十进制字符串
            cli = inet_ntoa(client_addr.sin_addr);
            // 发送端端口号，将网络字节序转为主机字节序，并且再转成字符串
            cli += ":" + to_string(ntohs(client_addr.sin_port));
            rcvBuf[rcvLen] = '\0';
            cout << "===================================================" << endl;
            std::cout << "Thread[" << this_thread::get_id() << "], port[" << portNo << "]" << endl;
            std::cout << "Receiving from client: " << cli << ",message >>>>" << rcvBuf << std::endl;
        }

        // 3. 解析收到的消息内容
        if (JsonReader.parse(rcvBuf, JsonRoot)) {
            reqType = JsonRoot["type"].asInt();
            cout << "Rcv request type:" << reqType << endl;
        } else {
            cout << "Failed to parse request type" << endl;
            continue;
        }

        if (reqType == REQUEST_USER_ALL) {
            srvInfoList.toJsonStr(&echo_string);
        } else if (reqType == REQUEST_USER) {
            reqContent = JsonRoot["content"];
            // 解析json中的源和目的ip字段
            reqSrcIP = reqContent["src_ip"].asInt();
            reqDstIP = reqContent["dst_ip"].asInt();

/**            memcpy(&tmpShort, rcvBuf, 1);
            reqSrcLen = tmpShort - 48;
            memcpy(reqSrcIP, &rcvBuf[1], reqSrcLen);

            memcpy(&tmpShort, &rcvBuf[1 + reqSrcLen], 1);
            reqDstLen = tmpShort - 48;
            memcpy(reqDstIP, &rcvBuf[2 + reqSrcLen], reqDstLen);
            **/

            // todo:加锁，保证不同的server对象访问期间，只有一个线程中的对象可以修改类的静态变量
            if (srvInfoList.getSrvInfoBySrcAndDst(reqSrcIP, reqDstIP, foundItem) == 0) {
                cout << "Find service info successfully.Preparing to send back." << endl;
                foundItem->toJsonStr(&echo_string);
            } else {
                cout << "Cannot find requested item, return empty string." << endl;
            }
        }

        sendLen = sendto(serv_sock, echo_string.c_str(), echo_string.size(), 0, (struct sockaddr *) &client_addr,
                         sin_size);
        if (sendLen < 0) {
            perror("Sendto() error");
        } else {
            cout << "Sent " << sendLen << " bytes to client successfully!" << endl;
        }
    }
}

SrvInfoUdpServer::~SrvInfoUdpServer() {
    cout << "Destroy a SrvInfoUdpServer object, No:" << serverID << endl;
    close(serv_sock);
    if (--serverCount == 0) {
        cout << "Closed all socket on server.Service unavailable." << endl;
    }
}