//
// Created by lenovo on 2026/4/10.
//

#ifndef CONTACTMANAGEMENTSYSTEM_SERVICE_H
#define CONTACTMANAGEMENTSYSTEM_SERVICE_H

#include "model.h"

bool addAccountInfo(Account account);
Account* queryAccountInfo(const char* pName,int* pIndex);
Account* queryAllAccountInfo(int* pCount);
int startServiceInfo(const char* pName, const char* pPwd, Start* pInfo);
int endServiceInfo(const char* pName, const char* pPwd, End* pInfo);
int doAddPointInfo(const char* pName, const char* pPwd, PointChangeRecord* pPointInfo);
int doRefundPointInfo(const char* pName, const char* pPwd, PointChangeRecord* pPointInfo);
int annulAccount(Account* pAccount);
int getStatisticsInfo(StatisticsInfo* pInfo);

#endif //CONTACTMANAGEMENTSYSTEM_SERVICE_H
