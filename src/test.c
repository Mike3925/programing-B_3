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

typedef struct address
{
  int code;       // 郵便番号
  char *pref;     // char pref[13]; //都道府県 MAX 4 x 3
  char city[32];  // 市町村 MAX 10 x 3
  char town[116]; // 町域 MAX 38 x 3
} ADDRESS;        // データ記録用構造体
// メモリ節約用都道府県名データ(jisコード順)
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
int mode;
ADDRESS address_data[MAX_SIZE];

// 住所データファイルを読み取り，配列に保存
void scan(ADDRESS address_data[])
{
  FILE *fp;
  long line = 0;
  char jis[6], code[CLEN + 1], city[ALEN + 1], town[ALEN + 1]; // tmp[ALEN+1];
  int jis_code;

  // datasizeの計算
  if ((fp = fopen(DATAFILE, "r")) == NULL)
  {
    fprintf(stderr, "error:cannot read %s\n", DATAFILE);
    exit(-1);
  }
  while (fscanf(fp, "%[^,],%*[^,],\"%[^\"]\",%*[^,],%*[^,],%*[^,],\"%*[^\"]\",\"%[^\"]\",\"%[^\"]\",%*s", jis, code, city, town) != EOF)
  {
    /*
      上のfscanfにより，code,pref,city,townにそれぞれ郵便番号，都道府県，市町村，町域を表す
      文字列が記憶される．この箇所にコードを加筆し，
　　　これらの情報を用いて構造体の配列に住所データを記憶させる．
     */
    jis_code = atoi(jis) / 1000; // 5桁のjisコードから都道府県コードを抽出
    address_data[line].code = atoi(code);
    address_data[line].pref = pref_names[jis_code - 1];
    // strcpy(address_data[line].pref, pref);
    strcpy(address_data[line].city, city);
    strcpy(address_data[line].town, town);
    line++;
  }
  // mergesort(address_index, 0, MAX_SIZE - 1);
  // qsort(address_index, line-1, sizeof(ADDRESS *), comp_int);
  // printf("sorted code\n");
  // for (int i = 0; i < 10; i++)
  // {
  //   printf("%d : %d (%s)\n", i, address_index[i]->code, address_index[i]->town);
  // }

  fclose(fp);
}

int isPref(char query[])
{
  for (int i = 0; i < 47; i++)
  {
    int current_line_index = 0;
    int query_index = 0;
    while (query[query_index] != pref_names[i][current_line_index] && current_line_index <= 2)
    { // 都道府県の最初の1文字とquery[0]を比較
      current_line_index++;
    }
    if (current_line_index > 2){
      continue;
    }
    while (pref_names[i][current_line_index] == query[query_index] && pref_names[i][current_line_index] != '\0' && query[query_index] != '\0'){
      current_line_index++;
      query_index++;
    }
    if (pref_names[i][current_line_index] == '\0')
    {
      return 1;
    }
  }
  return 0;
}

void input(char query[])
{
  printf("\n"
         "#########Top Menu#########\n"
         "# Search by postal code: 1\n"
         "# Search by address    : 2\n"
         "# Exit                 : 0\n"
         "> ");

  printf("\n");
  // scanf("%d", &mode);
  mode = 2;
  if (mode == 1)
  {
    printf("Postal code > ");
    scanf("%s", query);
  }
  else if (mode == 2)
  {
    printf("Search String > ");
    scanf("%s", query);
  }
}

int main(int argc, char **argv)
{
  // コマンドプロンプト、PowerShellを端末、あるいはVSCodeの端末上で使っている人は以下の4行のコメントを外すこと
  #ifndef __linux__
  SetConsoleOutputCP(CP_UTF8);
  SetConsoleCP(CP_UTF8);
  #endif
  setvbuf(stdout, NULL, _IONBF, 0);

  memset(address_data, 0, sizeof(address_data)); // 構造体の初期化
  // input(query);
  // scan(address_data);
  char query[] = "岡山県";
  char *result = strstr(pref_names[41], query);
  if (result != NULL){
    printf("%s", result);
  }
  // int a = isPref(query);
  // printf("%d",a);

  return 0;
}
