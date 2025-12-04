#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef __linux__
#include <windows.h>
#endif

#define ADVANCED 0 // 発展課題（絞り込み検索）に対応する場合は1に変更

#define DATAFILE "data_utf.csv"
#define CLEN 9          // 郵便番号の最大バイト長
#define ALEN 200        // 住所欄の最大バイト長
#define MAX_SIZE 200000 // 住所録中の住所数の最大数

// 動作確認で使うファイル実行モードで使う定数。修正不要
#define STANDBY_MAIN 0
#define QUERY_POSTAL 1
#define QUERY_ADDRESS 2
#define STANDBY_REFINE 3
#define QUERY_REFINE 4
