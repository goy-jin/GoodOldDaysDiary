/**
* @file nostalgia_manager.c
 * @brief 核心日记数据管理器：实现数据的二进制存储、查找和命令行交互。
 */

#include <stdio.h>      // 文件 I/O (fopen, fwrite, fread, fclose)
#include <stdlib.h>     // 标准库函数 (EXIT_SUCCESS, EXIT_FAILURE)
#include <string.h>     // 字符串操作 (strcmp, strncpy)
#include <time.h>       // 时间获取 (time, localtime, strftime)
#include <locale.h>     // 本地化设置 (setlocale, 帮助处理中文乱码)

#include "diary_struct.h" // 引入我们定义的结构体和常量
#include <windows.h> // 必须引入
#include <io.h>
#include <fcntl.h>

// --- 宏和常量定义 ---
#define DIARY_FILE_PATH "Data/diary.dat" // 数据文件路径
#define PASSWORD "123456"      // 密码


// --- 核心函数声明 (私有/内部实现) ---
static int write_entry_to_file(const DiaryEntry_t* entry);
static void output_entry_to_stdout(const DiaryEntry_t* entry);

// --- 业务逻辑函数 (供 main 函数调用) ---
int authenticate_password(const char* password);
int handle_save_command(const char* weather, const char* content);
int handle_view_command(const char* date);
// 放在文件顶部，例如在其他函数声明的旁边
static void gbk_to_utf8_copy(const char *source_gbk, char *dest_utf8, size_t dest_size);


int main(int argc, char *argv[]) {

    // 2. 判断命令模式

    // 模式 A: 独立认证模式 (argc == 2)
    if (argc == 2) {
        printf("正在尝试独立认证...\n");
        // argv[1] 是密码
        return authenticate_password(argv[1]);

        // 模式 B: 保存模式 (argc == 5 且 argv[1] == "save")
    } else if (argc == 5 && strcmp(argv[1], "save") == 0) {
        // 密码位于 argv[2]
        const char *password = argv[2];
        const char *weather = argv[3];
        const char *content = argv[4];

        if (authenticate_password(password) != EXIT_SUCCESS) {
            fprintf(stderr, "操作终止：密码认证失败。\n");
            return EXIT_FAILURE;
        }

        printf("密码验证成功，正在保存日记...\n");
        return handle_save_command(weather, content);

        // 模式 C: 查看模式 (argc == 4 且 argv[1] == "view")
    } else if (argc == 4 && strcmp(argv[1], "view") == 0) {
        // 密码位于 argv[2]
        const char *password = argv[2];
        const char *date = argv[3];

        if (authenticate_password(password) != EXIT_SUCCESS) {
            fprintf(stderr, "操作终止：密码认证失败。\n");
            return EXIT_FAILURE;
        }

        printf("密码验证成功，正在查询日记...\n");
        return handle_view_command(date);

        // 模式 D: 无效命令或帮助信息
    } else {
        fprintf(stderr, "错误：参数无效或缺失。\n");
        fprintf(stderr, "用法:\n");
        fprintf(stderr, "  1. 独立认证: %s <密码>\n", argv[0]);
        fprintf(stderr, "  2. 保存日记: %s save <密码> \"<天气>\" \"<内容>\"\n", argv[0]);
        fprintf(stderr, "  3. 查看日记: %s view <密码> <YYYY-MM-DD>\n", argv[0]);
        return EXIT_FAILURE;
    }
}

/**
 * @brief 验证用户密码。
 */
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

/**
 * @brief 创建并保存一篇日记。
 * 【注意】由于时间自动获取，只接收用户输入的天气和内容。
 */
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
        return EXIT_FAILURE;
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



int handle_view_command(const char* date) {
    FILE* fp = fopen(DIARY_FILE_PATH, "rb");
    if (fp == NULL) {
        fprintf(stderr, "Error: Failed to open file %s\n", DIARY_FILE_PATH);
        return EXIT_FAILURE;
    }

    long file_size = 0;
    size_t entry_count = 0;
    // --- 1. 计算文件大小 ---

    // 从文件末尾 (SEEK_END) 开始，移动 0 个字节 (offset)。 fseek(文件指针, 偏移量, 起始位置)
    fseek(fp, 0, SEEK_END);
    // 获取当前文件指针的位置，这个位置就是文件末尾，其返回值就是文件的总字节数
    file_size = ftell(fp);
    if (file_size == 0) {
        fprintf(stderr, "Error: Failed to get size of file %s\n", DIARY_FILE_PATH);
        fclose(fp);
        return EXIT_FAILURE;
    }
    // 将文件指针移回文件开头，准备开始读取数据
    rewind(fp);

    entry_count = file_size / sizeof(DiaryEntry_t);
    // --- 3. 检查文件完整性（重要！）---
    // 如果文件总大小不是结构体大小的整数倍，说明文件损坏
    if (file_size % sizeof(DiaryEntry_t) != 0) {
        fprintf(stderr, "错误：数据文件 %s 可能已损坏，文件大小不完整。\n", DIARY_FILE_PATH);
        fclose(fp);
        return EXIT_FAILURE;
    }
    // 动态分配内存
    // 声明一个指针，用于接收分配的内存地址
    DiaryEntry_t* all_entries = NULL;
    // 计算所需的总字节数：条目数量 * 单个结构体大小
    all_entries = (DiaryEntry_t*) malloc(entry_count * sizeof(DiaryEntry_t));
    if (all_entries == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for all entries.\n");
        fclose(fp);
        return EXIT_FAILURE;
    }
    // 读取所有日记
    // 声明一个变量，用于接收实际读取到的条目数量
    size_t items_read = 0;
    /*
    从文件流 fp 的当前位置开始，尝试读取 entry_count 个大小为 sizeof(DiaryEntry_t) 的数据块，
    并将这些数据块按顺序存入内存地址 all_entries 开始的区域。最后，将实际成功读取的完整数据块的数量返回给我，存入 items_read 变量。
    */
    items_read = fread(
        all_entries,
        sizeof(DiaryEntry_t),
        entry_count,
        fp);
    fclose(fp);
    // 检查读取是否完整
    if (items_read != entry_count) {
        fprintf(stderr, "错误：读取日记数据失败，只读取了 %zu/%zu 个条目。\n", items_read, entry_count);
        free(all_entries);
        return EXIT_FAILURE;
    }

    int found = 0;//查找标记
    int record_count = 0; // 用于给同一天的记录编号
    int first_record_printed = 0; // 记录日期/星期是否已打印
    for (size_t i =0; i<entry_count; i++) {
        if (strcmp(all_entries[i].date,date) == 0) {
            if (first_record_printed == 0) {
                printf("日期：%s %s\n",all_entries[i].date, all_entries[i].weekday);
                first_record_printed = 1;
            }
            record_count++;//计数，用于输出分隔符
            // 打印分隔符，区分同一天的不同记录
            if (record_count > 1) {
                printf("--- 记录 %d 的内容 ---\n", record_count);
            }
            else {
                // 如果是第一条，可以不打印额外的分隔线，使用默认的格式
            }

            output_entry_to_stdout(&all_entries[i]);
            found = 1;
        }
    }
    free(all_entries);
    if (!found) {
        fprintf(stderr, "未找到日期 %s 的日记条目。\n", date);
        return EXIT_FAILURE;
    }

    // 增加一个总结性的分隔符，使输出更美观
    if (record_count>1) {
        printf("========================================\n");
    }
    return EXIT_SUCCESS;

}

/**
 * @brief 将单个日记条目结构体以美观的格式输出到标准输出流。
 */
static void output_entry_to_stdout(const DiaryEntry_t* entry) {
    printf("天气: %s\n", entry->weather);
    printf("日记内容:\n");
    printf("%s\n", entry->content);
    printf("========================================\n");
}

