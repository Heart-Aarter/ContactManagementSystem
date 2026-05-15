#include <iostream>
#include <windows.h>

#include "menu.h"
#include "tool.h"

using namespace std;

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    int nSelection = -1;

    do {
        clearScreen();
        outputMenu();

        if (readMenuSelection("请选择操作(0~13): ", 0, 13, &nSelection)) {
            switch (nSelection) {
                case 1:
                    add();
                    break;
                case 2:
                    query();
                    break;
                case 3:
                    logon();
                    break;
                case 4:
                    settle();
                    break;
                case 5:
                    addMoney();
                    break;
                case 6:
                    refundMoney();
                    break;
                case 7:
                    annul();
                    break;
                case 8:
                    donate();
                    break;
                case 9:
                    aiAssistant();
                    break;
                case 10:
                    statistics();
                    break;
                case 11:
                    serviceRecords();
                    break;
                case 12:
                    pointRecords();
                    break;
                case 13:
                    helpPage();
                    break;
                case 0:
                    exitApp();
                    break;
                default:
                    break;
            }
        } else {
            finishPage();
        }
    } while (nSelection != 0);

    return 0;
}
