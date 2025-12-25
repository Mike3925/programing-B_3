#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// コマンドプロンプト、PowerShellを端末、あるいはVSCodeの端末上で使っている人は以下の3行のコメントを外すこと
#ifndef __linux__
#include <windows.h>
#endif


#define ADVANCED 0 // 発展課題（絞り込み検索）に対応する場合は1に変更

#define CLEN 9          // 郵便番号の最大バイト長
#define ALEN 200        // 住所欄の最大バイト長
#define DATAFILE "csv/data_utf.csv"
#define MAX_SIZE 124340// 住所録中の住所数の最大数

// #define DATAFILE "csv/light.csv"
// #define MAX_SIZE 2 // 住所録中の住所数の最大数

// 動作確認で使うファイル実行モードで使う定数。修正不要
#define STANDBY_MAIN 0
#define QUERY_POSTAL 1
#define QUERY_ADDRESS 2
#define STANDBY_REFINE 3
#define QUERY_REFINE 4

int mode;         // 検索モード 0:なし，1:郵便番号検索，2:文字列検索
int refine_flag;  // 絞り込み検索の有無 0:なし，1:あり
char query[ALEN]; // 検索クエリ（郵便番号or文字列）
typedef struct address{
  int code; //郵便番号
  char *pref;  //char pref[13]; //都道府県 MAX 4 x 3
  char city[32]; //市町村 MAX 10 x 3
  char town[116]; //町域 MAX 38 x 3
} ADDRESS; // データ記録用構造体
ADDRESS address_data[MAX_SIZE];
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

// 住所データファイルを読み取り，配列に保存
void scan()
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
    address_data[line].code = atoi(code);
    jis_code = atoi(jis)/1000; //5桁のjisコードから都道府県コードを抽出
    address_data[line].pref = pref_names[jis_code - 1];
    // strcpy(address_data[line].pref, pref);
    strcpy(address_data[line].city, city);
    strcpy(address_data[line].town, town);
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
  memset(address_data, 0, sizeof(address_data)); //構造体の初期化
  clock_t t1, t2;

  t1 = clock();
  scan();
  t2 = clock();
  // printf("\n### %f sec for initialization. ###\n",diff_time(t1,t2));
}

int binary_search(int search_code, int left, int right, ADDRESS *address_index[]){
  int half;
  while (left <= right){
    half = (left+right)/2;
    if (search_code < address_index[half]->code){
      right = half-1;
    }
    if (search_code > address_index[half]->code){
      left = half+1;
    }
    if (search_code == address_index[half]->code){
      return half;
    }
  }
  return -1;
}

void search_around(int index, int *left, int *right, ADDRESS *address_index[]){
  if (index > 0){
    *left = index;
    while (address_index[*left-1]->code == address_index[index]->code)  (*left)--;
  }
  if (index < MAX_SIZE - 1){
    *right = index;
    while (address_index[*right + 1]->code == address_index[index]->code) (*right)++;
  }
}

// comp for qsort()
int comp_adr(const void *vxp, const void *vyp)
{
  ADDRESS *xp = *(ADDRESS **)vxp;
  ADDRESS *yp = *(ADDRESS **)vyp;
  return xp->code - yp->code;
}

// 郵便番号による住所検索．検索結果を出力．
void code_search()
{
  if (query[0] == '\0' || query[0] == '\n')
  {
    return;
  }
  static int isSorted = 0;
  static ADDRESS *address_search[MAX_SIZE];
  if (!(isSorted)){
    for (int i=0; i< MAX_SIZE; i++){
      address_search[i] = &address_data[i];
    }
    qsort(address_search, MAX_SIZE, sizeof(ADDRESS *), comp_adr);
    isSorted = 1;
  }
  int search_code = atoi(query);
  int result = binary_search(search_code, 0, MAX_SIZE - 1, address_search);
  if (result != -1){
    int left , right;
    search_around(result, &left, &right, address_search);
    for (int i = left; i <= right; i++){
      printf("%07d:%s%s%s\n", address_search[i]->code, address_search[i]->pref, address_search[i]->city, address_search[i]->town);
    }
  }else{
    printf("ERROR\n");
  }
  return;
}

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

int line_search(int line)
{
  char address_line[170];
  strcpy(address_line, address_data[line].pref);
  strcat(address_line, address_data[line].city);
  strcat(address_line, address_data[line].town);
  if (strstr(address_line, query) != NULL)
  {
    return 1;
  }
  return 0;
}

int pref_search(int line)
{
  if (strstr(query, address_data[line].pref) != NULL)
  {
    return 1;
  }
  return 0;
}

int comp_code(const void *a, const void *b){
  // printf("%d\n", *(int *)a);
  return address_data[*(int *)a].code - address_data[*(int *)b].code;
}

// 文字列による住所検索．検索結果を出力．
void address_search()
{
  if (query[0] == '\0' || query[0] == '\n')
  {
    return;
  }
  static int hit_index_list[MAX_SIZE];
  memset(hit_index_list, -1, sizeof(hit_index_list));
  int hit_list_index = 0;
  int query_index = 0;
  int current_line_index = 0;
  int isPrefHit = isPref();
  int line = 0;

  if (isPrefHit)
  {
    while (!(pref_search(line)))
    {
      line++;
    }
    while (line < MAX_SIZE)
    {
      if (line_search(line))
      {
        hit_index_list[hit_list_index] = line;
        hit_list_index++;
      }
      if (!(pref_search(line)))
      {
        break;
      }
      line++;
    }
  }
  while (line < MAX_SIZE)
  {
    if (line_search(line))
    {
      hit_index_list[hit_list_index] = line;
      hit_list_index++;
    }
    line++;
  }
  qsort(hit_index_list, hit_list_index, sizeof(int), comp_code);
  for (int i = 0; i < hit_list_index; i++)
  {
    printf("%07d:%s%s%s\n", address_data[hit_index_list[i]].code, address_data[hit_index_list[i]].pref, address_data[hit_index_list[i]].city, address_data[hit_index_list[i]].town);
  }
  // printf("%d\n", hit_list_index);
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
