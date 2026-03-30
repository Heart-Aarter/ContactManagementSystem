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
        char aConfirmPwd[8] = {0};
        memset(aPwd, 0, 8);

        printf("请输入密码(长度为 1~7): ");
        readHiddenPassword(aPwd, 8);

        printf("请再次输入密码: ");
        readHiddenPassword(aConfirmPwd, 8);

        if (strlen(aPwd) == 0) {
            printf("密码不能为空。\n");
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
