# 📡 ft_irc - C++ IRC Server Project

C++で実装する IRC サーバープロジェクトです。

---

## 📁 ディレクトリ構成

```bash
.
├── .clang-format
├── .github
│   ├── PULL_REQUEST_TEMPLATE.md
│   └── workflows
│       ├── build-check.yml
│       └── clang-format-check.yml
├── .gitignore
├── .husky/
├── README.md
├── changelog.config.js
├── commitlint.config.js
├── ft_irc/
├── package-lock.json
└── package.json
```

---

## 🛠 編集対象ファイル

- 実装：`ft_irc/`
- フォーマット設定：`.clang-format` (基本禁止)
- CI フォーマットチェック：`.github/workflows/clang-format-check.yml`
- CI ビルドチェック: `.github/workflows/clang-format-check.yml`

---

## 📋 ブランチポリシー

### `main` ブランチ（保護対象）

- Pull Request 経由でのみ更新
- 削除・直接 Push 禁止（fast-forward 禁止）
- コードレビューコメントの解決が必須
- `clang-format` チェック必須
- ビルドチェック必須
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
| 変数名   | snake_case     | `_client_fd`, `_buffer`, `idx`  |
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

### ✅ Commit メッセージ規則 (`git-cz` 推奨 \*後述)

```bash
<type>(対象): <概要>

--- 例 ---
feat(SRC): add UserSession class
fix(SRC): fix memory leak in IrcServer
refactor(SRC): refactor sendMessage function
feat(ENV): add .env.example file
```

## 🛠 初期セットアップ

1. packege.json をインストール

   ```bash
   npm install
   ```

2. clang-format(vscode 拡張機能) のインストールと設定

   - VSCode の拡張機能から `clang-format` をインストール
   - VSCode の設定ファイルに次の内容を追加

   ```json
   "files.autoSave": "onFocusChange",
   "editor.formatOnSave": true,
   "editor.formatOnType": true,
   "editor.defaultFormatter": "ms-vscode.cpptools",
   "files.insertFinalNewline": true,
   "files.trimFinalNewlines": true,
   ```

   - ファイルが保存されるたびに自動でフォーマットされる

3. `git-cz` (commit-lint) の使い方

   - コミットしたいファイルを `git add` する
   - 対象ファイルやディレクトリがステージングにある状態で `npx git-cz` を実行
   - `alias cz='npx git-cz'` などとしておくと楽？

## ✅ CI について

### clang-format-check

- `clang-format` を使用してコードのフォーマットをチェック
- フォーマットが不正な場合は、エラーを表示

### build-check

- `make pedant` を使用してコードをビルド
- ビルドが失敗した場合は、エラーを表示
- `./ft_irc` を実行して、サーバーが起動することを確認
- `valgrind --leak-check=full -error-exitcode=1 ./ft_irc` を実行して、メモリリークがないことを確認
