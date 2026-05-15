//
// Created by lenovo on 2026/4/11.
//

#include "service.h"

#include <cstdlib>
#include <cstring>
#include <ctime>

#include "account_file.h"
#include "account_service.h"
#include "global.h"
#include "points_file.h"
#include "points_service.h"
#include "tracking_file.h"
#include "tracking_service.h"

/******************************************
 [函数名]queryAccountInfo
 [作用]查询账号信息
 [参数]pName:需要查询的账号名称
      pIndex:需要查询到的账号信息数量的指针
 [返回值]Account*:查询到的账号信息的指针
******************************************/

Account* queryAccountInfo(const char* pName,int* pIndex) {
    Account* pAccount=nullptr;
    pAccount=queryAccounts(pName,pIndex);

    return pAccount;
}

Account* queryAllAccountInfo(int* pCount) {
    if (pCount == nullptr) {
        return nullptr;
    }

    *pCount = 0;
    const int nCount = getAccountCount(ACCOUNTPATH);
    if (nCount <= 0) {
        return nullptr;
    }

    Account* pAccount = (Account*) malloc(sizeof(Account) * nCount);
    if (pAccount == nullptr) {
        return nullptr;
    }

    const int nReadCount = readAccount(pAccount, ACCOUNTPATH);
    if (nReadCount <= 0) {
        free(pAccount);
        return nullptr;
    }

    int nActiveCount = 0;
    for (int i = 0; i < nReadCount; i++) {
        if (pAccount[i].nDel == 0 && pAccount[i].nStatus != 2) {
            pAccount[nActiveCount] = pAccount[i];
            nActiveCount++;
        }
    }

    if (nActiveCount == 0) {
        free(pAccount);
        return nullptr;
    }

    *pCount = nActiveCount;
    return pAccount;
}

/******************************************
 [函数名]addAccountInfo
 [作用]添加账号信息
 [参数]account:添加的账号信息结构体
 [返回值]bool
******************************************/

bool addAccountInfo(Account account) {
    if (addAccount(account) == TRUE) {
        return TRUE;
    }else {
        return FALSE;
    }
}

/******************************************
 [函数名]startServiceInfo
 [作用]查询开始服务信息状态并开始服务
 [参数]pName:账号名称
      pPwd:账号密码
      pInfo:服务状态查询指针
 [返回值]int
******************************************/

int startServiceInfo(const char* pName, const char* pPwd, Start* pInfo) {
    int start=FALSE;
    start = startService(pName, pPwd, pInfo);
    return start;
}

/******************************************
 [函数名]endServiceInfo
 [作用]查询结束服务信息状态并结束服务
 [参数]pName:账号名称
      pPwd:账号密码
      pInfo:服务状态查询指针
 [返回值]int
******************************************/

int endServiceInfo(const char* pName, const char* pPwd, End* pInfo)
{
    int end = FALSE;
    end = endService(pName, pPwd, pInfo);

    return end;
}

int doAddPointInfo(const char* pName, const char* pPwd, PointChangeRecord* pPointInfo)
{
    return doAddPoint(pName, pPwd, pPointInfo);
}

int doRefundPointInfo(const char* pName, const char* pPwd, PointChangeRecord* pPointInfo)
{
    return doRefundPoint(pName, pPwd, pPointInfo);
}

int doDonatePointInfo(const char* pName, const char* pPwd, PointChangeRecord* pPointInfo)
{
    return doDonatePoint(pName, pPwd, pPointInfo);
}

int annulAccount(Account* pAccount)
{
    if (pAccount == nullptr) {
        return FALSE;
    }

    int nIndex = -1;
    Account* pCurAccount = checkAccount(pAccount->aName, pAccount->aPwd, &nIndex);
    if (pCurAccount == nullptr || pCurAccount->nDel != 0) {
        return FALSE;
    }

    if (pCurAccount->nStatus != 0) {
        return UNUSE;
    }

    pAccount->fBalance = pCurAccount->fBalance;

    pCurAccount->nStatus = 2;
    pCurAccount->nDel = 1;
    pCurAccount->fBalance = 0;
    pCurAccount->tLast = time(nullptr);

    if (FALSE == updateAccount(pCurAccount, ACCOUNTPATH, nIndex)) {
        return FALSE;
    }

    return TRUE;
}

int getStatisticsInfo(StatisticsInfo* pInfo)
{
    if (pInfo == nullptr) {
        return FALSE;
    }

    *pInfo = {};

    int nAccountCount = getAccountCount(ACCOUNTPATH);
    if (nAccountCount > 0) {
        Account* pAccounts = (Account*) malloc(sizeof(Account) * nAccountCount);
        if (pAccounts == nullptr) {
            return FALSE;
        }

        const int nReadCount = readAccount(pAccounts, ACCOUNTPATH);
        if (nReadCount > 0) {
            for (int i = 0; i < nReadCount; i++) {
                if (pAccounts[i].nStatus == 0 && pAccounts[i].nDel == 0) {
                    pInfo->nNormalAccountCount++;
                    pInfo->nAccountCount++;
                } else if (pAccounts[i].nStatus == 1 && pAccounts[i].nDel == 0) {
                    pInfo->nServingAccountCount++;
                    pInfo->nAccountCount++;
                } else if (pAccounts[i].nStatus == 2 || pAccounts[i].nDel != 0) {
                    pInfo->nAnnulledAccountCount++;
                }

                pInfo->fTotalBalance += pAccounts[i].fBalance;
                pInfo->fTotalPoints += pAccounts[i].fTotalPoints;
                pInfo->fTotalDuration += pAccounts[i].fTotalDuration;
                pInfo->nTotalUseCount += pAccounts[i].nUseCount;
            }
        }

        free(pAccounts);
    }

    int nTrackingCount = getTrackingCount(TRACKINGPATH);
    if (nTrackingCount > 0) {
        Tracking* pTracking = (Tracking*) malloc(sizeof(Tracking) * nTrackingCount);
        if (pTracking == nullptr) {
            return FALSE;
        }

        const int nReadCount = readTracking(pTracking, TRACKINGPATH);
        if (nReadCount > 0) {
            for (int i = 0; i < nTrackingCount; i++) {
                if (pTracking[i].nDel != 0) {
                    continue;
                }

                pInfo->nTrackingCount++;
                if (pTracking[i].nStatus == 0) {
                    pInfo->nRunningTrackingCount++;
                } else if (pTracking[i].nStatus == 1) {
                    pInfo->nCompletedTrackingCount++;
                }
            }
        }

        free(pTracking);
    }

    int nPointCount = getPointCount(MONEYPATH);
    if (nPointCount > 0) {
        PointChange* pPoints = (PointChange*) malloc(sizeof(PointChange) * nPointCount);
        if (pPoints == nullptr) {
            return FALSE;
        }

        const int nReadCount = readPoint(pPoints, MONEYPATH);
        if (nReadCount > 0) {
            for (int i = 0; i < nReadCount; i++) {
                if (pPoints[i].nDel != 0) {
                    continue;
                }

                pInfo->nPointRecordCount++;
                if (pPoints[i].nStatus == 0) {
                    pInfo->fAddedPoints += pPoints[i].fChange;
                } else if (pPoints[i].nStatus == 1) {
                    pInfo->fUsedPoints += pPoints[i].fChange;
                } else if (pPoints[i].nStatus == 2) {
                    pInfo->fDonatedPoints += pPoints[i].fChange;
                }
            }
        }

        free(pPoints);
    }

    return TRUE;
}

Tracking* queryAllTrackingInfo(int* pCount)
{
    if (pCount == nullptr) {
        return nullptr;
    }

    *pCount = 0;
    const int nCount = getTrackingCount(TRACKINGPATH);
    if (nCount <= 0) {
        return nullptr;
    }

    Tracking* pTracking = (Tracking*) malloc(sizeof(Tracking) * nCount);
    if (pTracking == nullptr) {
        return nullptr;
    }

    const int nReadCount = readTracking(pTracking, TRACKINGPATH);
    if (nReadCount <= 0) {
        free(pTracking);
        return nullptr;
    }

    int nActiveCount = 0;
    for (int i = 0; i < nReadCount; i++) {
        if (pTracking[i].nDel == 0) {
            pTracking[nActiveCount] = pTracking[i];
            nActiveCount++;
        }
    }

    if (nActiveCount == 0) {
        free(pTracking);
        return nullptr;
    }

    *pCount = nActiveCount;
    return pTracking;
}

Tracking* queryTrackingInfoByAccount(const char* pName, int* pCount)
{
    if (pName == nullptr || pCount == nullptr) {
        return nullptr;
    }

    int nAllCount = 0;
    Tracking* pAllTracking = queryAllTrackingInfo(&nAllCount);
    if (pAllTracking == nullptr || nAllCount <= 0) {
        return nullptr;
    }

    int nMatchCount = 0;
    for (int i = 0; i < nAllCount; i++) {
        if (strcmp(pAllTracking[i].aName, pName) == 0) {
            nMatchCount++;
        }
    }

    if (nMatchCount == 0) {
        free(pAllTracking);
        *pCount = 0;
        return nullptr;
    }

    Tracking* pResult = (Tracking*) malloc(sizeof(Tracking) * nMatchCount);
    if (pResult == nullptr) {
        free(pAllTracking);
        *pCount = 0;
        return nullptr;
    }

    int nIndex = 0;
    for (int i = 0; i < nAllCount; i++) {
        if (strcmp(pAllTracking[i].aName, pName) == 0) {
            pResult[nIndex] = pAllTracking[i];
            nIndex++;
        }
    }

    free(pAllTracking);
    *pCount = nMatchCount;
    return pResult;
}

PointChange* queryAllPointChangeInfo(int* pCount)
{
    if (pCount == nullptr) {
        return nullptr;
    }

    *pCount = 0;
    const int nCount = getPointCount(MONEYPATH);
    if (nCount <= 0) {
        return nullptr;
    }

    PointChange* pPoints = (PointChange*) malloc(sizeof(PointChange) * nCount);
    if (pPoints == nullptr) {
        return nullptr;
    }

    const int nReadCount = readPoint(pPoints, MONEYPATH);
    if (nReadCount <= 0) {
        free(pPoints);
        return nullptr;
    }

    int nActiveCount = 0;
    for (int i = 0; i < nReadCount; i++) {
        if (pPoints[i].nDel == 0) {
            pPoints[nActiveCount] = pPoints[i];
            nActiveCount++;
        }
    }

    if (nActiveCount == 0) {
        free(pPoints);
        return nullptr;
    }

    *pCount = nActiveCount;
    return pPoints;
}

PointChange* queryPointChangeInfoByAccount(const char* pName, int* pCount)
{
    if (pName == nullptr || pCount == nullptr) {
        return nullptr;
    }

    int nAllCount = 0;
    PointChange* pAllPoints = queryAllPointChangeInfo(&nAllCount);
    if (pAllPoints == nullptr || nAllCount <= 0) {
        return nullptr;
    }

    int nMatchCount = 0;
    for (int i = 0; i < nAllCount; i++) {
        if (strcmp(pAllPoints[i].aAccountName, pName) == 0) {
            nMatchCount++;
        }
    }

    if (nMatchCount == 0) {
        free(pAllPoints);
        *pCount = 0;
        return nullptr;
    }

    PointChange* pResult = (PointChange*) malloc(sizeof(PointChange) * nMatchCount);
    if (pResult == nullptr) {
        free(pAllPoints);
        *pCount = 0;
        return nullptr;
    }

    int nIndex = 0;
    for (int i = 0; i < nAllCount; i++) {
        if (strcmp(pAllPoints[i].aAccountName, pName) == 0) {
            pResult[nIndex] = pAllPoints[i];
            nIndex++;
        }
    }

    free(pAllPoints);
    *pCount = nMatchCount;
    return pResult;
}
