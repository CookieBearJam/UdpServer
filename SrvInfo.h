//
// Created by shirley on 2022/8/22.
//

#ifndef JXL_SRVINFO_H
#define JXL_SRVINFO_H

#include <string>
#include <iostream>
#include <fstream>
#include "json/json.h"

using namespace std;


#define DEFAULT_USER_CNT 10
#define REQUEST_NMS_ONE "REQUEST_NMS_ONE"
enum SRV_CLASS {
    PTDN_CLASS_00, PTDN_CLASS_01, PTDN_CLASS_10, PTDN_CLASS_11
};


// 定义的数据结构
class SrvInfo {
private:
    string src_ip;
    string dst_ip;      // 可以用整型来存，但是文件里必须要是
    string dev_id;
    int in_port;
    string srv_type;
    string srv_class;  // 需要映射为整型
    string srv_num; // 十六进制表示的字符串（010203）

public:
    const string &getSrcIp() const {
        return src_ip;
    }

    void setSrcIp(const string &srcIp) {
        src_ip = srcIp;
    }

    const string &getDstIp() const {
        return dst_ip;
    }

    void setDstIp(const string &dstIp) {
        dst_ip = dstIp;
    }

    const string &getDevId() const {
        return dev_id;
    }

    void setDevId(const string &devId) {
        dev_id = devId;
    }

    int getInPort() const {
        return in_port;
    }

    void setInPort(int inPort) {
        in_port = inPort;
    }

    const string &getSrvType() const {
        return srv_type;
    }

    void setSrvType(const string &srvType) {
        srv_type = srvType;
    }

    const string &getSrvClass() const {
        return srv_class;
    }

    void setSrvClass(const string &srvClass) {
        srv_class = srvClass;
    }

    const string &getSrvNum() const {
        return srv_num;
    }

    void setSrvNum(const string &srvNum) {
        srv_num = srvNum;
    }

    void printSrvInfo() const;

    void copyfrom(const SrvInfo &from);

    void toJsonStr(std::string *srvInfoJsonStr);

};

// 业务数组类型
class SrvInfoList {
public:
    SrvInfo *list;
    int size; // always remember to maintain this

    /* 初始化从文件导入用户业务信息，创建初始数量的servInfo对象，
 * 返回一个对象数组，存放json文件中的对象内容
 * 如果文件中没有user，默认初始化10，
 * 通过参数指定初始化对象数组大小
 * 。。。哪里创建哪里释放。。。。
 * */
    int init(const string &filepath, int userCnt);


    // 根据源ip和目的ip查询用户信息并写入到指针参数中
    int getSrvInfoBySrcAndDst(const string &srcIP, const string &dstIP, SrvInfo *foundItem);

    // 将对象内容转换为json字符串，如果index已经被指定，则只导出列表中的某一个用户（也就是查到的用户）
    void toJsonStr(std::string *srvInfoJsonStr);

    /*
     * todo:【实现】销毁刚才所创建的对象数组
     * */
    static int clear();
};


#endif //JXL_SRVINFO_H