#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef __linux__
#include <windows.h>
#endif

#define ADVANCED 0 // 発展課題（絞り込み検索）に対応する場合は1に変更

#define DATAFILE "csv/data_utf.csv"
#define CLEN 9          // 郵便番号の最大バイト長
#define ALEN 200        // 住所欄の最大バイト長
#define MAX_SIZE 124340 // 住所録中の住所数の最大数

// 動作確認で使うファイル実行モードで使う定数。修正不要
#define STANDBY_MAIN 0
#define QUERY_POSTAL 1
#define QUERY_ADDRESS 2
#define STANDBY_REFINE 3
#define QUERY_REFINE 4

int comp(const void *a, const void *b){
  return *(int *)a - *(int *)b;
}

char pref_names[47][17] = {
    "北海道", "青森県", "岩手県", "宮城県", "秋田県",
    "山形県", "福島県", "茨城県", "栃木県", "群馬県",
    "埼玉県", "千葉県", "東京都", "神奈川県", "新潟県",
    "富山県", "石川県", "福井県", "山梨県", "長野県",
    "岐阜県", "静岡県", "愛知県", "三重県", "滋賀県",
    "京都府", "大阪府", "兵庫県", "奈良県", "和歌山県",
    "鳥取県", "島根県", "岡山県", "広島県", "山口県",
    "徳島県", "香川県", "愛媛県", "高知県", "福岡県",
    "佐賀県", "長崎県", "熊本県", "大分県", "宮崎県",
    "鹿児島県", "沖縄県"};

char query[] = "山口県";

int isPref()
{
  for (int i = 0; i < 47; i++)
  {
    char *hit_pointer = strstr(query, pref_names[i]);
    if (hit_pointer != NULL)
    {
      return 1;
    }
  }
  return 0;
}

int main()
{
  SetConsoleOutputCP(65001);
  if (isPref()){
    printf("TRUE");
  }else{
    printf("FALSE");
  }
};