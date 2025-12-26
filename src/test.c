#include <stdlib.h>
#include <stdio.h>
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

char pref_names[] = "北海道\0青森県\0岩手県\0宮城県\0秋田県\0山形県\0福島県\0茨城県\0栃木県\0群馬県\0埼玉県\0千葉県\0東京都\0神奈川県\0新潟県\0富山県\0石川県\0福井県\0山梨県\0長野県\0岐阜県\0静岡県\0愛知県\0三重県\0滋賀県\0京都府\0大阪府\0兵庫県\0奈良県\0和歌山県\0鳥取県\0島根県\0岡山県\0広島県\0山口県\0徳島県\0香川県\0愛媛県\0高知県\0福岡県\0佐賀県\0長崎県\0熊本県\0大分県\0宮崎県\0鹿児島県\0沖縄県\0";

char *get_pref_index(char pref[])
{
  char *pref_p = strstr(pref_names, pref);
  return pref_p;
}

int main()
{
  char *p = get_pref_index("青森県");
  printf("%s", p);
};