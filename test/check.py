import pandas as pd
from scipy import stats
import numpy as np
import os

def analyze_significance(file_path, p=0.05):
    # 1. CSVの読み込み
    # header=None は、CSVの1行目からデータが始まっていることを示します
    df = pd.read_csv(file_path, header=None)
    
    # 2. データの抽出
    # iloc[0], iloc[1] でそれぞれ1行目、2行目を取得します
    # dropna() は、行の長さが異なる場合に発生する空データを除外します
    data1 = df.iloc[0].dropna().values
    data2 = df.iloc[1].dropna().values
    
    # 3. 統計的検定の実行 (ウェルチのt検定)
    # 2つのデータ群の平均値に差があるかを検定します
    # equal_var=False は「分散が等しくない」ことを想定した設定で、より汎用的で安全です
    t_stat, p_value = stats.ttest_ind(data1, data2, equal_var=False)
    
    # 4. 結果の出力
    print(f"--- 分析結果 ---")
    print(f"データ1 (1行目): 平均 {np.mean(data1):.2f}, 標準偏差 {np.std(data1):.5f}, 要素数 {len(data1)}")
    print(f"データ2 (2行目): 平均 {np.mean(data2):.2f}, 標準偏差 {np.std(data2):.5f}, 要素数 {len(data2)}")
    print(f"p値 (p-value): {p_value:.10f}")
    
    # 一般的な有意水準 5% (0.05) で判定
    if p_value < p:
        print(f"\n【判定】: 統計的に『有意な差があります』(p < {p})")
        print("※ 2つのデータの平均値の差は、偶然によるものではない可能性が高いです。")
    else:
        print("\n【判定】: 統計的に『有意な差はありません』(p >= 0.05)")
        print("※ 2つのデータの差は、誤差の範囲内である可能性があります。")

# --- 実行部分 ---
current_dir = os.path.dirname(__file__)
p = 0.01
# ここに実際のCSVファイル名を指定してください
file_name = r'data.csv' 
file_path = os.path.join(current_dir, file_name)

# ファイルが存在する場合のみ実行する処理（エラー回避のため）
try:
    analyze_significance(file_path, p)
except FileNotFoundError:
    print(f"エラー: '{file_path}' が見つかりませんでした。ファイル名を確認してください。")
except Exception as e:
    print(f"エラーが発生しました: {e}")