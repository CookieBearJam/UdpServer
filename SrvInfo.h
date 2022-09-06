//
// Created by shirley on 2022/8/22.
//
#pragma once

#ifndef JXL_SRVINFO_H
#define JXL_SRVINFO_H

#include <string>
#include <iostream>
#include <fstream>
#include <arpa/inet.h>
#include <cmath>
#include "json/json.h"

using namespace std;

#define DEFAULT_USER_CNT 10


//enum uint32_t {
//    PTDN_UNI, PTDN_VPN, PTDN_MUL
//};

//enum uint32_t {
//    PTDN_CLASS_00, PTDN_CLASS_01, PTDN_CLASS_10, PTDN_CLASS_11
//};

//enum REQ_TYPE {
//    REQUEST_USER_ALL, REQUEST_USER
//};

// 需要与ED客户端协定的请求类型：查询所有用户信息，查询单个用户信息
#define REQUEST_USER_ALL 10
#define REQUEST_USER 11


/** 用户业务信息类 **/
class SrvInfo {
private:
    uint32_t src_ip;        // 实现上用整型，但文件中是写的string，读取时再转换
    uint32_t dst_ip;        // 实现上用整型，但文件中是写的string，读取时再转换
    string dev_id;          // 设备名称，如"ed_1"
    uint32_t in_port;
    uint32_t srv_type;      // 0，1，2表示单播，VPN，组播
    uint32_t srv_class;     // 业务流类型，0，1，2，3
    uint32_t srv_num;       // 需要映射为整型，文件中存储十六进制表示的字符串（如010203，十进制值为66051），

public:
    uint32_t getSrcIp() const {
        return src_ip;
    }

    void setSrcIp(uint32_t srcIp) {
        src_ip = srcIp;
    }

    uint32_t getDstIp() const {
        return dst_ip;
    }

    void setDstIp(uint32_t dstIp) {
        dst_ip = dstIp;
    }

    const string &getDevId() const {
        return dev_id;
    }

    void setDevId(const string &devId) {
        dev_id = devId;
    }

    uint32_t getInPort() const {
        return in_port;
    }

    void setInPort(uint32_t inPort) {
        in_port = inPort;
    }

    uint32_t getSrvType() const {
        return srv_type;
    }

    void setSrvType(uint32_t srvType) {
        srv_type = srvType;
    }

    uint32_t getSrvClass() const {
        return srv_class;
    }

    void setSrvClass(uint32_t srvClass) {
        srv_class = srvClass;
    }

    uint32_t getSrvNum() const {
        return srv_num;
    }

    void setSrvNum(uint32_t srvNum) {
        srv_num = srvNum;
    }

    /** 控制台输出该对象信息 **/
    void printSrvInfo() const;

    /** 从from对象拷贝字段属性到本对象 **/
    void copyfrom(const SrvInfo &from);

    /** 对象信息转换为Json字符串 **/
    void toJsonStr(std::string *srvInfoJsonStr);

};

/** 业务数组类型**/
class SrvInfoList {
public:
    SrvInfo *list;
    int size; // 保持维护

    /** 初始化业务表，从文件service_info.json（filepath）读入
     * 默认传入userCnt值为0，不干预加载数量
     * userCnt大于0时，加载数量为MIN(userCnt，文件中用户数)
     * 文件中用户数为0时，初始化列表长度为
     * **/
    int init(const string &filepath, int userCnt);

    int getSrvInfoBySrc(uint32_t srcIP, SrvInfo *foundItem) const;

    // 根据源ip和目的ip查询用户信息并写入到指针参数中
    int getSrvInfoBySrcAndDst(uint32_t srcIP, uint32_t dstIP, SrvInfo *foundItem) const;

    /**
     * 将整个表（sevice_info.json中的数据）转换为json字符串
     * **/
    void toJsonStr(std::string *srvInfoJsonStr) const;

    static int clear();

    // todo:【实现】销毁init所创建的对象数组的析构函数
};


#endif //JXL_SRVINFO_H