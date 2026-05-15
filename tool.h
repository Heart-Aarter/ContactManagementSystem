//
// Created by lenovo on 2026/3/31.
//

#ifndef CONTACTMANAGEMENTSYSTEM_TOOL_H
#define CONTACTMANAGEMENTSYSTEM_TOOL_H

#include <ctime>
#include <string>
using namespace std;

void timeToString(time_t t,char *pBuf);
time_t stringToTime(const char *pTime);
void getPwd(char aPwd[]);
void getPwdOnce(char aPwd[]);
int getSize(const char* pInput);
void clearScreen();
void resetInput();
void showPageHeader(const char* pTitle);
void finishPage();
bool readMenuSelection(const char* pPrompt, int nMin, int nMax, int* pSelection);
bool readPositiveFloat(const char* pPrompt, float* pValue, const char* pValueName);
bool confirmAction(const char* pPrompt);
const char* accountStatusText(int nStatus, int nDel);
const char* trackingStatusText(int nStatus);
const char* pointStatusText(int nStatus);
void printDivider();
void printEmptyHint(const char* pEmptyText, const char* pNextStep);

#endif //CONTACTMANAGEMENTSYSTEM_TOOL_H
