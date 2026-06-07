*This project has been created as part of the 42 curriculum by \<takawaka\>.*

# ft_printf


## Description
`printf` を再実装することです。
書式指定文字列（Format string）を解析し、可変長引数を適切に処理して標準出力に表示するライブラリ
`libftprintf.a` を作成しました。

### Detailed explanation
与えられた文字列から1文字ずつ検査し、指定子が見つかった場合は単に文字を表示するだけでなく、数値を任意の基数に変換、ポインタアドレスの表示など、printfと同じ動作をするようにしました。
（対応している指定子は下記を参照）

### Justification of the chosen algorithm
1文字ずつ検査して出力しています。

### 対応している指定子
| 指定子 | 内容 |
| :--- | :--- |
| `%c` | 文字の出力 |
| `%s` | 文字列の出力 |
| `%p` | ポインタアドレス（16進数）の出力 |
| `%d / %i` | 符号付き10進整数の出力 |
| `%u` | 符号なし10進整数の出力 |
| `%x / %X` | 16進整数の出力（小文字 / 大文字） |
| `%%` | パーセント記号の出力 |

## Instructions

### Compilation
```
Makefileを使用してライブラリを作成します。
make        # .aを生成
make clean  # .oを削除
make fclean # 生成ファイルをすべて削除
make re     # fclean の後に　make

```

## Resources
AIの使用
<strdarg.h>の使い方について一時間ほど教えてもらい、subjectの翻訳や、細かい要件などを聞き、最後のチェックもしてもらいました。
READMEも手伝ってもらいました。
