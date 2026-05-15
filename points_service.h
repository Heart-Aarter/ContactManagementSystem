//
// Created by Codex on 2026/4/24.
//

#ifndef CONTACTMANAGEMENTSYSTEM_POINTS_SERVICE_H
#define CONTACTMANAGEMENTSYSTEM_POINTS_SERVICE_H

#include "model.h"

int doAddPoint(const char* pName, const char* pPwd, PointChangeRecord* pPointInfo);
int doRefundPoint(const char* pName, const char* pPwd, PointChangeRecord* pPointInfo);
int doDonatePoint(const char* pName, const char* pPwd, PointChangeRecord* pPointInfo);

#endif //CONTACTMANAGEMENTSYSTEM_POINTS_SERVICE_H
