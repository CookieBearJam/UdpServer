//
// Created by shirley on 2022/8/22.
//
#include "SrvInfo.h"

void SrvInfo::printSrvInfo() const {
    cout << "====================================" << endl;
    cout << "src_ip:" << this->getSrcIp() << endl;
    cout << "dst_ip:" << this->getDstIp() << endl;
    cout << "dev_id:" << this->getDevId() << endl;
    cout << "in_port:" << this->getInPort() << endl;
    cout << "srv_type:" << this->getSrvType() << endl;
    cout << "srv_class:" << this->getSrvClass() << endl;
    cout << "srv_num:" << this->getSrvNum() << endl;
    cout << "====================================" << endl;
}

void SrvInfo::copyfrom(const SrvInfo &from) {
    this->setSrcIp(from.getSrcIp());
    this->setDstIp(from.getDstIp());
    this->setDevId(from.getDevId());
    this->setInPort(from.getInPort());
    this->setSrvType(from.getSrvType());
    this->setSrvClass(from.getSrvClass());
    this->setSrvNum(from.getSrvNum());
}


// 直接返回此用户的字符串
// todo:string需要指定为引用类型吗
void SrvInfo::toJsonStr(std::string *srvInfoJsonStr) {
    Json::Value root, content, srvInfo;
    root["type"] = "REQUEST_NMS_ONE";

    srvInfo["src_ip"] = this->getSrcIp();
    srvInfo["dst_ip"] = this->getDstIp();
    srvInfo["dev_id"] = this->getDevId();
    srvInfo["in_port"] = this->getInPort();
    srvInfo["srv_type"] = this->getSrvType();
    srvInfo["srv_class"] = this->getSrvClass();
    srvInfo["srv_num"] = this->getSrvNum();

    content["user_cnt"] = 1;
    content["user"] = srvInfo;
    root["content"] = content;

    *srvInfoJsonStr = root.toStyledString();
    root.clear();

    std::cout << "Json:\n" << *srvInfoJsonStr << std::endl;
}

/*
 * 注意在解析文件的时候一定要写对user_cnt
 * */
int SrvInfoList::init(const string &filepath, int userCnt) {
    if (userCnt < 0) return -1;

    // std::ios::binary指定了二进制的读取方式，完整的读取所有的数据
    Json::Value jsonRoot;
    // 启用严格模式，当解析非法Json时返回false,不再自动容错
    Json::Reader jsonReader(Json::Features::strictMode());

    ifstream ifs(filepath, ios::binary);
    if (!ifs.is_open()) {
        cout << "Error opening service_info file.\n";
        return -2;
    }

    if (!jsonReader.parse(ifs, jsonRoot)) {
        cout << "Error parsing content from service_info.json.\n";
        ifs.close();
        return -3;
    }

    // 读取根节点信息
    int user_cnt = jsonRoot["user_cnt"].asInt();

    if (user_cnt == 0) {
        cout << "user_cnt in file equals to 0." << endl;
        //  若外界指定大小也是0，则取默认值
        user_cnt = (userCnt == 0) ? DEFAULT_USER_CNT : userCnt;
        this->list = new SrvInfo[user_cnt];
        this->size = user_cnt;
        // todo：整个finally来关文件
        ifs.close();
        return 1;
    } else {
        // 初始化内容不超过文件中的用户数
        if (userCnt > 0) user_cnt = userCnt > user_cnt ? user_cnt : userCnt;

        this->list = new SrvInfo[user_cnt];
        for (int i = 0; i < user_cnt; i++) {
            this->list[i].setSrcIp(jsonRoot["users"][i]["src_ip"].asString());
            this->list[i].setDstIp(jsonRoot["users"][i]["dst_ip"].asString());
            this->list[i].setDevId(jsonRoot["users"][i]["dev_id"].asString());
            this->list[i].setInPort(jsonRoot["users"][i]["in_port"].asInt());
            this->list[i].setSrvType(jsonRoot["users"][i]["srv_type"].asString());
            this->list[i].setSrvClass(jsonRoot["users"][i]["srv_class"].asString());
            this->list[i].setSrvNum(jsonRoot["users"][i]["srv_num"].asString());
        }
    }

    this->size = user_cnt;

    // todo：整个finally来关文件
    ifs.close();
    return 0;
}

int SrvInfoList::clear() {
    return 0;
}


// 返回值-1表示没有找到符合的用户信息，查找成功则返回0
int SrvInfoList::getSrvInfoBySrcAndDst(const string &srcIP, const string &dstIP, SrvInfo *foundItem) {
    for (int i = 0; i < this->size; i++) {
        if (srcIP == this->list[i].getSrcIp() && dstIP == this->list[i].getDstIp()) {
            foundItem->copyfrom(this->list[i]);
            return 0;
        }
    }
    return -1;
}

void SrvInfoList::toJsonStr(std::string *srvInfoJsonStr) {
    Json::Value root, content, srvInfo;
    root["type"] = "REQUEST_NMS_ALL";

    for (int i = 0; i < this->size; i++) {
        srvInfo.clear();
        srvInfo["src_ip"] = this->list[i].getSrcIp();
        srvInfo["dst_ip"] = this->list[i].getDstIp();
        srvInfo["dev_id"] = this->list[i].getDevId();
        srvInfo["in_port"] = this->list[i].getSrcIp();
        srvInfo["srv_type"] = this->list[i].getDstIp();
        srvInfo["srv_class"] = this->list[i].getDevId();
        srvInfo["srv_num"] = this->list[i].getSrvNum();
        content["users"][i] = srvInfo;
    }

    content["user_cnt"] = this->size;
    root["content"] = content;

    *srvInfoJsonStr = root.toStyledString();
    root.clear();

    std::cout << "Json string of all items:\n" << srvInfoJsonStr << std::endl;
}




