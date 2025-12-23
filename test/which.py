import csv

def search_address_from_csv(filename, search_text):
    """
    CSVファイルを読み込み、search_textを含む行を検索し、
    整形した住所リストを返す関数
    """
    result_list = []

    # Windows等のExcelで作ったCSVや、日本郵便のデータは 'cp932' (Shift-JIS) が多いです。
    # VS Codeで作ったUTF-8のファイルなら 'utf-8' にしてください。
    encoding_type = 'utf-8' 

    try:
        with open(filename, 'r', encoding=encoding_type) as f:
            reader = csv.reader(f)
            
            for i,row in enumerate(reader):
                # 行が空だったり、カラムが足りない場合はスキップ
                if len(row) < 4:
                    continue

                # ユーザーの例: 0010000,"北海道","札幌市北区","以下に掲載がない場合"
                # row[0]: 郵便番号
                # row[1]: 都道府県 (例: 北海道)
                # row[2]: 市区町村 (例: 札幌市北区)
                # row[3]: 町域 (例: 以下に掲載がない場合)
                
                code = row[0]
                pref = row[1]
                city = row[2]
                town = row[3]

                # --- 【整形ロジック】 ---
                # "以下に掲載がない場合" は住所として不要なので空文字にする
                # if town == "以下に掲載がない場合":
                #     town = ""
                
                # 日本郵便データによくある "（...）" の注釈などを消したい場合はここに追記

                # 住所を結合
                full_address =  pref + city + town

                # --- 【検索ロジック】 ---
                # 検索ワード(text)が、結合した住所や郵便番号に含まれているか確認
                # 検索対象を広げたい場合は row全体を結合してチェックしてもOK
                search_target = pref + city + town + row[0]

                if search_text in search_target:
                    result_list.append([code + ":" + full_address, i])

    except FileNotFoundError:
        print(f"エラー: ファイル '{filename}' が見つかりません。")
        return []
    except UnicodeDecodeError:
        print(f"エラー: 文字コードが合いません。encoding='cp932' などを試してください。")
        return []

    return result_list

# --- 実行部分 ---

# 検索したい単語
text = "富士見"
csv_file = "test/sorted.csv"

# 関数呼び出し
found_addresses = search_address_from_csv(csv_file, text)

# csv_file = "test/out.csv"
# with open(csv_file, 'r', encoding='utf-8') as f:
#     reader = csv.reader(f)
#     for i, row in enumerate(reader):
#         print("A  " + str(row[0]))
#         print("B  " + str(found_addresses[i][0]))
        
              

# 結果の表示
print(f"検索ワード: '{text}'")
print(f"ヒット数: {len(found_addresses)}件")
# print("-" * 20)
# for addr in found_addresses:
#     print(addr)