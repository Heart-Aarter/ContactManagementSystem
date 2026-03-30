//
// Created by lenovo on 2026/4/11.
//

#ifndef CONTACTMANAGEMENTSYSTEM_ACCOUNT_SERVICE_H
#define CONTACTMANAGEMENTSYSTEM_ACCOUNT_SERVICE_H

#include "model.h"

Account* queryAccounts(const char* pName,int* pIndex);
bool getAccount();
bool initAccountList();
void releaseAccountList();
bool addAccount(Account account);
Account* checkAccount(const char* pName, const char* pPwd, int* pIndex);

#endif //CONTACTMANAGEMENTSYSTEM_ACCOUNT_SERVICE_H
