#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <iostream>

#include "global.h"
#include "large_model.h"
#include "menu.h"
#include "service.h"
#include "tool.h"

using namespace std;

namespace {

bool validateNameLength(const char* pName) {
    if (getSize(pName) >= 18) {
        cout << "账号长度不能超过 17 个字符。" << endl;
        finishPage();
        return false;
    }
    return true;
}

bool validatePasswordLength(const char* pPwd) {
    if (getSize(pPwd) >= 8) {
        cout << "密码长度不能超过 7 个字符。" << endl;
        finishPage();
        return false;
    }
    return true;
}

void printUnavailableFeature(const char* pTitle) {
    showPageHeader(pTitle);
    cout << "该功能暂未开放。" << endl;
    finishPage();
}

void inputAccountAndPassword(char* pName, int nNameSize, char* pPwd, int nPwdSize) {
    cout << "请输入志愿者账号(长度为 1~17): ";
    cin >> setw(nNameSize) >> pName;

    cout << "请输入密码(长度为 1~7): ";
    cin >> setw(nPwdSize) >> pPwd;
}

void printAccountTable(const Account* pAccount, int nCount) {
    char aLastTime[TIMELENGTH] = {0};

    cout << "账号\t状态\t当前积分\t累计积分\t累计时长\t最后操作时间" << endl;
    for (int i = 0; i < nCount; i++) {
        timeToString(pAccount[i].tLast, aLastTime);
        cout << pAccount[i].aName << '\t'
             << pAccount[i].nStatus << '\t'
             << pAccount[i].fBalance << '\t'
             << pAccount[i].fTotalPoints << '\t'
             << pAccount[i].fTotalDuration << '\t'
             << aLastTime << endl;
    }
}

}

void outputMenu() {
    cout << "========== 留守儿童关爱系统 ==========" << endl << endl;
    cout << "1. 注册志愿者" << endl;
    cout << "2. 查询志愿者" << endl;
    cout << "3. 开始服务" << endl;
    cout << "4. 结束服务" << endl;
    cout << "5. 获取积分" << endl;
    cout << "6. 使用积分" << endl;
    cout << "7. 注销账号" << endl;
    cout << "8. 物资捐赠" << endl;
    cout << "9. AI 助手" << endl;
    cout << "10. 统计查询" << endl;
    cout << "0. 退出系统" << endl << endl;
    cout << "请选择操作(0~10): ";
}

void add() {
    showPageHeader("注册志愿者");

    char aName[18] = {0};
    char aPwd[8] = {0};
    Account account{};
    Account* pAccount = nullptr;
    int nIndex = 0;

    cout << "请输入志愿者账号(长度为 1~17): ";
    cin >> setw(sizeof(aName)) >> aName;
    if (!validateNameLength(aName)) {
        return;
    }

    getPwd(aPwd);
    if (!validatePasswordLength(aPwd)) {
        return;
    }

    pAccount = queryAccountInfo(aName, &nIndex);
    if (pAccount != nullptr) {
        cout << endl << "账号已存在，注册失败。" << endl;
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
    if (FALSE == addAccountInfo(account)) {
        cout << "注册失败。" << endl;
    } else {
        cout << "注册成功。" << endl;
    }

    finishPage();
}

void query() {
    showPageHeader("查询志愿者");

    char aName[18] = {0};
    Account* pAccount = nullptr;
    int nCount = 0;
    int nMode = 0;

    cout << "1. 按账号查询" << endl;
    cout << "2. 查看所有志愿者" << endl << endl;
    cout << "请选择查询方式(1~2): ";
    cin >> nMode;
    if (!cin) {
        resetInput();
        cout << endl << "输入有误，请输入 1 或 2。" << endl;
        finishPage();
        return;
    }

    if (nMode == 2) {
        pAccount = queryAllAccountInfo(&nCount);
        cout << endl;
        if (pAccount == nullptr || nCount == 0) {
            cout << "当前没有志愿者记录。" << endl;
            finishPage();
            return;
        }

        printAccountTable(pAccount, nCount);
        free(pAccount);
        finishPage();
        return;
    }

    if (nMode != 1) {
        cout << endl << "查询方式不存在。" << endl;
        finishPage();
        return;
    }

    cout << "请输入要查询的志愿者账号: ";
    cin >> setw(sizeof(aName)) >> aName;
    if (!validateNameLength(aName)) {
        return;
    }

    pAccount = queryAccountInfo(aName, &nCount);
    cout << endl;
    if (pAccount == nullptr) {
        cout << "未找到该账号。" << endl;
        finishPage();
        return;
    }

    printAccountTable(pAccount, nCount);

    free(pAccount);
    finishPage();
}

void logon() {
    showPageHeader("开始服务");

    char aName[18] = {0};
    char aPwd[8] = {0};
    Start info{};
    char aTime[TIMELENGTH] = {0};

    inputAccountAndPassword(aName, sizeof(aName), aPwd, sizeof(aPwd));
    if (!validateNameLength(aName) || !validatePasswordLength(aPwd)) {
        return;
    }

    const int nResult = startServiceInfo(aName, aPwd, &info);

    cout << endl;
    switch (nResult) {
        case FALSE:
            cout << "开始服务失败，请检查账号和密码。" << endl;
            break;
        case TRUE:
            timeToString(info.tLogon, aTime);
            cout << "开始服务成功。" << endl << endl;
            cout << "账号: " << info.aName << endl;
            cout << "当前积分: " << info.fBalance << endl;
            cout << "开始时间: " << aTime << endl;
            break;
        case UNUSE:
            cout << "该账号正在服务或已注销，无法开始服务。" << endl;
            break;
        case ENOUGHMONEY:
            cout << "账号积分不足。" << endl;
            break;
        default:
            cout << "开始服务发生未知错误。" << endl;
            break;
    }

    finishPage();
}

void settle() {
    showPageHeader("结束服务");

    char aName[18] = {0};
    char aPwd[8] = {0};
    End info{};
    char aStartTime[TIMELENGTH] = {0};
    char aEndTime[TIMELENGTH] = {0};

    inputAccountAndPassword(aName, sizeof(aName), aPwd, sizeof(aPwd));
    if (!validateNameLength(aName) || !validatePasswordLength(aPwd)) {
        return;
    }

    const int nResult = endServiceInfo(aName, aPwd, &info);

    cout << endl;
    switch (nResult) {
        case FALSE:
            cout << "结束服务失败，请检查账号和密码。" << endl;
            break;
        case TRUE:
            timeToString(info.tStart, aStartTime);
            timeToString(info.tEnd, aEndTime);
            cout << "结束服务成功。" << endl << endl;
            cout << "账号: " << info.aName << endl;
            cout << "本次获得积分: " << info.fAmount << endl;
            cout << "当前积分: " << info.fBalance << endl;
            cout << "开始时间: " << aStartTime << endl;
            cout << "结束时间: " << aEndTime << endl;
            break;
        case UNUSE:
            cout << "该账号当前没有正在进行的服务。" << endl;
            break;
        case ENOUGHMONEY:
            cout << "账号积分不足。" << endl;
            break;
        default:
            cout << "结束服务发生未知错误。" << endl;
            break;
    }

    finishPage();
}

void addPoint() {
    showPageHeader("获取积分");

    char aName[18] = {0};
    char aPwd[8] = {0};
    float fAmount = 0;
    PointChangeRecord pointInfo{};

    inputAccountAndPassword(aName, sizeof(aName), aPwd, sizeof(aPwd));
    if (!validateNameLength(aName) || !validatePasswordLength(aPwd)) {
        return;
    }

    cout << "请输入获取积分数量: ";
    cin >> fAmount;
    if (!cin || fAmount <= 0) {
        resetInput();
        cout << endl << "积分数量必须为正数。" << endl;
        finishPage();
        return;
    }

    pointInfo.fChange = fAmount;
    const int nResult = doAddPointInfo(aName, aPwd, &pointInfo);

    cout << endl;
    switch (nResult) {
        case FALSE:
            cout << "获取积分失败，请检查账号、密码和账号状态。" << endl;
            break;
        case TRUE:
            cout << "账号\t本次获取积分\t当前积分" << endl;
            cout << pointInfo.aAccountName << '\t'
                 << pointInfo.fChange << "\t\t"
                 << pointInfo.fBalance << endl;
            break;
        case UNUSE:
            cout << "该账号正在服务或已注销，无法获取积分。" << endl;
            break;
        default:
            cout << "获取积分发生未知错误。" << endl;
            break;
    }

    finishPage();
}

void usePoint() {
    showPageHeader("使用积分");

    char aName[18] = {0};
    char aPwd[8] = {0};
    float fAmount = 0;
    PointChangeRecord pointInfo{};

    inputAccountAndPassword(aName, sizeof(aName), aPwd, sizeof(aPwd));
    if (!validateNameLength(aName) || !validatePasswordLength(aPwd)) {
        return;
    }

    cout << "请输入使用积分数量: ";
    cin >> fAmount;
    if (!cin || fAmount <= 0) {
        resetInput();
        cout << endl << "积分数量必须为正数。" << endl;
        finishPage();
        return;
    }

    pointInfo.fChange = fAmount;
    const int nResult = doRefundPointInfo(aName, aPwd, &pointInfo);

    cout << endl;
    switch (nResult) {
        case FALSE:
            cout << "使用积分失败，请检查账号和密码。" << endl;
            break;
        case TRUE:
            cout << "账号\t本次使用积分\t剩余积分" << endl;
            cout << pointInfo.aAccountName << '\t'
                 << pointInfo.fChange << "\t\t"
                 << pointInfo.fBalance << endl;
            cout << "积分折合为 " << pointInfo.fChange / 10.0f << " 元，已捐赠。" << endl;
            break;
        case UNUSE:
            cout << "该账号正在服务或已注销，无法使用积分。" << endl;
            break;
        case ENOUGHMONEY:
            cout << "账号积分不足。" << endl;
            break;
        default:
            cout << "使用积分发生未知错误。" << endl;
            break;
    }

    finishPage();
}

void addMoney() {
    addPoint();
}

void refundMoney() {
    usePoint();
}

void annul() {
    showPageHeader("注销账号");

    Account account{};
    char cConfirm = '\0';

    inputAccountAndPassword(account.aName, sizeof(account.aName), account.aPwd, sizeof(account.aPwd));
    if (!validateNameLength(account.aName) || !validatePasswordLength(account.aPwd)) {
        return;
    }

    cout << "注销后账号不可恢复，确认注销吗？(Y/N): ";
    cin >> cConfirm;
    if (cConfirm != 'Y' && cConfirm != 'y') {
        cout << endl << "已取消注销。" << endl;
        finishPage();
        return;
    }

    const int nResult = annulAccount(&account);

    cout << endl;
    switch (nResult) {
        case FALSE:
            cout << "注销账号失败，请检查账号和密码。" << endl;
            break;
        case TRUE:
            cout << "账号\t退还积分" << endl;
            cout << account.aName << '\t' << account.fBalance << endl;
            break;
        case UNUSE:
            cout << "该账号正在服务或已注销，无法注销。" << endl;
            break;
        default:
            cout << "注销账号发生未知错误。" << endl;
            break;
    }

    finishPage();
}

void donate() {
    printUnavailableFeature("物资捐赠");
}

void aiAssistant() {
    showPageHeader("AI 助手");
    resetInput();
    runDeepSeekAssistant();
    finishPage();
}

void statistics() {
    showPageHeader("统计查询");

    StatisticsInfo info{};
    if (FALSE == getStatisticsInfo(&info)) {
        cout << "统计数据读取失败。" << endl;
        finishPage();
        return;
    }

    cout << "账号统计" << endl;
    cout << "志愿者总数: " << info.nAccountCount << endl;
    cout << "正常账号: " << info.nNormalAccountCount << endl;
    cout << "服务中账号: " << info.nServingAccountCount << endl;
    cout << "已注销账号: " << info.nAnnulledAccountCount << endl;
    cout << endl;

    cout << "积分与服务汇总" << endl;
    cout << "当前积分总余额: " << info.fTotalBalance << endl;
    cout << "累计获得积分: " << info.fTotalPoints << endl;
    cout << "累计服务时长: " << info.fTotalDuration << " 小时" << endl;
    cout << "累计开始服务次数: " << info.nTotalUseCount << endl;
    cout << endl;

    cout << "服务记录统计" << endl;
    cout << "服务记录总数: " << info.nTrackingCount << endl;
    cout << "进行中服务记录: " << info.nRunningTrackingCount << endl;
    cout << "已完成服务记录: " << info.nCompletedTrackingCount << endl;
    cout << endl;

    cout << "积分流水统计" << endl;
    cout << "积分流水总数: " << info.nPointRecordCount << endl;
    cout << "手动获取积分合计: " << info.fAddedPoints << endl;
    cout << "使用积分合计: " << info.fUsedPoints << endl;
    cout << "捐赠积分合计: " << info.fDonatedPoints << endl;

    finishPage();
}

void exitApp() {
    showPageHeader("退出系统");
    cout << "感谢使用留守儿童关爱系统。" << endl;
    finishPage();
}
