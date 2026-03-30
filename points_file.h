//
// Created by Codex on 2026/4/24.
//

#ifndef CONTACTMANAGEMENTSYSTEM_POINTS_FILE_H
#define CONTACTMANAGEMENTSYSTEM_POINTS_FILE_H

#include "global.h"
#include "model.h"

int savePoint(const PointChange* pPoint, const char* pPath);
int getPointCount(const char* pPath);
int readPoint(PointChange* pPoint, const char* pPath);

#endif //CONTACTMANAGEMENTSYSTEM_POINTS_FILE_H
