*This project has been created as part of the 42 curriculum by \<takawaka\>.*

# Libft - はじめてのC言語ライブラリ

## Description
**Libft** は、42のカリキュラムにおける最初のプロジェクトです。このプロジェクトの目的は、C言語の標準ライブラリ（libc）の一部の関数を再構築し、その後のプロジェクトで活用できる自分専用のユーティリティライブラリを作成することにあります。

このプロジェクトを通じて、メモリ管理（malloc/free）、ポインタ演算、そして連結リストのような動的なデータ構造の仕組みを深く理解することができました。すべてのコードは42の厳格なコーディング規約「Norm」に準拠しています。

## Detailed Description of the Library
このライブラリには、大きく分けて3つのパートが含まれています。

- **Part 1 (Libc Functions):** `ft_strlen`, `ft_memcpy`, `ft_strnstr`, `ft_atoi` など、標準Cライブラリの基本的な関数を再実装したものです。
- **Part 2 (Additional Functions):** `ft_split`（文字列の分割）、`ft_itoa`（数値から文字列への変換）、`ft_strmapi`（文字列への関数適用）など、標準ライブラリにはないが便利なユーティリティ関数群です。
- **(Linked Lists):** 動的なデータ管理を可能にする連結リスト操作関数（`ft_lstnew`, `ft_lstadd_back`, `ft_lstdelone`, `ft_lstmap` など）です。

## Instructions
### Compilation（コンパイル）
Makefileを使用してコンパイルを行います。ターミナルで以下のコマンドを実行してください。

```bash```
make          # 関数をコンパイルし libft.a を作成します
make clean    # オブジェクトファイル (.o) を削除します
make fclean   # オブジェクトファイルと libft.a を削除します
make re       # fcleanの後に再ビルドを行います

## Resources
man で情報を調べてやりました。
AiにREADMEを手伝ってもらい、チェックもしてもらいました。
