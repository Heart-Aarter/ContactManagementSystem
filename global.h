//
// Created by lenovo on 2026/4/10.
//

#ifndef CONTACTMANAGEMENTSYSTEM_GLOBAL_H
#define CONTACTMANAGEMENTSYSTEM_GLOBAL_H

#define FALSE 0
#define TRUE 1

#define TIMELENGTH 20
#define UNUSE 2
#define ENOUGHMONEY 3

#define ACCOUNT_NAME_MAX_LENGTH 18
#define ACCOUNT_NAME_LENGTH (ACCOUNT_NAME_MAX_LENGTH + 1)
#define PASSWORD_MAX_LENGTH 8
#define PASSWORD_LENGTH (PASSWORD_MAX_LENGTH + 1)

#define MAX_ACCOUNTS 100

#ifndef PROJECT_ROOT_PATH
#define PROJECT_ROOT_PATH "."
#endif

#define ACCOUNTPATH PROJECT_ROOT_PATH "/data/account.ams"
#define POINTSPATH PROJECT_ROOT_PATH "/data/points.ams"
#define MONEYPATH PROJECT_ROOT_PATH "/data/money.ams"
#define TRACKINGPATH PROJECT_ROOT_PATH "/data/tracking.ams"

#define UNIT 15                 // 最小积分累积单元(分钟)
#define POINT_RATE 1.0          // 每个单元累积积分
#define NEWCOMER_RATE 1.2       // 新人礼遇增长倍率
#define WEEKEND_RATE 2.0        // 周末爱心加倍倍率
#define NEWCOMER_DAYS 30        // 新人礼遇天数

#endif //CONTACTMANAGEMENTSYSTEM_GLOBAL_H
