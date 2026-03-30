//
// Created by lenovo on 2026/3/31.
//

#ifndef CONTACTMANAGEMENTSYSTEM_TOOL_H
#define CONTACTMANAGEMENTSYSTEM_TOOL_H

#include <string>
using namespace std;

void timeToString(time_t t,char *pBuf);
time_t stringToTime(const char *pTime);
void getPwd(char aPwd[]);
int getSize(const char* pInput);
void clearScreen();
void resetInput();
void showPageHeader(const char* pTitle);
void finishPage();

#endif //CONTACTMANAGEMENTSYSTEM_TOOL_H
