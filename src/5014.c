#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// コマンドプロンプト、PowerShellを端末、あるいはVSCodeの端末上で使っている人は以下の3行のコメントを外すこと
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

int mode;         // 検索モード 0:なし，1:郵便番号検索，2:文字列検索
int refine_flag;  // 絞り込み検索の有無 0:なし，1:あり
char query[ALEN]; // 検索クエリ（郵便番号or文字列）

// 住所データファイルを読み取り，配列に保存
void scan()
{
  FILE *fp;
  long line = 0;
  char code[CLEN + 1], pref[ALEN + 1], city[ALEN + 1], town[ALEN + 1]; // tmp[ALEN+1];

  // datasizeの計算
  if ((fp = fopen(DATAFILE, "r")) == NULL)
  {
    fprintf(stderr, "error:cannot read %s\n", DATAFILE);
    exit(-1);
  }
  while (fscanf(fp, "%*[^,],%*[^,],%[^,],%*[^,],%*[^,],%*[^,],%[^,],%[^,],%[^,],%*s", code, pref, city, town) != EOF)
  {
    /*
      上のfscanfにより，code,pref,city,townにそれぞれ郵便番号，都道府県，市町村，町域を表す
      文字列が記憶される．この箇所にコードを加筆し，
　　　これらの情報を用いて構造体の配列に住所データを記憶させる．
     */
    line++;
  }
  fclose(fp);
}

double diff_time(clock_t t1, clock_t t2)
{
  return (double)(t2 - t1) / CLOCKS_PER_SEC;
}

// 初期化処理
void init()
{
  clock_t t1, t2;

  t1 = clock();
  scan();
  t2 = clock();
  // printf("\n### %f sec for initialization. ###\n",diff_time(t1,t2));
}

// 郵便番号による住所検索．検索結果を出力．
void code_search()
{
  return;
}

// 文字列による住所検索．検索結果を出力．
void address_search()
{
  return;
}

// 絞り込み検索の実施
void refinement()
{
  return;
}

void input()
{
  printf("\n"
         "#########Top Menu#########\n"
         "# Search by postal code: 1\n"
         "# Search by address    : 2\n"
         "# Exit                 : 0\n"
         "> ");

  scanf("%d", &mode);
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

// 絞り込み検索の有無を確認
void re_input()
{
  printf("\n"
         "# Continue Searching: 1\n"
         "# Return to Top Menu: 0\n"
         "> ");
  scanf("%d", &refine_flag);
  if (refine_flag == 1)
  {
    printf("String for Refinement> ");
    scanf("%s", query);
  }
}

// クエリへの応答
void respond()
{
  clock_t t1, t2;
  mode = 1;
  while (1)
  {
    input();

    if (mode == 1)
    {
      t1 = clock();
      code_search();
      t2 = clock();
      printf("\n### %f sec for search. ###\n", diff_time(t1, t2));
    }
    else if (mode == 2)
    {
      t1 = clock();
      address_search();
      t2 = clock();
      printf("\n### %f sec for search. ###\n", diff_time(t1, t2));
      if (!ADVANCED)
        continue;
      while (1)
      {
        re_input();
        if (refine_flag == 0)
          break;
        t1 = clock();
        refinement();
        t2 = clock();
        printf("\n### %f sec for search. ###\n", diff_time(t1, t2));
      }
    }
    else
      break;
  }
}

/*
動作確認時のファイル実行モード用の処理。
コマンド、検索対象の郵便番号、または住所の列をテキストファイルから受け取り、コード検索を行う。
このコードは変更しない。
*/
void run_from_file(const char *filename)
{
  FILE *fp = fopen(filename, "r");
  if (!fp)
  {
    fprintf(stderr, "cannot open test file: %s\n", filename);
    exit(1);
  }
  char buf[256];
  int state = STANDBY_MAIN;
  while (fgets(buf, sizeof(buf), fp))
  {
    buf[strcspn(buf, "\r\n")] = '\0';
    if (state == STANDBY_MAIN)
    {
      int transit = atoi(buf);
      if (transit == 1)
      {
        state = QUERY_POSTAL;
      }
      else if (transit == 2)
      {
        state = QUERY_ADDRESS;
      }
      else if (transit == 0)
      {
        exit(0);
      }
      else
      {
        printf("The current state is on STANDBY_MAIN. \n");
        exit(0);
      }
    }
    else if (state == QUERY_POSTAL)
    {
      strcpy(query, buf);
      code_search();
      state = STANDBY_MAIN;
    }
    else if (state == QUERY_ADDRESS)
    {
      strcpy(query, buf);
      address_search();
      state = STANDBY_REFINE;
    }
    else if (state == STANDBY_REFINE)
    {
      int transit = atoi(buf);
      if (transit == 0)
      {
        state = STANDBY_MAIN;
      }
      else if (transit == 1)
      {
        state = QUERY_REFINE;
      }
      else
      {
        printf("The current state is on STANDBY_REFINE. \n");
        exit(0);
      }
    }
    else if (state == QUERY_REFINE)
    {
      strcpy(query, buf);
      refinement();
      state = STANDBY_REFINE;
    }
    else
    {
      printf("Non-existing state occurs.\n");
      exit(0);
    }
  }
  fclose(fp);
}

int main(int argc, char **argv)
{
  // コマンドプロンプト、PowerShellを端末、あるいはVSCodeの端末上で使っている人は以下の4行のコメントを外すこと
  #ifndef __linux__
  SetConsoleOutputCP(CP_UTF8);
  SetConsoleCP(CP_UTF8);
  #endif

  setvbuf(stdout, NULL, _IONBF, 0);
  init();
  if (argc > 1)
  {
    run_from_file(argv[1]);
  }
  else
  {
    respond();
  }
  return 0;
}
