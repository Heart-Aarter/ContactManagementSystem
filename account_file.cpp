//
// Created by lenovo on 2026/4/11.
//

#include "account_file.h"

#include <filesystem>
#include <iostream>
#include <cstring>

#include "global.h"
#include "model.h"

namespace {

std::string resolveDataPath(const char* pPath) {
    const std::filesystem::path directPath(pPath);
    if (std::filesystem::exists(directPath) || std::filesystem::exists(directPath.parent_path())) {
        return directPath.string();
    }

    const std::filesystem::path parentPath = std::filesystem::path("..") / directPath;
    if (std::filesystem::exists(parentPath) || std::filesystem::exists(parentPath.parent_path())) {
        return parentPath.string();
    }

    const std::filesystem::path grandParentPath = std::filesystem::path("..") / ".." / directPath;
    if (std::filesystem::exists(grandParentPath) || std::filesystem::exists(grandParentPath.parent_path())) {
        return grandParentPath.string();
    }

    return directPath.string();
}

FILE* openResolvedFile(const char* pPath, const char* pMode) {
    const std::string resolvedPath = resolveDataPath(pPath);
    return fopen(resolvedPath.c_str(), pMode);
}

}

/******************************************
[函数名]isExist
[功能]根据账号查找账户
[参数]pNum:账户号
     pPath:文件路径
[返回值]bool
******************************************/

bool isExist(const char* pNum,const char* pPath) {
    FILE* pFile=nullptr;
    Account account;

    if (pNum == nullptr || pPath == nullptr) {
        return FALSE;
    }

    pFile = openResolvedFile(pPath, "rb");
    if (pFile == nullptr) {
        return FALSE;
    }

    while (fread(&account,sizeof(Account),1,pFile) == 1) {
        if (strcmp(account.aName,pNum)==0 && account.nDel == 0) {
            fclose(pFile);
            return TRUE;
        }
    }

    fclose(pFile);
    return FALSE;
}

/******************************************
[函数名]saveAccount
[功能]保存单个账户信息到文件
[参数]pAccount:要保存的账户指针
     pPath:文件路径
[返回值]bool
******************************************/

bool saveAccount(const Account* pAccount,const char* pPath) {
    FILE* pFile=nullptr;

    if (pAccount == nullptr || pPath == nullptr) {
        return FALSE;
    }

    pFile = openResolvedFile(pPath, "ab");
    if (pFile == nullptr) {
        return FALSE;
    }

    if (fwrite(pAccount,sizeof(Account),1,pFile) != 1) {
        fclose(pFile);
        return FALSE;
    }

    fclose(pFile);
    return TRUE;
}

/******************************************
[函数名]getAccountCount
[功能]获取文件中的账户数量
[参数]pPath:文件路径
[返回值]int
******************************************/

int getAccountCount(const char* pPath) {
    FILE* pFile=nullptr;
    long lSize = 0;

    if (pPath == nullptr) {
        return FALSE;
    }

    pFile = openResolvedFile(pPath, "rb");
    if (pFile == nullptr) {
        return FALSE;
    }

    if (fseek(pFile, 0, SEEK_END) != 0) {
        fclose(pFile);
        return FALSE;
    }

    lSize = ftell(pFile);
    if (lSize < 0) {
        fclose(pFile);
        return FALSE;
    }

    fclose(pFile);
    return (int) (lSize / (long) sizeof(Account));
}

/******************************************
[函数名]readAccount
[功能]从文件中读取指定位置的账户信息
[参数]pAccount:用于存储读取的账户
     pPath:文件路径
[返回值]int
******************************************/

int readAccount(Account* pAccount,const char* pPath) {
    FILE* pFile = nullptr;
    int i=0;

    if (pAccount == nullptr || pPath ==nullptr) {
        return FALSE;
    }

    pFile = openResolvedFile(pPath, "rb");
    if (pFile == nullptr) {
        return FALSE;
    }

    while (i<MAX_ACCOUNTS && fread(&pAccount[i],sizeof(Account),1,pFile)==1) {
        i++;
    }

    fclose(pFile);

    return i;
}

/*************************************************
[函数名] updateAccount
[功能]  更新志愿者信息文件中的一条志愿者信息
[参数]  pAccount:更新后的志愿者内容
    pPath:志愿者信息文件的路径
    nIndex:需要更新的志愿者信息在文件中的记录数
[返回值] bool
*************************************************/

bool updateAccount(const Account* pAccount, const char* pPath, int nIndex) {
    FILE* fp = nullptr;
    const long lPosition = (long) nIndex * (long) sizeof(Account);

    if (pAccount == nullptr || pPath == nullptr || nIndex < 0) {
        return FALSE;
    }

    if (nIndex >= getAccountCount(pPath)) {
        return FALSE;
    }

    fp = openResolvedFile(pPath, "rb+");
    if (fp == nullptr) {
        return FALSE;
    }

    if (fseek(fp, lPosition, SEEK_SET) != 0) {
        fclose(fp);
        return FALSE;
    }

    if (fwrite(pAccount, sizeof(Account), 1, fp) != 1) {
        fclose(fp);
        return FALSE;
    }

    fclose(fp);
    return TRUE;
}
