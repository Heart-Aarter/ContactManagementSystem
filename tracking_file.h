//
// Created by lenovo on 2026/4/18.
//

#ifndef CONTACTMANAGEMENTSYSTEM_TRACKING_FILE_H
#define CONTACTMANAGEMENTSYSTEM_TRACKING_FILE_H

#include "model.h"
#include "global.h"
#include <iostream>
#include <fstream>

int saveTracking(const Tracking* pTracking, const char* pPath);
int getTrackingCount(const char* pPath);
int readTracking(Tracking* pTracking, const char* pPath);
int updateTracking(const Tracking* pTracking, const char* pPath, int nIndex);

#endif //CONTACTMANAGEMENTSYSTEM_TRACKING_FILE_H
