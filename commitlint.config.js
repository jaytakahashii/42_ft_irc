module.exports = {
  extends: ['@commitlint/config-conventional'],
  rules: {
    'type-enum': [
      2, // エラー (1: 警告, 0: 無視)
      'always',
      ['feat', 'fix', 'refactor', 'Revert'],
    ],
    'scope-enum': [
      2,
      'always',
      ['SRC', 'DOC', 'CI', 'ENV'],
    ],
    // コミットメッセージの最大長を設定
    'header-max-length': [2, 'always', 64],
    // スコープを必須とする
    'scope-empty': [2, 'never'],
    // サブジェクトを必須とする
    'subject-empty': [2, 'never'],
  },
};
