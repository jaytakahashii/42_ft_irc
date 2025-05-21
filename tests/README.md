# IRC Server Test Suite

本ディレクトリは IRC サーバーの動作確認・自動テストを行うためのテストスイートです。

---

## 📦 必要環境

- Python 3.8 以上
- `venv` による仮想環境推奨
- 依存ライブラリは `requirements.txt` に記載されています

---

## 🛠 セットアップ手順

```bash
# 仮想環境の作成
python3 -m venv venv

# 仮想環境の有効化
source venv/bin/activate  # Mac/Linux

# 依存ライブラリのインストール
pip install -r requirements.txt

# 終了後
deactivate  # 仮想環境の無効化
```

## 🚀 テストの実行

```bash
python test_runner.py
```

オプションやテストケースの絞り込みがある場合は test_runner.py 側で対応してください。

## 📁 ディレクトリ構成

```bash
.
├── venv/ # 仮想環境（Git 追跡対象外）
├── cases/ # YAML 形式で定義されたテストケース
│ └── pass_nick_user.yml
├── test_runner.py # テストエントリポイント
├── requirements.txt # 依存ライブラリ一覧
└── README.md
```

## 🧪 テストケースの形式 (例)

```yml
- name: "登録に成功する"
  steps:
    - send: "PASS password"
    - send: "NICK testNick"
    - send: "USER user 0 * :Real Name"
  expect: ":irc.42tokyo.jp 001 testNick :Welcome to the Internet Relay Network testNick!user@127.0.0.1\r\n"

- name: "PASSなしでNICK送信 → エラーになる"
  steps:
    - send: "NICK testNick"
  expect: ":irc.42tokyo.jp 451 * :You have not registered\r\n"
```
