//
// Created by lenovo on 2026/4/18.
//

#include "tracking_service.h"
#include <cstring>

#include "account_file.h"
#include "account_service.h"
#include "tracking_file.h"


lpTrackingNode trackingList = nullptr;

/**************************************************************
[函数名] startService
[函数功能] 进行开始服务操作
[函数参数] pName:志愿者账号号
         pPwd:密码
         pInfo:开始服务的信息
[返回值] int
***************************************************************/

int startService(const char* pName, const char* pPwd, Start* pInfo) {
    Account* pAccount = nullptr;
    int nIndex = 0;
    Tracking tracking;

    pAccount = checkAccount(pName, pPwd, &nIndex);

    if(pAccount == nullptr)
    {
        return FALSE;
    }

    if(pAccount->nStatus != 0)
    {
        return UNUSE;
    }

    pAccount->nStatus = 1;
    pAccount->tLast = time(nullptr);
    pAccount->nUseCount ++;

    if(FALSE == updateAccount(pAccount, ACCOUNTPATH, nIndex))
    {
        return FALSE;
    }

    strcpy(tracking.aName, pName);
    tracking.tStart = time(nullptr);
    tracking.tEnd = time(nullptr);
    tracking.fAmount = 0;
    tracking.nStatus = 0;
    tracking.nDel = 0;

    if(FALSE == saveTracking(&tracking, TRACKINGPATH))
    {
        return FALSE;
    }

    strcpy(pInfo->aName, pName);
    pInfo->tLogon = time(nullptr);
    pInfo->fBalance = pAccount->fBalance;

    return TRUE;
}

/**************************************************************
[函数名] endService
[函数功能] 进行结束服务操作
[函数参数] pName:志愿者账号号
         pPwd:密码
         pInfo:开始服务的信息
[返回值] int
***************************************************************/

int endService(const char* pName, const char* pPwd, End* pInfo) {
    Account *pAccount = nullptr;
    Tracking *pTracking = nullptr;
    int nIndex = 0;
    int nPosition = 0;
    double dbPoints = 0.0;
    float fBalance = 0.0;
    double dbDuration = 0.0;

    pAccount = checkAccount(pName, pPwd, &nIndex);

    if (pAccount == nullptr) {
        return FALSE;
    }

    if (pAccount->nStatus != 1) {
        return UNUSE;
    }

    pTracking = queryTracking(pName, &nPosition);

    if (pTracking == nullptr) {
        return FALSE;
    }

    dbPoints = calculatePoints(pTracking->tStart, pAccount->tStart);

    fBalance = pAccount->fBalance + (float) dbPoints;

    dbDuration = (double) (time(nullptr) - pTracking->tStart) / 3600.0;

    pAccount->fBalance = fBalance;
    pAccount->fTotalPoints += (float) dbPoints;
    pAccount->fTotalDuration += (float) dbDuration;
    pAccount->nStatus = 0;
    pAccount->tLast = time(nullptr);

    if (FALSE == updateAccount(pAccount, ACCOUNTPATH, nIndex)) {
        return FALSE;
    }

    pTracking->fAmount = (float) dbPoints;
    pTracking->nStatus = 1;
    pTracking->tEnd = time(nullptr);

    if (FALSE == updateTrackingInfo(pTracking, nPosition)) {
        return FALSE;
    }

    strcpy(pInfo->aName, pName);
    pInfo->fAmount = (float) dbPoints;
    pInfo->fBalance = fBalance;
    pInfo->tStart = pTracking->tStart;
    pInfo->tEnd = time(nullptr);

    return TRUE;
}

/**************************************************************
[函数名] queryTracking
[函数功能] 在服务追踪记录链表中，查询对应志愿者账号号的记录，并获取该记录在链表中的索
引号
[函数参数] pName:志愿者账号号
         pIndex:查询到的服务追踪记录在链表中的索引号
[返回值] Tracking*
***************************************************************/

Tracking* queryTracking(const char* pName, int* pIndex) {
    static Tracking tracking;
    Tracking* pTracking = nullptr;
    int nCount = 0;
    int nReadCount = 0;

    if (pName == nullptr || pIndex == nullptr) {
        return nullptr;
    }

    nCount = getTrackingCount(TRACKINGPATH);
    if (nCount <= 0) {
        return nullptr;
    }

    pTracking = (Tracking*) malloc(sizeof(Tracking) * nCount);
    if (pTracking == nullptr) {
        return nullptr;
    }

    nReadCount = readTracking(pTracking, TRACKINGPATH);
    if (nReadCount <= 0) {
        free(pTracking);
        return nullptr;
    }

    for (int i = 0; i < nReadCount; i++) {
        if(strcmp(pTracking[i].aName, pName) == 0 && pTracking[i].nStatus == 0) {
            tracking = pTracking[i];
            *pIndex = i;
            free(pTracking);
            return &tracking;
        }
    }

    free(pTracking);
    return nullptr;
}

/**************************************************************
[函数名]： loadTracking
[函数功能]：从服务追踪记录文件中，获取服务追踪记录保存到链表中
[函数参数]：void
[返回值]：int: 获取信息成功，获取信息失败
***************************************************************/
int loadTracking() {
    int nCount = 0;
    Tracking *pTracking = nullptr;
    lpTrackingNode node = nullptr;
    int i = 0;
    lpTrackingNode cur = nullptr;

    if (trackingList != nullptr) {
        releaseTrackingList();
    }

    initTrackingList();
    nCount = getTrackingCount(TRACKINGPATH);

    pTracking = (Tracking *) malloc(sizeof(Tracking) * nCount);
    if (pTracking != nullptr) {
        if (FALSE == readTracking(pTracking, TRACKINGPATH)) {
            free(pTracking);
            return FALSE;
        }

        for (i = 0, node = trackingList; i < nCount; i++) {
            cur = (lpTrackingNode) malloc(sizeof(TrackingNode));
            if (cur == nullptr) {
                free(pTracking);
                return FALSE;
            }

            memset(cur, 0, sizeof(TrackingNode));

            cur->data = pTracking[i];
            cur->next = nullptr;

            node->next = cur;
            node = cur;
        }
        free(pTracking);
        return TRUE;
    }
    return FALSE;
}

/**************************************************************
[函数名]： initTrackingList
[函数功能]：初始化服务追踪记录链表头文件，为头结点分配内存
[函数参数]：void
[返回值]：void
***************************************************************/

void initTrackingList() {
    lpTrackingNode head = nullptr;
    if (trackingList == nullptr) {
        head = (lpTrackingNode) malloc(sizeof(TrackingNode));

        head->next = nullptr;
        trackingList = head;
    }
}

/**************************************************************
[函数名]： releaseTrackingList
[函数功能]：释放服务追踪记录链表
[函数参数]：void
[返回值]：void
***************************************************************/

void releaseTrackingList() {
    lpTrackingNode cur = trackingList;
    lpTrackingNode next = nullptr;
    while (cur != nullptr) {
        next = cur->next;
        free(cur);
        cur = next;
    }
    trackingList = nullptr;
}

/**************************************************************
[函数名]： calculatePoints
[函数功能]：根据服务时间，计算获得的积分
[函数参数]：tStart:开始服务时间
        tReg: 注册时间
[返回值]：double:获得的积分
***************************************************************/

double calculatePoints(time_t tStart, time_t tReg) {
    double dbPoints = 0;
    int nCount = 0;
    int nSec = 0;
    int nMinutes = 0;
    time_t tEnd = time(nullptr);
    double fRate = 1.0;

    nSec = (int) (tEnd - tStart);
    nMinutes = nSec / 60;

    if (nMinutes % UNIT == 0) {
        nCount = nMinutes / UNIT;
    } else {
        nCount = nMinutes / UNIT + 1;
    }

    if (tEnd - tReg < NEWCOMER_DAYS * 24 * 3600) {
        fRate *= NEWCOMER_RATE;
    }

    struct tm *pTime = localtime(&tEnd);
    if (pTime->tm_wday == 0 || pTime->tm_wday == 6) {
        fRate *= WEEKEND_RATE;
    }

    dbPoints = nCount * POINT_RATE * fRate;

    return dbPoints;
}

/**************************************************************
[函数名]： updateTrackingInfo
[函数功能]：更新服务追踪记录信息
[函数参数]：pTracking: 需要更新的服务追踪记录结构体指针
nIndex: 需要更新的记录索引
[返回值]：int: TRUE表示成功，FALSE表示失败
**************************************************************/

int updateTrackingInfo(const Tracking *pTracking, int nIndex) {
    return updateTracking(pTracking, TRACKINGPATH, nIndex);
}
