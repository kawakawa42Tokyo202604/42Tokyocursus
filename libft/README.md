*This project has been created as part of the 42 curriculum by \<takawaka\>.*

# Libft - はじめてのC言語ライブラリ

## 概要
**Libft** は、42のカリキュラムで最初に取り組むプロジェクトです。C言語の標準ライブラリ（libc）の一部を再実装し、後続の課題でも利用できるライブラリを作成します。

このプロジェクトを通じて、メモリ管理（malloc/free）、ポインタ演算、連結リストなどの動的データ構造について学びました。ソースコードは42のコーディング規範「Norm」に準拠しています。

## ライブラリの構成
ライブラリは、次の3つのカテゴリで構成されています。

- **第1部（Libc関数）:** `ft_strlen`、`ft_memcpy`、`ft_strnstr`、`ft_atoi` など、標準Cライブラリの基本関数を再実装しています。
- **第2部（追加関数）:** `ft_split`（文字列の分割）、`ft_itoa`（数値から文字列への変換）、`ft_strmapi`（文字列の各文字への関数適用）など、標準Cライブラリにはないユーティリティ関数を実装しています。
- **第3部（連結リスト）:** `ft_lstnew`、`ft_lstadd_back`、`ft_lstdelone`、`ft_lstmap` など、連結リストを操作する関数を実装しています。

## 使用方法
### コンパイル
付属のMakefileを使って、以下のコマンドでビルドします。

```bash
make          # libft.a を生成
make clean    # オブジェクトファイルを削除
make fclean   # オブジェクトファイルと libft.a を削除
make re       # fclean の後に再ビルド
```

## 参考資料
各関数の仕様は、対応する `man` ページで確認しました。

### AIの利用
READMEの構成と文章の見直しにAIを利用しました。
