#include <stdio.h>
#include <time.h>
#include <stdlib.h> // 包含 EXIT_SUCCESS
#include "diary_struct.h" // 引入我们定义的结构体和常量
#include <corecrt.h>
#include <string.h>     // 字符串操作 (strcmp, strncpy)

// static const char* chinese_weekdays[] = {
//     "星期日", "星期一", "星期二", "星期三","星期四", "星期五", "星期六"
// };

static const char* chinese_weekdays[] = {  // 虽然变量名仍为chinese_weekdays，但内容已改为英文
    "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
};
int main() {
    DiaryEntry_t new_entry;
    // 1. 获取原始时间 (time_t)
    time_t now = time(NULL);

    // 2. 转换为本地时间结构体 (struct tm)
    struct tm tm = *localtime(&now);


    size_t len = strftime(
            new_entry.date,          // 参数 1: 目标缓冲区 (char 数组)
            DATE_SIZE,               // 参数 2: 目标缓冲区的最大容量 (11 字节)
            "%Y-%m-%d",              // 参数 3: 格式字符串
            &tm       // 参数 4: 源数据 (struct tm 结构的地址)
        );
    const char* cn_day = chinese_weekdays[tm.tm_wday];//转成星期几
    strncpy(new_entry.weekday, cn_day, MAX_WEEKDAY_SIZE - 1);// 关键修正：使用 strncpy 复制整个字符串
    printf(" %s\n", new_entry.weekday);
}