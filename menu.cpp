#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>

#include "global.h"
#include "large_model.h"
#include "menu.h"
#include "service.h"
#include "tool.h"

using namespace std;

namespace {

bool validateNameLength(const char* pName) {
    if (getSize(pName) > ACCOUNT_NAME_MAX_LENGTH) {
        cout << "账号长度不能超过 " << ACCOUNT_NAME_MAX_LENGTH << " 个字符。" << endl;
        finishPage();
        return false;
    }
    return true;
}

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

void printUnavailableFeature(const char* pTitle) {
    showPageHeader(pTitle);
    cout << "该功能暂未开放。" << endl;
    finishPage();
}

bool inputAccountAndPassword(char* pName, int nNameSize, char* pPwd, int nPwdSize) {
    const string namePrompt = "请输入志愿者账号(长度为 1~" + to_string(ACCOUNT_NAME_MAX_LENGTH) + "): ";
    if (!inputBoundedText(namePrompt.c_str(), pName, nNameSize, ACCOUNT_NAME_MAX_LENGTH)) {
        return false;
    }

    getPwdOnce(pPwd);
    return validatePassword(pPwd);
}

void printAccountTable(const Account* pAccount, int nCount) {
    char aLastTime[TIMELENGTH] = {0};

    const int nNameColumnWidth = ACCOUNT_NAME_MAX_LENGTH + 4;

    cout << left
         << setw(nNameColumnWidth) << "账号"
         << setw(8) << "状态"
         << setw(12) << "当前积分"
         << setw(12) << "累计积分"
         << setw(12) << "累计时长"
         << "最后操作时间" << endl;
    for (int i = 0; i < nCount; i++) {
        timeToString(pAccount[i].tLast, aLastTime);
        cout << left
             << setw(nNameColumnWidth) << pAccount[i].aName
             << setw(8) << pAccount[i].nStatus
             << setw(12) << pAccount[i].fBalance
             << setw(12) << pAccount[i].fTotalPoints
             << setw(12) << pAccount[i].fTotalDuration
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

    char aName[ACCOUNT_NAME_LENGTH] = {0};
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

    if (!inputBoundedText("请输入要查询的志愿者账号: ", aName, sizeof(aName), ACCOUNT_NAME_MAX_LENGTH)) {
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

    char aName[ACCOUNT_NAME_LENGTH] = {0};
    char aPwd[PASSWORD_LENGTH] = {0};
    Start info{};
    char aTime[TIMELENGTH] = {0};

    if (!inputAccountAndPassword(aName, sizeof(aName), aPwd, sizeof(aPwd))) {
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

    char aName[ACCOUNT_NAME_LENGTH] = {0};
    char aPwd[PASSWORD_LENGTH] = {0};
    End info{};
    char aStartTime[TIMELENGTH] = {0};
    char aEndTime[TIMELENGTH] = {0};

    if (!inputAccountAndPassword(aName, sizeof(aName), aPwd, sizeof(aPwd))) {
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

    char aName[ACCOUNT_NAME_LENGTH] = {0};
    char aPwd[PASSWORD_LENGTH] = {0};
    float fAmount = 0;
    PointChangeRecord pointInfo{};

    if (!inputAccountAndPassword(aName, sizeof(aName), aPwd, sizeof(aPwd))) {
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
            cout << left
                 << setw(ACCOUNT_NAME_MAX_LENGTH + 4) << "账号"
                 << setw(16) << "本次获取积分"
                 << "当前积分" << endl;
            cout << left
                 << setw(ACCOUNT_NAME_MAX_LENGTH + 4) << pointInfo.aAccountName
                 << setw(16) << pointInfo.fChange
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

    char aName[ACCOUNT_NAME_LENGTH] = {0};
    char aPwd[PASSWORD_LENGTH] = {0};
    float fAmount = 0;
    PointChangeRecord pointInfo{};

    if (!inputAccountAndPassword(aName, sizeof(aName), aPwd, sizeof(aPwd))) {
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
            cout << left
                 << setw(ACCOUNT_NAME_MAX_LENGTH + 4) << "账号"
                 << setw(16) << "本次使用积分"
                 << "剩余积分" << endl;
            cout << left
                 << setw(ACCOUNT_NAME_MAX_LENGTH + 4) << pointInfo.aAccountName
                 << setw(16) << pointInfo.fChange
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

    if (!inputAccountAndPassword(account.aName, sizeof(account.aName), account.aPwd, sizeof(account.aPwd))) {
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
            cout << left
                 << setw(ACCOUNT_NAME_MAX_LENGTH + 4) << "账号"
                 << "退还积分" << endl;
            cout << left
                 << setw(ACCOUNT_NAME_MAX_LENGTH + 4) << account.aName
                 << account.fBalance << endl;
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
