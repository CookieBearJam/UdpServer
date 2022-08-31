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
    unsigned short tmpShort = 0;
    char rcvBuf[BUFSIZE], sendBuf[BUFSIZE]; // 收到报文
    char reqSrcIP[IP4MAXLEN], reqDstIP[IP4MAXLEN];
    std::string cli, echo_string; //

    // 收到和发出的报文总长度
    ssize_t rcvLen, sendLen;
    // 收到查询报文中的源目ip地址长度
    int reqSrcLen, reqDstLen;
    auto *foundItem = new SrvInfo();


    while (!terminated) {
        memset(rcvBuf, 0, BUFSIZE);
        memset(sendBuf, 0, BUFSIZE);
        memset(reqSrcIP, 0, IP4MAXLEN);
        memset(reqDstIP, 0, IP4MAXLEN);

        // todo:recvmsg?哪个更高效？
        // todo:这里是阻塞式的，所以需要一直等待返回
        rcvLen = recvfrom(serv_sock, rcvBuf, BUFSIZE, 0, (struct sockaddr *) &client_addr, &sin_size);
        if (rcvLen < 0) {
            perror("Recvfrom() error");
            // todo:exit和return？？？
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
        // todo：假设现在是前两个字节是源ip的长度（short），后续的几个字节是源ip的内容
        //  一个字符表示的长度，也就是可以表示0~255长度的ip地址（字符形式的'1'，ascii=48）
        memcpy(&tmpShort, rcvBuf, 1);
        reqSrcLen = tmpShort - 48;
        memcpy(reqSrcIP, &rcvBuf[1], reqSrcLen);

        memcpy(&tmpShort, &rcvBuf[1 + reqSrcLen], 1);
        reqDstLen = tmpShort - 48;
        memcpy(reqDstIP, &rcvBuf[2 + reqSrcLen], reqDstLen);

        // todo:加锁，保证不同的server对象访问期间，只有一个线程中的对象可以修改类的静态变量
        if (srvInfoList.getSrvInfoBySrcAndDst(reqSrcIP, reqDstIP, foundItem) == 0) {
            cout << "Find service info successfully.Preparing to send back." << endl;
            foundItem->toJsonStr(&echo_string);
        } else {
            cout << "Cannot find requested item,return null." << endl;
            sendBuf[0] = '\0';
            echo_string = sendBuf;//todo:这个从char[]到string的转换。。。？
        }

        // todo:发送的字节数是否包括最后一个\0？
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


