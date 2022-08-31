#include <iostream>
#include "SrvInfoUdpServer.h"

#include <thread>
#include <csignal>

using namespace std;

#define BUFSIZE 2049
#define IP4MAXLEN 16    // 点分十进制表示的ip字符串最大长度（含有\0）
#define THREAD_NUM 2
#define CONFIG_FILE_PATH "../service_info.json"

void startService(std::mutex &syncReadLock) {
    syncReadLock.lock();

    SrvInfoUdpServer server;
    cout << "Thread " << this_thread::get_id() << "'s number of servers: " << SrvInfoUdpServer::serverCount << endl;
    // 初始化对象需要执行初始业务表格加载，其他对象只需要执行同步的读写
    if (SrvInfoUdpServer::serverCount == 1) {
        SrvInfoUdpServer::initSrvList(CONFIG_FILE_PATH);
    }

    syncReadLock.unlock();

    // 1.2 初始化server对象本身：创建socket等
    server.initServer();
    server.startServer();
}


// todo: 1.3需要设置销毁server的操作，也就是管理员的操作接口需要配置???
// 信号量处理函数：销毁线程，调用对象的析构
void signalHandler(int signum) {
    cout << "Interrupt signal (" << signum << ") received.\n";
    SrvInfoUdpServer::terminated = true;
    cout << "Value of terminated: " << SrvInfoUdpServer::terminated << endl;
//    exit(signum);
}


int main() {
    // 0. 创建线程池(两个)，分别创建一个udp服务器对象
    std::thread srvThreads[THREAD_NUM];
    // 一个公用的锁，目前只有读
    std::mutex syncReadLock;


//    // Register signal SIGINT and signal handler
//    signal(SIGINT, signalHandler);
    // 谨慎使用 signal, 尽量使用 sigaction
    struct sigaction sa;
    sa.sa_handler = signalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);


    for (auto &srvThread: srvThreads) {
        srvThread = std::thread(startService, std::ref(syncReadLock));
        if (srvThread.joinable()) {
            cout << "[Main] Created thread id: " << srvThread.get_id() << ", joinable." << endl;
            srvThread.join();
        } else {
            cout << "[Main] Created thread id: " << srvThread.get_id() << ", not joinable." << endl;
        }
    }

    cout << "[Main] Detected that child threads all finished, exit." << endl;
    return 0;
}