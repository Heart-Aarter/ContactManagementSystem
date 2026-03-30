//
// Created by lenovo on 2026/4/11.
//

#ifndef CONTACTMANAGEMENTSYSTEM_ACCOUNT_FILE_H
#define CONTACTMANAGEMENTSYSTEM_ACCOUNT_FILE_H

#include "model.h"

bool isExist(const char* pNum,const char* pPath);
bool saveAccount(const Account* pAccount,const char* pPath);
int getAccountCount(const char* pPath);
int readAccount(Account* pAccount,const char* pPath);
bool updateAccount(const Account* pAccount, const char* pPath, int nIndex);


#endif //CONTACTMANAGEMENTSYSTEM_ACCOUNT_FILE_H
