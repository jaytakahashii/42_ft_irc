# IRC Server Test Suite

このリポジトリは、IRC サーバーの動作を自動的に検証するための **pytest + YAMLベースのテストスイート**です。複数クライアントの接続・メッセージ送信・受信・状態遷移を YAML によって宣言的に記述し、`test_runner.py` がそれを自動実行します。

---

## ✅ 特徴

- YAML による簡潔なテストケース定義
- 複数クライアント (`client0`, `client1`, ...) に対応
- `repeat` / `{{i}}` によるテストケースの展開
- `pytest` による簡易・高速なテスト実行と出力

---

## 📦 動作環境

- Python 3.8 以上
- UNIX 系 OS (Linux / macOS)
- IRC サーバーが `127.0.0.1:4242` にて起動していること

---

## 🛠 セットアップ手順

```bash
# 仮想環境の作成
python3 -m venv venv # 一度作成したら、以降は不要

# 仮想環境の有効化
source venv/bin/activate # 2回目以降はここから

# 依存ライブラリのインストール
pip install -r requirements.txt # 2回目以降は不要
```

## 🚀 テストの実行方法

1. IRC サーバーを起動します。

2. 以下のコマンドを実行して、テストを実行します。
```bash
pytest --tb=short --port=4242
# 4242: IRCサーバーのポート番号
# --tb=short は、pytestのオプションで、失敗時のトレースバックを簡潔に表示します。
```

## 🧪 テストケース記述ルール（YAML）

YAMLファイルは cases/ ディレクトリに配置してください。

### 基本構成

```yaml
- name: "テスト名"
  repeat: 3              # 任意、{{i}} と連動して複数展開されます
  steps:
    - send: "PASS password"
    - send: "NICK user{{i}}"
    - send: "USER user{{i}} 0 * :Real Name {{i}}"
    - receive: ":irc.42tokyo.jp 001 user{{i}} :Welcome to the Internet Relay Network user{{i}}!user{{i}}@127.0.0.1"
```

- steps: 各クライアントが送信・受信する IRC コマンド列

- send: クライアントからサーバーへ送信

- receive: サーバーからクライアントへの期待レスポンス

- repeat: {{i}} を展開して client0, client1, ... を自動生成

### 複数クライアントの例

README用の例であり、正確ではありません。実際のテストケースは適宜変更してください。

```yaml
- name: "privmsg client0 to client1"
  steps:
    - client: client0
      send: "JOIN #test"
      receive: ":irc.42tokyo.jp 353 client0 = #test :client0"
    - client: client1
      send: "JOIN #test"
      receive: ":irc.42tokyo.jp 353 client1 = #test :client0 client1"
    - client: client0
      send: "PRIVMSG #test :Hello, world!"
    - client: client1
      receive: ":irc.42tokyo.jp PRIVMSG #test :Hello, world!"
```

## 🧾 出力例（失敗時）

```bash
FAILED test_runner.py::test_case[Missing PASS command 3 times]

Test Name: Missing PASS command 3 times
Source:    authentication.yml
Client:    client1
Expected:  :irc.42tokyo.jp 464 * :Password incorrect
Received:  :irc.42tokyo.jp 484 * :Your connection is restricted!
```

## 📁 ディレクトリ構成

```bash
.
├── cases/                # YAMLで記述されたテストケース群
│   └── authentication.yml
├── test_runner.py        # テストランナー（pytestベース）
├── requirements.txt      # pip依存パッケージ
├── README.md             # このファイル
└── venv/                 # Python仮想環境（.gitignore推奨）
```

## 仮想環境の無効化

```bash
deactivate
```
