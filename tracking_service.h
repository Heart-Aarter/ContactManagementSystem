//
// Created by lenovo on 2026/4/18.
//

#ifndef CONTACTMANAGEMENTSYSTEM_TRACKING_SERVICE_H
#define CONTACTMANAGEMENTSYSTEM_TRACKING_SERVICE_H

#include "model.h"
#include "global.h"

void initTrackingList();
void releaseTrackingList();
Tracking* queryTracking(const char* pName, int* pIndex);
int updateTrackingInfo(const Tracking* pTracking, int nIndex);
double calculatePoints(time_t tStart, time_t tReg);
int startService(const char* pName, const char* pPwd, Start* pInfo);
int endService(const char* pName, const char* pPwd, End* pInfo);
int loadTracking();

#endif //CONTACTMANAGEMENTSYSTEM_TRACKING_SERVICE_H
