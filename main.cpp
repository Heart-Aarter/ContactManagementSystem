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
        cout << "========== 留守儿童关爱系统 ==========" << endl << endl;
        cout << "1. 管理员登录" << endl;
        cout << "2. 用户入口" << endl;
        cout << "0. 退出系统" << endl << endl;

        if (readMenuSelection("请选择登录方式(0~2): ", 0, 2, &nSelection)) {
            switch (nSelection) {
                case 1:
                    if (adminLogin()) {
                        adminMainMenu();
                    }
                    break;
                case 2:
                    userEntryMenu();
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
