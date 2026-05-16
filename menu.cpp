#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>

#include "account_service.h"
#include "global.h"
#include "large_model.h"
#include "menu.h"
#include "service.h"
#include "tool.h"

using namespace std;

namespace {

bool validatePasswordLength(const char* pPwd) {
    if (getSize(pPwd) > PASSWORD_MAX_LENGTH) {
        cout << "密码长度不能超过 " << PASSWORD_MAX_LENGTH << " 个字符。" << endl;
        finishPage();
        return false;
    }
    return true;
}

bool validatePasswordContent(const char* pPwd) {
    if (pPwd == nullptr || pPwd[0] == '\0') {
        cout << "密码不能为空。" << endl;
        finishPage();
        return false;
    }

    for (int i = 0; pPwd[i] != '\0'; i++) {
        const unsigned char ch = (unsigned char) pPwd[i];
        const bool isDigit = ch >= '0' && ch <= '9';
        const bool isLower = ch >= 'a' && ch <= 'z';
        const bool isUpper = ch >= 'A' && ch <= 'Z';
        if (!isDigit && !isLower && !isUpper) {
            cout << "密码只能包含英文字母和数字。" << endl;
            finishPage();
            return false;
        }
    }

    return true;
}

bool validatePassword(const char* pPwd) {
    return validatePasswordLength(pPwd) && validatePasswordContent(pPwd);
}

bool inputBoundedText(const char* pPrompt, char* pOutput, int nOutputSize, int nMaxLength) {
    string input;
    cout << pPrompt;
    cin >> input;

    if (!cin || input.empty()) {
        resetInput();
        cout << "输入不能为空。" << endl;
        finishPage();
        return false;
    }

    if ((int) input.size() > nMaxLength) {
        cout << "输入长度不能超过 " << nMaxLength << " 个字符。" << endl;
        finishPage();
        return false;
    }

    memset(pOutput, 0, nOutputSize);
    strncpy(pOutput, input.c_str(), nOutputSize - 1);
    return true;
}

bool inputAccountAndPassword(char* pName, int nNameSize, char* pPwd, int nPwdSize) {
    const string namePrompt = "请输入志愿者账号(长度为 1~" + to_string(ACCOUNT_NAME_MAX_LENGTH) + "): ";
    if (!inputBoundedText(namePrompt.c_str(), pName, nNameSize, ACCOUNT_NAME_MAX_LENGTH)) {
        return false;
    }

    getPwdOnce(pPwd);
    return validatePassword(pPwd);
}

void printMoney(float fValue) {
    cout << fixed << setprecision(2) << fValue << defaultfloat;
}

void printOverview() {
    StatisticsInfo info{};
    if (getStatisticsInfo(&info) != TRUE) {
        cout << "系统概览: 暂无统计数据" << endl << endl;
        return;
    }

    cout << "系统概览: 志愿者 " << info.nAccountCount
         << " 人 | 服务中 " << info.nServingAccountCount
         << " 人 | 当前积分总余额 ";
    printMoney(info.fTotalBalance);
    cout << endl << endl;
}

void printAccountTable(const Account* pAccount, int nCount) {
    char aLastTime[TIMELENGTH] = {0};
    const int nNameColumnWidth = ACCOUNT_NAME_MAX_LENGTH + 4;

    printDivider();
    cout << left
         << setw(nNameColumnWidth) << "账号"
         << setw(10) << "状态"
         << setw(12) << "当前积分"
         << setw(12) << "累计积分"
         << setw(12) << "累计时长"
         << "最后操作时间" << endl;
    printDivider();

    for (int i = 0; i < nCount; i++) {
        timeToString(pAccount[i].tLast, aLastTime);
        cout << left
             << setw(nNameColumnWidth) << pAccount[i].aName
             << setw(10) << accountStatusText(pAccount[i].nStatus, pAccount[i].nDel)
             << setw(12) << fixed << setprecision(2) << pAccount[i].fBalance
             << setw(12) << pAccount[i].fTotalPoints
             << setw(12) << pAccount[i].fTotalDuration
             << aLastTime << defaultfloat << endl;
    }
}

void printTrackingTable(const Tracking* pTracking, int nCount) {
    char aStartTime[TIMELENGTH] = {0};
    char aEndTime[TIMELENGTH] = {0};
    const int nNameColumnWidth = ACCOUNT_NAME_MAX_LENGTH + 4;

    printDivider();
    cout << left
         << setw(nNameColumnWidth) << "账号"
         << setw(10) << "状态"
         << setw(18) << "获得积分"
         << setw(18) << "开始时间"
         << "结束时间" << endl;
    printDivider();

    for (int i = 0; i < nCount; i++) {
        timeToString(pTracking[i].tStart, aStartTime);
        if (pTracking[i].nStatus == 1) {
            timeToString(pTracking[i].tEnd, aEndTime);
        } else {
            strcpy(aEndTime, "进行中");
        }

        cout << left
             << setw(nNameColumnWidth) << pTracking[i].aName
             << setw(10) << trackingStatusText(pTracking[i].nStatus)
             << setw(18) << fixed << setprecision(2) << pTracking[i].fAmount
             << setw(18) << aStartTime
             << aEndTime << defaultfloat << endl;
    }
}

void printPointTable(const PointChange* pPoints, int nCount) {
    char aTime[TIMELENGTH] = {0};
    const int nNameColumnWidth = ACCOUNT_NAME_MAX_LENGTH + 4;

    printDivider();
    cout << left
         << setw(nNameColumnWidth) << "账号"
         << setw(12) << "类型"
         << setw(14) << "积分变动"
         << "时间" << endl;
    printDivider();

    for (int i = 0; i < nCount; i++) {
        timeToString(pPoints[i].tTime, aTime);
        cout << left
             << setw(nNameColumnWidth) << pPoints[i].aAccountName
             << setw(12) << pointStatusText(pPoints[i].nStatus)
             << setw(14) << fixed << setprecision(2) << pPoints[i].fChange
             << aTime << defaultfloat << endl;
    }
}

bool readQueryMode(int* pMode) {
    cout << "1. 按账号查询" << endl;
    cout << "2. 查看全部记录" << endl << endl;
    return readMenuSelection("请选择查询方式(1~2): ", 1, 2, pMode);
}

void printOperationFailed(const char* pAction) {
    cout << pAction << "失败。可能原因: 账号不存在、密码错误或数据文件无法读取。" << endl;
}

}

namespace {

bool inputOperationAccount(const char* pFixedName, char* pName, int nNameSize, char* pPwd, int nPwdSize) {
    int nIndex = -1;

    if (pName == nullptr || pPwd == nullptr || nNameSize <= 0 || nPwdSize <= 0) {
        return false;
    }

    if (pFixedName == nullptr) {
        return inputAccountAndPassword(pName, nNameSize, pPwd, nPwdSize);
    }

    if (pFixedName[0] == '\0') {
        cout << "当前登录信息无效，请重新登录。" << endl;
        finishPage();
        return false;
    }

    memset(pName, 0, nNameSize);
    strncpy(pName, pFixedName, nNameSize - 1);

    cout << "当前账号: " << pName << endl;
    getPwdOnce(pPwd);
    if (!validatePassword(pPwd)) {
        return false;
    }

    if (checkAccount(pName, pPwd, &nIndex) == nullptr) {
        cout << endl << "密码错误或账号状态不可用，请重新确认。" << endl;
        finishPage();
        return false;
    }

    return true;
}

void doLogon(const char* pFixedName) {
    showPageHeader("开始服务");

    char aName[ACCOUNT_NAME_LENGTH] = {0};
    char aPwd[PASSWORD_LENGTH] = {0};
    Start info{};
    char aTime[TIMELENGTH] = {0};

    if (!inputOperationAccount(pFixedName, aName, sizeof(aName), aPwd, sizeof(aPwd))) {
        return;
    }

    const int nResult = startServiceInfo(aName, aPwd, &info);

    cout << endl;
    switch (nResult) {
        case FALSE:
            printOperationFailed("开始服务");
            break;
        case TRUE:
            timeToString(info.tLogon, aTime);
            cout << "开始服务成功。" << endl << endl;
            cout << "账号: " << info.aName << endl;
            cout << "当前积分: ";
            printMoney((float) info.fBalance);
            cout << endl;
            cout << "开始时间: " << aTime << endl;
            cout << "下一步: 服务结束后返回菜单选择结束服务。" << endl;
            break;
        case UNUSE:
            cout << "开始服务失败。该账号正在服务或已注销。" << endl;
            break;
        case ENOUGHMONEY:
            cout << "开始服务失败。账号积分不足。" << endl;
            break;
        default:
            cout << "开始服务发生未知错误。" << endl;
            break;
    }

    finishPage();
}

void doSettle(const char* pFixedName) {
    showPageHeader("结束服务");

    char aName[ACCOUNT_NAME_LENGTH] = {0};
    char aPwd[PASSWORD_LENGTH] = {0};
    End info{};
    char aStartTime[TIMELENGTH] = {0};
    char aEndTime[TIMELENGTH] = {0};

    if (!inputOperationAccount(pFixedName, aName, sizeof(aName), aPwd, sizeof(aPwd))) {
        return;
    }

    const int nResult = endServiceInfo(aName, aPwd, &info);

    cout << endl;
    switch (nResult) {
        case FALSE:
            printOperationFailed("结束服务");
            break;
        case TRUE:
            timeToString(info.tStart, aStartTime);
            timeToString(info.tEnd, aEndTime);
            cout << "结束服务成功。" << endl << endl;
            cout << "账号: " << info.aName << endl;
            cout << "本次获得积分: ";
            printMoney(info.fAmount);
            cout << endl;
            cout << "当前积分: ";
            printMoney(info.fBalance);
            cout << endl;
            cout << "开始时间: " << aStartTime << endl;
            cout << "结束时间: " << aEndTime << endl;
            break;
        case UNUSE:
            cout << "结束服务失败。该账号当前没有正在进行的服务。" << endl;
            break;
        case ENOUGHMONEY:
            cout << "结束服务失败。账号积分不足。" << endl;
            break;
        default:
            cout << "结束服务发生未知错误。" << endl;
            break;
    }

    finishPage();
}

void doAddMoney(const char* pFixedName) {
    showPageHeader("获取积分");

    char aName[ACCOUNT_NAME_LENGTH] = {0};
    char aPwd[PASSWORD_LENGTH] = {0};
    float fAmount = 0;
    PointChangeRecord pointInfo{};

    if (!inputOperationAccount(pFixedName, aName, sizeof(aName), aPwd, sizeof(aPwd))) {
        return;
    }

    if (!readPositiveFloat("请输入获取积分数量: ", &fAmount, "积分数量")) {
        finishPage();
        return;
    }

    pointInfo.fChange = fAmount;
    const int nResult = doAddPointInfo(aName, aPwd, &pointInfo);

    cout << endl;
    switch (nResult) {
        case FALSE:
            printOperationFailed("获取积分");
            break;
        case TRUE:
            cout << "获取积分成功。" << endl;
            cout << "账号: " << pointInfo.aAccountName << endl;
            cout << "本次获取积分: ";
            printMoney(pointInfo.fChange);
            cout << endl;
            cout << "当前积分: ";
            printMoney(pointInfo.fBalance);
            cout << endl;
            break;
        case UNUSE:
            cout << "获取积分失败。该账号正在服务或已注销。" << endl;
            break;
        default:
            cout << "获取积分发生未知错误。" << endl;
            break;
    }

    finishPage();
}

void doRefundMoney(const char* pFixedName) {
    showPageHeader("使用积分");

    char aName[ACCOUNT_NAME_LENGTH] = {0};
    char aPwd[PASSWORD_LENGTH] = {0};
    float fAmount = 0;
    PointChangeRecord pointInfo{};

    if (!inputOperationAccount(pFixedName, aName, sizeof(aName), aPwd, sizeof(aPwd))) {
        return;
    }

    if (!readPositiveFloat("请输入使用积分数量: ", &fAmount, "积分数量")) {
        finishPage();
        return;
    }

    pointInfo.fChange = fAmount;
    const int nResult = doRefundPointInfo(aName, aPwd, &pointInfo);

    cout << endl;
    switch (nResult) {
        case FALSE:
            printOperationFailed("使用积分");
            break;
        case TRUE:
            cout << "使用积分成功。" << endl;
            cout << "账号: " << pointInfo.aAccountName << endl;
            cout << "本次使用积分: ";
            printMoney(pointInfo.fChange);
            cout << endl;
            cout << "剩余积分: ";
            printMoney(pointInfo.fBalance);
            cout << endl;
            cout << "积分折合为 ";
            printMoney(pointInfo.fChange / 10.0f);
            cout << " 元。" << endl;
            break;
        case UNUSE:
            cout << "使用积分失败。该账号正在服务或已注销。" << endl;
            break;
        case ENOUGHMONEY:
            cout << "使用积分失败。账号积分不足。" << endl;
            break;
        default:
            cout << "使用积分发生未知错误。" << endl;
            break;
    }

    finishPage();
}

void doDonate(const char* pFixedName) {
    showPageHeader("物资捐赠");

    char aName[ACCOUNT_NAME_LENGTH] = {0};
    char aPwd[PASSWORD_LENGTH] = {0};
    float fAmount = 0;
    float fPoint = 0;
    PointChangeRecord pointInfo{};

    if (!inputOperationAccount(pFixedName, aName, sizeof(aName), aPwd, sizeof(aPwd))) {
        return;
    }

    if (!readPositiveFloat("请输入捐赠金额: ", &fAmount, "捐赠金额")) {
        finishPage();
        return;
    }

    fPoint = fAmount * (float) POINT_RATE;
    pointInfo.fChange = fPoint;
    const int nResult = doDonatePointInfo(aName, aPwd, &pointInfo);

    cout << endl;
    switch (nResult) {
        case FALSE:
            printOperationFailed("物资捐赠");
            break;
        case TRUE:
            cout << "物资捐赠成功。" << endl;
            cout << "账号: " << pointInfo.aAccountName << endl;
            cout << "捐赠金额: ";
            printMoney(fAmount);
            cout << endl;
            cout << "获得积分: ";
            printMoney(pointInfo.fChange);
            cout << endl;
            cout << "当前积分: ";
            printMoney(pointInfo.fBalance);
            cout << endl;
            break;
        case UNUSE:
            cout << "物资捐赠失败。该账号正在服务或已注销。" << endl;
            break;
        default:
            cout << "物资捐赠发生未知错误。" << endl;
            break;
    }

    finishPage();
}

bool doAnnul(const char* pFixedName) {
    showPageHeader(pFixedName == nullptr ? "注销账号" : "注销本人账号");

    Account account{};

    if (!inputOperationAccount(pFixedName, account.aName, sizeof(account.aName), account.aPwd, sizeof(account.aPwd))) {
        return false;
    }

    if (!confirmAction("确认注销该账号")) {
        cout << endl << "已取消注销。" << endl;
        finishPage();
        return false;
    }

    const int nResult = annulAccount(&account);

    cout << endl;
    switch (nResult) {
        case FALSE:
            printOperationFailed("注销账号");
            break;
        case TRUE:
            cout << "注销账号成功。" << endl;
            cout << "账号: " << account.aName << endl;
            cout << "清退积分: ";
            printMoney(account.fBalance);
            cout << endl;
            finishPage();
            return true;
        case UNUSE:
            cout << "注销账号失败。该账号正在服务或已注销。" << endl;
            break;
        default:
            cout << "注销账号发生未知错误。" << endl;
            break;
    }

    finishPage();
    return false;
}

void showServiceRecordResult(Tracking* pTracking, int nCount) {
    cout << endl;
    if (pTracking == nullptr || nCount == 0) {
        printEmptyHint("当前没有符合条件的服务记录。", "可先完成一次开始服务和结束服务流程。");
        finishPage();
        return;
    }

    printTrackingTable(pTracking, nCount);
    free(pTracking);
    finishPage();
}

void showPointRecordResult(PointChange* pPoints, int nCount) {
    cout << endl;
    if (pPoints == nullptr || nCount == 0) {
        printEmptyHint("当前没有符合条件的积分流水。", "可先获取积分、使用积分或完成物资捐赠。");
        finishPage();
        return;
    }

    printPointTable(pPoints, nCount);
    free(pPoints);
    finishPage();
}

void showServiceRecordsByAccount(const char* pName) {
    int nCount = 0;
    showServiceRecordResult(queryTrackingInfoByAccount(pName, &nCount), nCount);
}

void showPointRecordsByAccount(const char* pName) {
    int nCount = 0;
    showPointRecordResult(queryPointChangeInfoByAccount(pName, &nCount), nCount);
}

}

bool adminLogin() {
    const char ADMIN_NAME[] = "admin";
    const char ADMIN_PASSWORD[] = "123456";

    showPageHeader("管理员登录");

    char aName[ACCOUNT_NAME_LENGTH] = {0};
    char aPwd[PASSWORD_LENGTH] = {0};

    if (!inputBoundedText("请输入管理员账号: ", aName, sizeof(aName), ACCOUNT_NAME_MAX_LENGTH)) {
        return false;
    }

    getPwdOnce(aPwd);
    if (!validatePassword(aPwd)) {
        return false;
    }

    if (strcmp(aName, ADMIN_NAME) != 0 || strcmp(aPwd, ADMIN_PASSWORD) != 0) {
        cout << endl << "管理员账号或密码错误。" << endl;
        finishPage();
        return false;
    }

    cout << endl << "管理员登录成功。" << endl;
    finishPage();
    return true;
}

bool userLogin(char* pName, int nNameSize) {
    showPageHeader("用户登录");

    char aName[ACCOUNT_NAME_LENGTH] = {0};
    char aPwd[PASSWORD_LENGTH] = {0};
    int nIndex = -1;

    if (!inputAccountAndPassword(aName, sizeof(aName), aPwd, sizeof(aPwd))) {
        return false;
    }

    Account* pAccount = checkAccount(aName, aPwd, &nIndex);
    if (pAccount == nullptr || pAccount->nDel != 0 || pAccount->nStatus == 2) {
        cout << endl << "用户账号或密码错误，或账号已注销。" << endl;
        finishPage();
        return false;
    }

    memset(pName, 0, nNameSize);
    strncpy(pName, aName, nNameSize - 1);

    cout << endl << "用户登录成功。" << endl;
    finishPage();
    return true;
}

void adminMainMenu() {
    int nSelection = -1;

    do {
        clearScreen();
        cout << "========== 管理员菜单 ==========" << endl << endl;
        printOverview();
        cout << "1. 注册志愿者" << endl;
        cout << "2. 查询志愿者" << endl;
        cout << "3. 统计查询" << endl;
        cout << "4. 服务记录查询" << endl;
        cout << "5. 积分流水查询" << endl;
        cout << "6. 注销账号" << endl;
        cout << "7. 使用帮助/规则说明" << endl;
        cout << "0. 退出登录" << endl << endl;

        if (readMenuSelection("请选择管理员功能(0~7): ", 0, 7, &nSelection)) {
            switch (nSelection) {
                case 1:
                    add();
                    break;
                case 2:
                    query();
                    break;
                case 3:
                    statistics();
                    break;
                case 4:
                    serviceRecords();
                    break;
                case 5:
                    pointRecords();
                    break;
                case 6:
                    annul();
                    break;
                case 7:
                    helpPage();
                    break;
                case 0:
                    break;
                default:
                    break;
            }
        } else {
            finishPage();
        }
    } while (nSelection != 0);
}

void userEntryMenu() {
    int nSelection = -1;
    char aLoginName[ACCOUNT_NAME_LENGTH] = {0};

    do {
        clearScreen();
        cout << "========== 用户入口 ==========" << endl << endl;
        cout << "1. 注册志愿者" << endl;
        cout << "2. 登录账号" << endl;
        cout << "0. 返回上一级" << endl << endl;

        if (readMenuSelection("请选择用户功能(0~2): ", 0, 2, &nSelection)) {
            switch (nSelection) {
                case 1:
                    add();
                    break;
                case 2:
                    memset(aLoginName, 0, sizeof(aLoginName));
                    if (userLogin(aLoginName, sizeof(aLoginName))) {
                        userMainMenu(aLoginName);
                    }
                    break;
                case 0:
                    break;
                default:
                    break;
            }
        } else {
            finishPage();
        }
    } while (nSelection != 0);
}

void userMainMenu(const char* pLoginName) {
    int nSelection = -1;

    do {
        clearScreen();
        cout << "========== 用户菜单 ==========" << endl << endl;
        cout << "当前账号: " << pLoginName << endl << endl;
        cout << "1. 开始服务" << endl;
        cout << "2. 结束服务" << endl;
        cout << "3. 获取积分" << endl;
        cout << "4. 使用积分" << endl;
        cout << "5. 物资捐赠" << endl;
        cout << "6. 注销本人账号" << endl;
        cout << "7. 查看本人服务记录" << endl;
        cout << "8. 查看本人积分流水" << endl;
        cout << "9. AI 助手" << endl;
        cout << "10. 使用帮助/规则说明" << endl;
        cout << "0. 退出登录" << endl << endl;

        if (readMenuSelection("请选择用户功能(0~10): ", 0, 10, &nSelection)) {
            switch (nSelection) {
                case 1:
                    doLogon(pLoginName);
                    break;
                case 2:
                    doSettle(pLoginName);
                    break;
                case 3:
                    doAddMoney(pLoginName);
                    break;
                case 4:
                    doRefundMoney(pLoginName);
                    break;
                case 5:
                    doDonate(pLoginName);
                    break;
                case 6:
                    if (doAnnul(pLoginName)) {
                        nSelection = 0;
                    }
                    break;
                case 7:
                    showServiceRecordsByAccount(pLoginName);
                    break;
                case 8:
                    showPointRecordsByAccount(pLoginName);
                    break;
                case 9:
                    aiAssistant();
                    break;
                case 10:
                    helpPage();
                    break;
                case 0:
                    break;
                default:
                    break;
            }
        } else {
            finishPage();
        }
    } while (nSelection != 0);
}

void add() {
    showPageHeader("注册志愿者");

    char aName[ACCOUNT_NAME_LENGTH] = {0};
    char aPwd[PASSWORD_LENGTH] = {0};
    Account account{};
    Account* pAccount = nullptr;
    int nIndex = 0;

    const string namePrompt = "请输入志愿者账号(长度为 1~" + to_string(ACCOUNT_NAME_MAX_LENGTH) + "): ";
    if (!inputBoundedText(namePrompt.c_str(), aName, sizeof(aName), ACCOUNT_NAME_MAX_LENGTH)) {
        return;
    }

    getPwd(aPwd);
    if (!validatePassword(aPwd)) {
        return;
    }

    pAccount = queryAccountInfo(aName, &nIndex);
    if (pAccount != nullptr) {
        cout << endl << "账号已存在，注册失败。请更换账号或返回查询功能确认账号状态。" << endl;
        free(pAccount);
        finishPage();
        return;
    }

    strcpy(account.aName, aName);
    strcpy(account.aPwd, aPwd);
    account.fBalance = 0;
    account.fTotalPoints = 0;
    account.fTotalDuration = 0;
    account.nDel = 0;
    account.nStatus = 0;
    account.nUseCount = 0;
    account.tStart = time(nullptr);
    account.tLast = account.tStart;

    tm endTime = *localtime(&account.tStart);
    endTime.tm_year += 1;
    account.tEnd = mktime(&endTime);

    cout << endl;
    if (addAccountInfo(account) == FALSE) {
        cout << "注册失败。可能原因: 账号已存在或数据文件无法写入。" << endl;
    } else {
        char aRegTime[TIMELENGTH] = {0};
        timeToString(account.tStart, aRegTime);
        cout << "注册成功。" << endl;
        cout << "账号: " << account.aName << endl;
        cout << "状态: " << accountStatusText(account.nStatus, account.nDel) << endl;
        cout << "注册时间: " << aRegTime << endl;
        cout << "下一步: 可返回主菜单选择 3 开始服务。" << endl;
    }

    finishPage();
}

void query() {
    showPageHeader("查询志愿者");

    char aName[ACCOUNT_NAME_LENGTH] = {0};
    Account* pAccount = nullptr;
    int nCount = 0;
    int nMode = 0;

    cout << "1. 按账号查询" << endl;
    cout << "2. 查看所有志愿者" << endl << endl;
    if (!readMenuSelection("请选择查询方式(1~2): ", 1, 2, &nMode)) {
        finishPage();
        return;
    }

    if (nMode == 2) {
        pAccount = queryAllAccountInfo(&nCount);
        cout << endl;
        if (pAccount == nullptr || nCount == 0) {
            printEmptyHint("当前没有可显示的志愿者记录。", "可先返回主菜单选择 1 注册志愿者。");
            finishPage();
            return;
        }

        printAccountTable(pAccount, nCount);
        free(pAccount);
        finishPage();
        return;
    }

    if (!inputBoundedText("请输入要查询的志愿者账号: ", aName, sizeof(aName), ACCOUNT_NAME_MAX_LENGTH)) {
        return;
    }

    pAccount = queryAccountInfo(aName, &nCount);
    cout << endl;
    if (pAccount == nullptr) {
        printEmptyHint("未找到该账号。", "请确认账号拼写，或返回主菜单选择 1 注册。");
        finishPage();
        return;
    }

    printAccountTable(pAccount, nCount);
    free(pAccount);
    finishPage();
}

void logon() {
    doLogon(nullptr);
}

void settle() {
    doSettle(nullptr);
}

void addMoney() {
    doAddMoney(nullptr);
}

void refundMoney() {
    doRefundMoney(nullptr);
}

void annul() {
    doAnnul(nullptr);
}

void donate() {
    doDonate(nullptr);
}

void aiAssistant() {
    showPageHeader("AI 助手(DeepSeek V4 flash)");
    cout << "提示: 如未配置 DEEPSEEK_API_KEY，提问时会显示连接失败。" << endl;
    cout << "可先在帮助页查看配置说明。" << endl << endl;
    resetInput();
    runDeepSeekAssistant();
    finishPage();
}

void statistics() {
    showPageHeader("统计查询");

    StatisticsInfo info{};
    if (getStatisticsInfo(&info) == FALSE) {
        cout << "统计数据读取失败。" << endl;
        finishPage();
        return;
    }

    cout << "账号统计" << endl;
    printDivider();
    cout << "志愿者总数: " << info.nAccountCount << endl;
    cout << "正常账号: " << info.nNormalAccountCount << endl;
    cout << "服务中账号: " << info.nServingAccountCount << endl;
    cout << "已注销账号: " << info.nAnnulledAccountCount << endl;
    cout << endl;

    cout << "积分与服务汇总" << endl;
    printDivider();
    cout << "当前积分总余额: ";
    printMoney(info.fTotalBalance);
    cout << endl;
    cout << "累计获得积分: ";
    printMoney(info.fTotalPoints);
    cout << endl;
    cout << "累计服务时长: ";
    printMoney(info.fTotalDuration);
    cout << " 小时" << endl;
    cout << "累计开始服务次数: " << info.nTotalUseCount << endl;
    cout << endl;

    cout << "服务记录统计" << endl;
    printDivider();
    cout << "服务记录总数: " << info.nTrackingCount << endl;
    cout << "进行中服务记录: " << info.nRunningTrackingCount << endl;
    cout << "已完成服务记录: " << info.nCompletedTrackingCount << endl;
    cout << endl;

    cout << "积分流水统计" << endl;
    printDivider();
    cout << "积分流水总数: " << info.nPointRecordCount << endl;
    cout << "手动获取积分合计: ";
    printMoney(info.fAddedPoints);
    cout << endl;
    cout << "使用积分合计: ";
    printMoney(info.fUsedPoints);
    cout << endl;
    cout << "物资捐赠获得积分合计: ";
    printMoney(info.fDonatedPoints);
    cout << endl;

    finishPage();
}

void serviceRecords() {
    showPageHeader("服务记录查询");

    char aName[ACCOUNT_NAME_LENGTH] = {0};
    Tracking* pTracking = nullptr;
    int nCount = 0;
    int nMode = 0;

    if (!readQueryMode(&nMode)) {
        finishPage();
        return;
    }

    if (nMode == 1) {
        if (!inputBoundedText("请输入志愿者账号: ", aName, sizeof(aName), ACCOUNT_NAME_MAX_LENGTH)) {
            return;
        }
        pTracking = queryTrackingInfoByAccount(aName, &nCount);
    } else {
        pTracking = queryAllTrackingInfo(&nCount);
    }

    showServiceRecordResult(pTracking, nCount);
}

void pointRecords() {
    showPageHeader("积分流水查询");

    char aName[ACCOUNT_NAME_LENGTH] = {0};
    PointChange* pPoints = nullptr;
    int nCount = 0;
    int nMode = 0;

    if (!readQueryMode(&nMode)) {
        finishPage();
        return;
    }

    if (nMode == 1) {
        if (!inputBoundedText("请输入志愿者账号: ", aName, sizeof(aName), ACCOUNT_NAME_MAX_LENGTH)) {
            return;
        }
        pPoints = queryPointChangeInfoByAccount(aName, &nCount);
    } else {
        pPoints = queryAllPointChangeInfo(&nCount);
    }

    showPointRecordResult(pPoints, nCount);
}

void helpPage() {
    showPageHeader("使用帮助/规则说明");

    cout << "基本流程" << endl;
    printDivider();
    cout << "1. 新志愿者先选择 1 注册账号。" << endl;
    cout << "2. 开始服务前选择 3，输入账号和密码。" << endl;
    cout << "3. 服务结束后选择 4，系统按服务时长结算积分。" << endl;
    cout << "4. 可通过 11 查看服务记录，通过 12 查看积分流水。" << endl << endl;

    cout << "积分规则" << endl;
    printDivider();
    cout << "每 " << UNIT << " 分钟为一个积分单位，基础积分为 " << POINT_RATE << "。" << endl;
    cout << "注册 " << NEWCOMER_DAYS << " 天内享受新人加成，周末服务享受周末加成。" << endl;
    cout << "物资捐赠会按物资价值折算积分；使用积分时按 10 积分约 1 元展示捐赠金额。" << endl << endl;

    cout << "账号状态" << endl;
    printDivider();
    cout << "正常: 可开始服务、获取积分、使用积分、捐赠和注销。" << endl;
    cout << "服务中: 已开始服务，需先结束服务后再进行其他积分操作。" << endl;
    cout << "已注销: 账号不可继续使用。" << endl << endl;

    cout << "AI 助手配置" << endl;
    printDivider();
    cout << "AI 助手需要环境变量 DEEPSEEK_API_KEY。" << endl;
    cout << "PowerShell 临时配置示例: $env:DEEPSEEK_API_KEY=\"你的 API Key\"" << endl;
    cout << "不使用 AI 助手时，可以跳过配置。" << endl;

    finishPage();
}

void exitApp() {
    showPageHeader("退出系统");
    cout << "感谢使用留守儿童关爱系统。" << endl;
    finishPage();
}
