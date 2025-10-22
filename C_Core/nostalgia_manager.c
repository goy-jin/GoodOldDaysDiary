/**
* @file nostalgia_manager.c
 * @brief 核心日记数据管理器：实现数据的二进制存储、查找和命令行交互。
 */

#include <stdio.h>      // 文件 I/O (fopen, fwrite, fread, fclose)
#include <stdlib.h>     // 标准库函数 (EXIT_SUCCESS, EXIT_FAILURE)
#include <string.h>     // 字符串操作 (strcmp, strncpy)
#include <time.h>       // 【新增】时间获取 (time, localtime, strftime)
#include <locale.h>     // 【新增】本地化设置 (setlocale, 帮助处理中文乱码)

#include "diary_struct.h" // 引入我们定义的结构体和常量

// --- 宏和常量定义 ---
#define DIARY_FILE_PATH "Data/diary.dat" // 数据文件路径
#define PASSWORD "123456"      // 密码


// --- 核心函数声明 (私有/内部实现) ---

static int write_entry_to_file(const DiaryEntry_t* entry);
static void output_entry_to_stdout(const DiaryEntry_t* entry);


// --- 业务逻辑函数 (供 main 函数调用) ---

/**
 * @brief 验证用户密码。
 */
int authenticate_password(const char* password);


/**
 * @brief 创建并保存一篇日记。
 * 【注意】由于时间自动获取，只接收用户输入的天气和内容。
 */
int handle_save_command(const char* weather, const char* content);

/**
 * @brief 根据日期查找日记。
 */
int handle_view_command(const char* date);




int authenticate_password(const char* password)
{
  if (strcmp(password, PASSWORD) == 0) {
      printf("密码正确\n");
      return EXIT_SUCCESS;
  }
    else {
        fprintf(stderr, "错误：密码错误，请重试。\n");
        return EXIT_FAILURE;
    }
}


static const char* chinese_weekdays[] = {
    "星期日", "星期一", "星期二", "星期三","星期四", "星期五", "星期六"
};
int handle_save_command(const char* weather, const char* content) {
    // 初始化一个新的日记条目，确保所有字段（尤其是字符串）安全清零。
    DiaryEntry_t  new_entry;
    memset(&new_entry, 0,sizeof(DiaryEntry_t));
    time_t now = time(NULL); //获取原始时间
    struct tm local_time_struct = *localtime(&now);//解指针.转换为本地时间
    size_t len = strftime(
            new_entry.date,          // 参数 1: 目标缓冲区 (char 数组)
            DATE_SIZE,               // 参数 2: 目标缓冲区的最大容量 (11 字节)
            "%Y-%m-%d",              // 参数 3: 格式字符串
            &local_time_struct       // 参数 4: 源数据 (struct tm 结构的地址)
        );
    if (len == 0) {
        fprintf(stderr,"Error: Failed to format date into YYYY-MM-DD.\n");
        return EXIT_FAILURE;
    }

    const char* cn_day = chinese_weekdays[local_time_struct.tm_wday];//转成星期几
    strncpy(new_entry.weekday, cn_day, MAX_WEEKDAY_SIZE - 1);// 关键修正：使用 strncpy 复制整个字符串

    strncpy(new_entry.weather,weather,MAX_WEATHER_SIZE - 1);
    strncpy(new_entry.content,content,MAX_CONTENT_SIZE - 1);

    int save_status = write_entry_to_file(&new_entry);
    if ( save_status != EXIT_SUCCESS ) {
        return EXIT_FAILURE;
    }
    printf("日记保存成功！\n");
    return EXIT_SUCCESS;
}

/**
 * @brief 将单个日记条目以二进制形式写入数据文件。
 * * @param entry 指向要写入文件的 DiaryEntry_t 结构体的指针。
 * @return int 成功返回 EXIT_SUCCESS (0)，失败返回 EXIT_FAILURE (1)。
 */
static int write_entry_to_file(const DiaryEntry_t* entry) {
    //  打开文件：使用 "ab" (追加二进制) 模式打开文件。
    FILE* fp = fopen(DIARY_FILE_PATH, "ab");
    // 检查文件是否打开成功。
    if (fp == NULL) {
        fprintf(stderr, "Error: Failed to open file %s\n", DIARY_FILE_PATH);
    }
    //写入数据：将 entry 结构体的全部内容作为一个块写入文件。
    size_t items_written = fwrite(
        entry,
        sizeof(DiaryEntry_t),
        1,
        fp);
    if (items_written != 1) {
        fprintf(stderr, "Error: Failed to write to file %s\n", DIARY_FILE_PATH);
        // 在返回失败之前，必须关闭文件句柄，避免资源泄露。
        fclose(fp);
        return EXIT_FAILURE;
    }
    // 关闭文件句柄并返回成功。
    // 确保数据从缓冲区冲刷到硬盘，并释放资源。
    fclose(fp);
    return EXIT_SUCCESS;
}



