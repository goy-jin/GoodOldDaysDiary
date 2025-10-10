
/**
 * @file diary_struct.h
 * @brief 定义日记条目的数据结构和相关常量。
 * * 这是项目核心 C 语言数据层的结构定义。
 * 日记条目将被以二进制形式存储和读取。
 */

#ifndef DIARY_STRUCT_H
#define DIARY_STRUCT_H

// --- 常量定义 ---
/** 日记内容的最大长度 (字节)。 */
#define MAX_CONTENT_SIZE 4096

/** 日记日期的长度：格式 YYYY-MM-DD + 终止符 '\0' = 11 字节。 */
#define DATE_SIZE 11

#define MAX_WEEKDAY_SIZE 10
#define MAX_WEATHER_SIZE 32

/** 存储图片路径的最大长度 (保留未来扩展用)。 */
#define MAX_IMAGE_PATH_SIZE 256

/** 密码的最大长度。 */
#define MAX_PASSWORD_SIZE 32

// --- 核心结构体 ---

/**
 * @struct DiaryEntry
 * @brief 表示单个日记条目的数据结构。
 * * 这个结构体用于二进制文件 I/O，确保数据大小固定且易于读写。
 */
typedef struct DiaryEntry {

    char date[DATE_SIZE]; // 日期 (例如: "2023-10-27")。

    // 日记内容的主体。
    char content[MAX_CONTENT_SIZE];

    char weekday[MAX_WEEKDAY_SIZE];// 星期几
    char weather[MAX_WEATHER_SIZE]; // 天气情况
    char image_path[MAX_IMAGE_PATH_SIZE];

} DiaryEntry_t;

#endif // DIARY_STRUCT_H