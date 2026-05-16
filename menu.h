//
// Created by lenovo on 2026/3/30.
//

#ifndef CONTACTMANAGEMENTSYSTEM_MENU_H
#define CONTACTMANAGEMENTSYSTEM_MENU_H

void outputMenu();
void adminMainMenu();
void userEntryMenu();
void userMainMenu(const char* pLoginName);
bool adminLogin();
bool userLogin(char* pName, int nNameSize);
void add();
void query();
void logon();
void settle();
void addPoint();
void usePoint();
void addMoney();
void refundMoney();
void annul();
void donate();
void aiAssistant();
void statistics();
void serviceRecords();
void pointRecords();
void helpPage();
void exitApp();
#endif //CONTACTMANAGEMENTSYSTEM_MENU_H
