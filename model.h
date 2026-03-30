//
// Created by lenovo on 2026/3/31.
//

#ifndef CONTACTMANAGEMENTSYSTEM_MODEL_H
#define CONTACTMANAGEMENTSYSTEM_MODEL_H

#include <ctime>

typedef struct Account {    //志愿者信息结构体
    char aName[18];         //账号名称
    char aPwd[8];           //账号密码
    int nStatus;            //账号状态
    time_t tStart;          //注册时间
    time_t tEnd;            //截止时间
    float fBalance;         //积分余额
    float fTotalPoints;     //累计积分
    float fTotalDuration;   //累计时长
    time_t tLast;           //最后一次使用时间
    int nUseCount;          //使用次数
    int nDel;               //删除标识(0-未删除,1-已删除)
}Account;

typedef struct AccountNode {//志愿者链表结点
    Account data;           //志愿者信息
    AccountNode* next;      //指向下一个结点的指针
}AccountNode,*lpAccountNode;

typedef struct Point {      //志愿者服务积分信息结构体
    char aName[18];         //账号名称
    time_t tStart;          //开始服务时间
    time_t tEnd;            //结束服务时间
    float fAmount;         //积分
    int nDel;              //删除状态
    int nStatus;           //是否正在服务状态
}Point;

typedef struct PointsNode { //志愿者服务积分链表结点
    Point data;             //志愿者服务积分信息
    struct PointsNode* next;//指向下一个结点的指针
}PointsNode,*lpPonintsNode;

typedef struct Start {      //开始服务信息结构体
    char aName[18];
    time_t tLogon;
    double fBalance;
}Start;

typedef struct End {        //结束服务信息结构体
    char aName[18];
    time_t tStart;
    time_t tEnd;
    float fAmount;
    float fBalance;
}End;

typedef struct Tracking {   //服务追踪信息结构体
    char aName[18];
    time_t tStart;
    time_t tEnd;
    int nDel;
    int nStatus;
    float fAmount;
}Tracking;

typedef struct TrackingNode {
    Tracking data;
    struct TrackingNode* next;
}TrackingNode,*lpTrackingNode;

typedef struct PointChange {
    char aAccountName[18];
    time_t tTime;
    int nStatus;       // 0-add, 1-use, 2-donate
    float fChange;
    int nDel;
} PointChange;

typedef struct PointChangeRecord {
    char aAccountName[18];
    float fChange;
    float fBalance;
} PointChangeRecord;

typedef struct StatisticsInfo {
    int nAccountCount;
    int nNormalAccountCount;
    int nServingAccountCount;
    int nAnnulledAccountCount;
    float fTotalBalance;
    float fTotalPoints;
    float fTotalDuration;
    int nTotalUseCount;

    int nTrackingCount;
    int nRunningTrackingCount;
    int nCompletedTrackingCount;

    int nPointRecordCount;
    float fAddedPoints;
    float fUsedPoints;
    float fDonatedPoints;
} StatisticsInfo;

#endif //CONTACTMANAGEMENTSYSTEM_MODEL_H
