//
// Created by lenovo on 2026/3/31.
//

#ifndef CONTACTMANAGEMENTSYSTEM_MODEL_H
#define CONTACTMANAGEMENTSYSTEM_MODEL_H

#include <ctime>

#include "global.h"

typedef struct Account {
    char aName[ACCOUNT_NAME_LENGTH];    // max 18 chars + '\0'
    char aPwd[PASSWORD_LENGTH];         // max 8 chars + '\0'
    int nStatus;
    time_t tStart;
    time_t tEnd;
    float fBalance;
    float fTotalPoints;
    float fTotalDuration;
    time_t tLast;
    int nUseCount;
    int nDel;
} Account;

typedef struct AccountNode {
    Account data;
    AccountNode* next;
} AccountNode, *lpAccountNode;

typedef struct Point {
    char aName[ACCOUNT_NAME_LENGTH];
    time_t tStart;
    time_t tEnd;
    float fAmount;
    int nDel;
    int nStatus;
} Point;

typedef struct PointsNode {
    Point data;
    struct PointsNode* next;
} PointsNode, *lpPonintsNode;

typedef struct Start {
    char aName[ACCOUNT_NAME_LENGTH];
    time_t tLogon;
    double fBalance;
} Start;

typedef struct End {
    char aName[ACCOUNT_NAME_LENGTH];
    time_t tStart;
    time_t tEnd;
    float fAmount;
    float fBalance;
} End;

typedef struct Tracking {
    char aName[ACCOUNT_NAME_LENGTH];
    time_t tStart;
    time_t tEnd;
    int nDel;
    int nStatus;
    float fAmount;
} Tracking;

typedef struct TrackingNode {
    Tracking data;
    struct TrackingNode* next;
} TrackingNode, *lpTrackingNode;

typedef struct PointChange {
    char aAccountName[ACCOUNT_NAME_LENGTH];
    time_t tTime;
    int nStatus;       // 0-add, 1-use, 2-donate
    float fChange;
    int nDel;
} PointChange;

typedef struct PointChangeRecord {
    char aAccountName[ACCOUNT_NAME_LENGTH];
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
