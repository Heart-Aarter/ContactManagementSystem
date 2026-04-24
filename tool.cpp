//
// Created by lenovo on 2026/3/31.
//

#include "tool.h"

#include <conio.h>
#include <cstdio>
#include <ctime>
#include <cstring>
#include <iostream>
#include <limits>

#include "global.h"

using namespace std;

namespace {

void readHiddenPassword(char aPwd[], int nSize) {
    int i = 0;

    while (true) {
        const int ch = _getch();
        if (ch == '\r') {
            break;
        }

        if (ch == '\b') {
            if (i > 0) {
                i--;
                aPwd[i] = '\0';
                printf("\b \b");
            }
            continue;
        }

        if (i < nSize - 1) {
            aPwd[i++] = (char) ch;
            aPwd[i] = '\0';
            printf("*");
        }
    }

    printf("\n");
}

bool isAlphaNumericPassword(const char* pPwd) {
    if (pPwd == nullptr || pPwd[0] == '\0') {
        return false;
    }

    for (int i = 0; pPwd[i] != '\0'; i++) {
        const unsigned char ch = (unsigned char) pPwd[i];
        const bool isDigit = ch >= '0' && ch <= '9';
        const bool isLower = ch >= 'a' && ch <= 'z';
        const bool isUpper = ch >= 'A' && ch <= 'Z';
        if (!isDigit && !isLower && !isUpper) {
            return false;
        }
    }

    return true;
}

}

void timeToString(time_t t, char *pBuf) {
    tm *timeinfo = localtime(&t);
    strftime(pBuf, 20, "%Y-%m-%d %H:%M", timeinfo);
}

time_t stringToTime(const char *pTime) {
    tm tm1{};
    time_t time1;
    sscanf(pTime, "%d-%d-%d %d:%d", &tm1.tm_year, &tm1.tm_mon, &tm1.tm_mday, &tm1.tm_hour, &tm1.tm_min); // NOLINT(*-err34-c)
    tm1.tm_year -= 1900;
    tm1.tm_mon -= 1;
    tm1.tm_sec = 0;
    tm1.tm_isdst = -1;

    time1 = mktime(&tm1);

    return time1;
}

void getPwd(char aPwd[]) {
    while (true) {
        char aConfirmPwd[PASSWORD_LENGTH] = {0};
        memset(aPwd, 0, PASSWORD_LENGTH);

        printf("请输入密码(长度为 1~%d): ", PASSWORD_MAX_LENGTH);
        readHiddenPassword(aPwd, PASSWORD_LENGTH);

        printf("请再次输入密码: ");
        readHiddenPassword(aConfirmPwd, PASSWORD_LENGTH);

        if (strlen(aPwd) == 0) {
            printf("密码不能为空。\n");
            system("cls");
            continue;
        }

        if (!isAlphaNumericPassword(aPwd)) {
            printf("密码只能包含英文字母和数字。\n");
            system("cls");
            continue;
        }

        if (strcmp(aPwd, aConfirmPwd) != 0) {
            printf("两次输入的密码不一致，请重新输入。\n");
            system("cls");
            continue;
        }

        break;
    }
}

void getPwdOnce(char aPwd[]) {
    memset(aPwd, 0, PASSWORD_LENGTH);
    printf("请输入密码(长度为 1~%d): ", PASSWORD_MAX_LENGTH);
    readHiddenPassword(aPwd, PASSWORD_LENGTH);
}

int getSize(const char* pInput) {
    int nSize = 0;
    while (*(pInput + nSize) != '\0') {
        nSize++;
    }
    return nSize;
}

void clearScreen() {
    system("cls");
}

void resetInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void showPageHeader(const char* pTitle) {
    clearScreen();
    cout << '\n' << "----------" << pTitle << "----------" << endl << endl;
}

void finishPage() {
    cout << endl << "按任意键继续...";
    _getch();
    clearScreen();
}
