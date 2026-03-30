//
// Created by lenovo on 2026/4/11.
//

#include "service.h"

#include <cstdlib>
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

    *pCount = nReadCount;
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
            pInfo->nAccountCount = nReadCount;
            for (int i = 0; i < nReadCount; i++) {
                if (pAccounts[i].nStatus == 0 && pAccounts[i].nDel == 0) {
                    pInfo->nNormalAccountCount++;
                } else if (pAccounts[i].nStatus == 1 && pAccounts[i].nDel == 0) {
                    pInfo->nServingAccountCount++;
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
