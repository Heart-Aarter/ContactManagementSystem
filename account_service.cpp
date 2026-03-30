//
// Created by lenovo on 2026/4/11.
//

#include "account_service.h"

#include <cstdlib>
#include <cstring>

#include "account_file.h"
#include "global.h"
#include "model.h"

using namespace std;

lpAccountNode accountList = nullptr;

/******************************************
 [函数名]queryAccounts
 [作用]在志愿者信息链表中，查询账号号相同的志愿者信息
 [参数]pName:要查询的志愿者账号名称
      pIndex:查询到的志愿者信息数量的指针
 [返回值]Account*:查询到的志愿者信息的指针
******************************************/

Account* queryAccounts(const char* pName,int* pIndex) {
    lpAccountNode node = nullptr;
    Account *pAccount = nullptr;

    if (getAccount() == FALSE) {
        return nullptr;
    }
    if (accountList == nullptr || accountList->next == nullptr) {
        return nullptr;
    }

    pAccount = (Account *) malloc(sizeof(Account));
    if (pAccount == nullptr) {
        return nullptr;
    }

    *pIndex = 0;
    node = accountList->next;
    while (node != nullptr) {
        if (strcmp(node->data.aName, pName) == 0) {
            pAccount[*pIndex] = node->data;
            (*pIndex)++;
            auto *temp = (Account *) realloc(pAccount, ((*pIndex) + 1) * sizeof(Account));
            if (temp == nullptr) {
                free(pAccount);
                return nullptr;
            }
            pAccount = temp;
        }
        node = node->next;
    }

    if (*pIndex == 0) {
        free(pAccount);
        return nullptr;
    }
    return pAccount;
}

/******************************************
 [函数名]getAccount
 [作用]从账号信息文件中获取账号信息，并保存到链表中
 [参数]void
 [返回值]bool
******************************************/

bool getAccount() {
    int nCount = 0;
    int readCount = 0;
    int i = 0;
    Account* pAccount = nullptr;
    lpAccountNode node = nullptr;
    lpAccountNode cur = nullptr;

    if (accountList != nullptr) {
        releaseAccountList();
    }

    if (initAccountList() == FALSE) {
        return FALSE;
    }

    nCount = getAccountCount(ACCOUNTPATH);

    if (nCount == 0) {
        return TRUE;
    }
    pAccount = (Account *) malloc(sizeof(Account) * nCount);

    if (pAccount != nullptr) {
        readCount = readAccount(pAccount, ACCOUNTPATH);
        if (readCount <= 0) {
            free(pAccount);
            return FALSE;
        }
    }

    for (i = 0, node = accountList; i < readCount; i++) {
        cur = (lpAccountNode) malloc(sizeof(AccountNode));

        if (cur == nullptr) {
            free(pAccount);
            return FALSE;
        }

        memset(cur, 0, sizeof(AccountNode));

        cur->data = pAccount[i];
        cur->next = nullptr;
        node->next = cur;
        node = cur;
    }
    free(pAccount);
    return TRUE;
}

/******************************************
[函数名]initAccountList
[功能]初始化账号信息链表
[参数]void
[返回值]bool
******************************************/

bool initAccountList() {
    lpAccountNode head = nullptr;
    if (accountList == nullptr) {
        head = (lpAccountNode) malloc(sizeof(AccountNode));
        if (head != nullptr) {
            head->next = nullptr;
            accountList = head;
            return TRUE;
        }
        return FALSE;
    }
    return TRUE;
}

/******************************************
[函数名]releaseAccountList
[功能]释放账号信息链表
[参数]void
[返回值]void
******************************************/

void releaseAccountList() {
    lpAccountNode cur = accountList;
    lpAccountNode next = nullptr;

    while (cur!=nullptr) {
        next=cur->next;
        free(cur);
        cur=next;
    }

    accountList=nullptr;
}


/******************************************
[函数名]addAccount
[功能]添加账户到文件
[参数]account:账号信息结构体
[返回值]bool
******************************************/

bool addAccount(Account account) {
    if (isExist(account.aName, ACCOUNTPATH) == TRUE) {
        return FALSE;
    }

    if (TRUE == saveAccount(&account, ACCOUNTPATH)) {
        return TRUE;
    }

    return FALSE;
}

/*************************************************
[函数名] checkAccount
[功能] 根据账号号和密码，在链表中查询账号信息，并获取查询到的账号信息在链表中的位置
[参数] pName:账号名称
    pPwd:密码
    pIndex:志愿者信息在链表中的索引的指针
[返回值] Account*
*************************************************/

Account* checkAccount(const char* pName, const char* pPwd, int* pIndex) {
    lpAccountNode accountNode=nullptr;
    int nIndex=0;

    if (FALSE == getAccount()) {
        return FALSE;
    }

    accountNode = accountList->next;

    while (accountNode != nullptr) {
        if ((strcmp(accountNode->data.aName, pName) == 0) && (strcmp(accountNode->data.aPwd, pPwd) == 0)) {
            *pIndex = nIndex;
            return &accountNode->data;
        }
        accountNode = accountNode->next;
        nIndex++;
    }

    return nullptr;
}
