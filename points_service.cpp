#include "points_service.h"

#include <cstring>
#include <ctime>

#include "account_file.h"
#include "account_service.h"
#include "global.h"
#include "points_file.h"

int doAddPoint(const char* pName, const char* pPwd, PointChangeRecord* pPointInfo) {
    if (pName == nullptr || pPwd == nullptr || pPointInfo == nullptr || pPointInfo->fChange <= 0) {
        return FALSE;
    }

    int nIndex = 0;
    Account* pAccount = checkAccount(pName, pPwd, &nIndex);
    if (pAccount == nullptr || pAccount->nDel != 0) {
        return FALSE;
    }

    if (pAccount->nStatus != 0) {
        return UNUSE;
    }

    pAccount->fBalance += pPointInfo->fChange;
    pAccount->fTotalPoints += pPointInfo->fChange;
    pAccount->tLast = time(nullptr);

    if (FALSE == updateAccount(pAccount, ACCOUNTPATH, nIndex)) {
        return FALSE;
    }

    PointChange point{};
    strcpy(point.aAccountName, pName);
    point.tTime = time(nullptr);
    point.nStatus = 0;
    point.fChange = pPointInfo->fChange;
    point.nDel = 0;

    if (FALSE == savePoint(&point, MONEYPATH)) {
        return FALSE;
    }

    strcpy(pPointInfo->aAccountName, pName);
    pPointInfo->fBalance = pAccount->fBalance;

    return TRUE;
}

int doRefundPoint(const char* pName, const char* pPwd, PointChangeRecord* pPointInfo) {
    if (pName == nullptr || pPwd == nullptr || pPointInfo == nullptr || pPointInfo->fChange <= 0) {
        return FALSE;
    }

    int nIndex = 0;
    Account* pAccount = checkAccount(pName, pPwd, &nIndex);
    if (pAccount == nullptr || pAccount->nDel != 0) {
        return FALSE;
    }

    if (pAccount->nStatus != 0) {
        return UNUSE;
    }

    if (pAccount->fBalance < pPointInfo->fChange) {
        return ENOUGHMONEY;
    }

    pAccount->fBalance -= pPointInfo->fChange;
    pAccount->tLast = time(nullptr);

    if (FALSE == updateAccount(pAccount, ACCOUNTPATH, nIndex)) {
        return FALSE;
    }

    PointChange point{};
    strcpy(point.aAccountName, pName);
    point.tTime = time(nullptr);
    point.nStatus = 1;
    point.fChange = pPointInfo->fChange;
    point.nDel = 0;

    if (FALSE == savePoint(&point, MONEYPATH)) {
        return FALSE;
    }

    strcpy(pPointInfo->aAccountName, pName);
    pPointInfo->fBalance = pAccount->fBalance;

    return TRUE;
}

int doDonatePoint(const char* pName, const char* pPwd, PointChangeRecord* pPointInfo) {
    if (pName == nullptr || pPwd == nullptr || pPointInfo == nullptr || pPointInfo->fChange <= 0) {
        return FALSE;
    }

    int nIndex = 0;
    Account* pAccount = checkAccount(pName, pPwd, &nIndex);
    if (pAccount == nullptr || pAccount->nDel != 0) {
        return FALSE;
    }

    if (pAccount->nStatus != 0) {
        return UNUSE;
    }

    pAccount->fBalance += pPointInfo->fChange;
    pAccount->fTotalPoints += pPointInfo->fChange;
    pAccount->tLast = time(nullptr);

    if (FALSE == updateAccount(pAccount, ACCOUNTPATH, nIndex)) {
        return FALSE;
    }

    PointChange point{};
    strcpy(point.aAccountName, pName);
    point.tTime = time(nullptr);
    point.nStatus = 2;
    point.fChange = pPointInfo->fChange;
    point.nDel = 0;

    if (FALSE == savePoint(&point, MONEYPATH)) {
        return FALSE;
    }

    strcpy(pPointInfo->aAccountName, pName);
    pPointInfo->fBalance = pAccount->fBalance;

    return TRUE;
}
