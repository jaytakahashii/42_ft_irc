# 📡 ft_irc - C++ IRC Server Project

C++で実装する IRC サーバープロジェクトです。本プロジェクトは保守性・拡張性を重視し、CI/CD やコードスタイル統一により高品質な開発を目指します。

---

## 📁 ディレクトリ構成

```bash
.
├── .clang-format                    # clang-format 設定ファイル
├── .github                          # GitHub 関連設定
│   ├── PULL_REQUEST_TEMPLATE.md    # プルリクエストテンプレート
│   └── workflows                   # GitHub Actions ワークフロー
│       └── clang-format-check.yml  # clang-format チェック
├── .gitignore
├── .husky                           # Husky 設定
│   └── commit-msg                  # コミットメッセージチェック
├── README.md
├── changelog.config.js              # Changelog 設定ファイル
├── commitlint.config.js             # Commitlint 設定ファイル
├── ft_irc/                          # プロジェクトソースコード
├── package-lock.json                # npm ロックファイル
└── package.json                     # npm 設定ファイル
```

---

## 🛠 編集対象ファイル

- 実装：`ft_irc/`
- フォーマット設定：`.clang-format`
- CI 設定：`.github/workflows/clang-format-check.yml`

---

## 📋 ブランチポリシー

### `main` ブランチ（保護対象）

- Pull Request 経由でのみ更新
- 削除・直接 Push 禁止（fast-forward 禁止）
- コードレビューコメントの解決が必須
- `clang-format` チェック必須
- `merge`, `squash`, `rebase` いずれかでマージ可能

### `develop` ブランチ（開発用）

- fast-forward 禁止
- 削除禁止
- CI ビルドチェックあり

---

## 🧾 開発ルール

### ✅ 命名規則

| 種別     | 規則           | 例                              |
| -------- | -------------- | ------------------------------- |
| クラス名 | UpperCamelCase | `UserSession`, `IrcServer`      |
| 変数名   | snake_case     | `_client_fd`, `_buffer`         |
| 関数名   | lowerCamelCase | `sendMessage()`, `handleJoin()` |

### ✅ コーディングスタイル

- `.clang-format` に準拠
- `-Wall -Wextra -Werror --std=c++98 --pedantic` でコンパイルが通ること
- `#pragma once` によるインクルードガードを使用
- ヘッダ内に関数実装を記述しない（テンプレート除く）

### ✅ Pull Request 規約

- `develop` に対して PR を作成
- 作業途中の場合は PR タイトルに `[WIP]` を含める or Draft で作成
- PR テンプレートに従って記述
- コードレビュー後にマージ（レビュー必須は今後導入予定）
- **CI チェック（clang-format, build check）通過必須**

### ✅ Commit メッセージ規則

```bash
<type>(対象): <概要>

--- 例 ---
feat(SRC): add UserSession class
fix(SRC): fix memory leak in IrcServer
refactor(SRC): refactor sendMessage function
feat(ENV): add .env.example file
```

### 初期セットアップ

```bash
npm install
```

### git-cz の使い方

- コミットメッセージを簡単に作成するためのツールです。
- コミットメッセージのテンプレートに従って、対話形式でメッセージを作成できます。
- コミットメッセージのテンプレートは、`commitlint.config.js` に定義されています。
- コミットメッセージのテンプレートは、`add` したあとに `npx git-cz` コマンドを実行すると自動的に表示されます。
