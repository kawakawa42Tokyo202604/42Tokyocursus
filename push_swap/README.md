*This project has been created as part of the 42 curriculum by \<amakino\>,\<takawaka\>.*

# push_swap

## Description
`push_swap` is a C program that sorts a given sequence of integers using two stacks (Stack A and Stack B) and a limited set of instructions. It selects an algorithm based on the input size, disorder, and any strategy flag.

### Detailed Explanation
The program receives a list of integers as arguments, parses them, and performs error checking (validating for duplicates, non-integer inputs, and integer overflows). Valid data is then loaded into Stack A.

To achieve sorting, the program combines specialized instructions to move and rotate elements between the two stacks, rearranging the data efficiently. Finally, it outputs the completed sequence of instructions to the standard output, separated by newlines.

### Available Instructions
| Instruction | Description |
| :--- | :--- |
| `sa` / `sb` | Swap the first two elements at the top of Stack A or B. |
| `ss` | Execute `sa` and `sb` at the same time. |
| `pa` / `pb` | Take the first element at the top of Stack B(A) and put it at the top of Stack A(B). |
| `ra` / `rb` | Shift up all elements of Stack A or B by 1 (the first element becomes the last one). |
| `rr` | Execute `ra` and `rb` at the same time. |
| `rra` / `rrb` | Shift down all elements of Stack A or B by 1 (the last element becomes the first one). |
| `rrr` | Execute `rra` and `rrb` at the same time. |

---

## Justification of the Chosen Algorithm
This project adopts a dynamic approach that switches algorithms based on the number of elements, the data's level of disorder, and specified flags.

### 1. Simple 戦略 — $O(n^2)$
A straightforward algorithm combining selection sort logic with pattern matching specifically optimized for a small number of elements.
*   **Characteristics:** Inputs of 2 or 3 elements use dedicated operation sequences. For larger inputs, this strategy repeatedly moves the smallest remaining index to Stack B, sorts the final three elements, and moves the saved elements back to Stack A.

### 2. Medium 戦略（Chunk Sort）
An algorithm that divides numbers into several blocks (chunks) and pushes them to Stack B sequentially, starting with the smallest chunks.
*   **Characteristics:** The chunk width depends on the input size: 8 for up to 50 elements, 12 for up to 100, 30 for up to 500, and `size / 12` for larger inputs. The algorithm pushes indices to Stack B in increasing ranges, rotating the lowest indices within B, then brings the largest remaining index back to Stack A each time. Operation counts depend on the input; benchmark them with `--bench` for your test cases.

### 3. Complex 戦略（Radix Sort）— $O(n \log n)$
An algorithm that treats data at the bit level (binary representation), evaluating numbers from the least significant bit to the most significant bit to sort them into Stack B.
*   **Characteristics:** Coordinate compression maps each value to its sorted rank, so the radix passes operate on non-negative indices even when the input contains negative or large integers. For a fixed input size, the number of bit passes is predictable and does not depend on the original values.

### 4. Adaptive Strategy (Default Behavior)
In adaptive mode, inputs of up to five elements use the simple strategy. For larger inputs, the program uses the measured disorder: below 20% it selects simple; below 80%, or for up to 500 elements, it selects medium; otherwise it selects radix. These are fixed heuristics, not a comparison of predicted operation counts.

---

## 使用方法

### コンパイル
The program is compiled using the included Makefile.
```bash
make        # Generates the executable file: push_swap
make clean  # Removes object files (.o)
make fclean # Removes all generated files (including the executable)
make re     # Executes fclean followed by make
make bonus  # Generates checker file : checker
```

### Execution & Usage

Run the program by passing a list of integers to sort as arguments. If no arguments are provided, the program displays nothing and returns to the prompt.

#### Basic Execution (Adaptive mode is selected automatically)

```bash
./push_swap 4 67 3 87 23
```

#### Strategy Selection Flags

You can force the program to use a specific algorithm regardless of the input size or data status by using the following flags:

* `--simple`: Forces the $O(n^2)$ algorithm
* `--medium`: Forces the $O(n\sqrt{n})$ algorithm
* `--complex`: Forces the $O(n \log n)$ algorithm
* `--adaptive`: Automatically selects based on disorder (Default)

```bash
./push_swap --medium 4 67 3 87 23
```

#### Benchmark Mode (`--bench`)

Enabling the `--bench` flag prints the standard sorting instruction sequence first, followed by detailed statistical information displayed in the **standard error output (stderr)**:

* Calculated disorder percentage (formatted to two decimal places)
* The name of the chosen strategy and its theoretical computational complexity class
* Total operation count
* Breakdown of execution frequency for each instruction type (`sa`, `sb`, `ss`, `pa`, `pb`, `ra`, `rb`, `rr`, `rra`, `rrb`, `rrr`)

```bash
./push_swap --bench --medium 4 67 3 87 23
```

#### Checker Execution (Bonus Component)

The checker program reads instructions from the standard input and verifies if they successfully sort the given stack.

#### Pipeline Integration

You can directly verify the correctness of the algorithm by piping the output of `push_swap` into `checker`.

```bash
ARG="4 67 3 87 23"; ./push_swap $ARG | ./checker $ARG
```

#### Error Handling

If invalid arguments are provided—such as duplicate values or numbers outside the integer range—the program outputs `Error` followed by a newline to the standard error output (stderr).

```bash
./push_swap 1 2 3 2
Error
```
---

## Credits / Division of Labor
This project was co-developed with a clear and distinct division of roles:
* **\<takawaka\>**: Conceptualized, designed, and implemented the core sorting algorithms ($O(n^2)$ Simple Sort, $O(n\sqrt{n})$ Chunk Sort, and $O(n \log n)$ Radix Sort).
* **\<amakino\>**: Implemented argument parsing, error handling, stack initialization, coordinate compression, adaptive strategy selection, benchmark output (`--bench`), and the bonus `checker` with a custom `get_next_line`.

## Resources

* https://ja.wikipedia.org/wiki/%E9%81%B8%E6%8A%9E%E3%82%BD%E3%83%BC%E3%83%88
* https://qiita.com/MoriP-K/items/54ee96dc634148cf40a8
* https://qiita.com/tommyecguitar/items/3c1897bceda4a06beef2
* https://qiita.com/r-ngtm/items/f4fa55c77459f63a5228
* https://qiita.com/wanyawanya/items/45b037375a3a24b620da
* Additional insights and discussions were gathered from the 42 Discord community.

### Use of AI

AI was consulted to conceptualize basic sorting algorithms and their practical applications to stacks (specifically concerning Radix Sort and Chunk Sort). It was also utilized to identify edge cases—such as integer limit inputs and invalid arguments—as well as to structure and refine the wording of this README file.

# push_swap

## 概要
`push_swap` は、2つのスタック（スタックAとスタックB）と限られた命令セットを使って整数列を整列するC言語のプログラムです。入力サイズや乱雑度（disorder）、指定されたフラグに応じて戦略を選びます。

### 処理の流れ
プログラムはコマンドライン引数から整数列を読み取り、入力形式、重複、整数オーバーフローを検査します。問題のない値をスタックAに格納します。

2つのスタック間で要素を移動・回転する命令を組み合わせ、整数列を整列します。生成した命令は標準出力に1行ずつ出力されます。

### 使用可能な命令
| 命令 | 内容 |
| :--- | :--- |
| `sa` / `sb` | スタックAまたはスタックBの先頭2要素を入れ替える |
| `ss` | `sa` と `sb` を同時に実行する |
| `pa` | スタックBの先頭要素をスタックAの先頭へ移す |
| `pb` | スタックAの先頭要素をスタックBの先頭へ移す |
| `ra` / `rb` | スタックAまたはスタックBの先頭要素を末尾へ移す |
| `rr` | `ra` と `rb` を同時に実行する |
| `rra` / `rrb` | スタックAまたはスタックBの末尾要素を先頭へ移す |
| `rrr` | `rra` と `rrb` を同時に実行する |

---

## アルゴリズムの選び方
要素数、入力データの乱雑度、指定されたフラグに応じてソート戦略を切り替えます。

### 1. Simple戦略 — $O(n^2)$
選択ソートを応用した戦略です。入力が2個または3個の場合は、専用の命令列で整列します。それより大きい場合は、最小のインデックスを順にスタックBへ移します。スタックAに残った3個を整列してから、スタックBの要素を戻します。

### 2. Medium戦略（チャンクソート）
インデックスを範囲（チャンク）ごとに分けてスタックBへ移し、最後にスタックAへ戻す戦略です。

チャンク幅は入力サイズに応じて決まります。要素数が50以下の場合は8、100以下の場合は12、500以下の場合は30、それを超える場合は `size / 12` です。インデックスの小さい要素からスタックBへ移し、スタックBを回転させながら配置します。その後、最大のインデックスから順にスタックAへ戻します。必要な命令数は入力によって異なるため、`--bench`で測定できます。

### 3. Complex戦略（基数ソート）— $O(n \log n)$
座標圧縮により、各値を昇順の順位に対応するインデックスへ変換し、そのビットを下位から順に処理します。この方法で、負の数や大きな整数を含む入力も扱えます。同じ要素数であれば、必要なビット走査回数は一定です。

### 4. Adaptive戦略（デフォルト）
Adaptiveモードでは、まず要素数が5以下の場合にSimple戦略を使います。それ以外では、乱雑度が20%未満ならSimple戦略を選びます。残りの入力は、要素数が500以下、または乱雑度が80%未満ならMedium戦略を、それ以外はComplex戦略を選びます。戦略ごとの予測命令数を比較するのではなく、固定のしきい値に基づいて選択します。

---

## 使い方

### コンパイル
付属のMakefileを使ってコンパイルします。
```bash
make        # 実行ファイル push_swap を生成
make clean  # .o（オブジェクトファイル）を削除
make fclean # 生成ファイルをすべて削除
make re     # fclean の後に make を実行
make bonus  # checker を生成
```

### 実行
ソートする整数を引数に指定して実行します。引数がない場合は、何も出力せずに終了します。

#### 基本的な実行（Adaptiveモード）
```bash
./push_swap 4 67 3 87 23
```

#### 戦略の指定
次のフラグを使って、ソート戦略を指定できます。
*   `--simple`: Simple戦略を使用
*   `--medium`: チャンクソートを使用
*   `--complex`: 基数ソートを使用
*   `--adaptive`: 乱雑度に基づいて自動選択（デフォルト）

```bash
./push_swap --medium 4 67 3 87 23
```

#### ベンチマーク（`--bench`）
`--bench`を指定すると、ソート命令を標準出力に出力した後、次の統計情報を標準エラー出力（stderr）に表示します。
*   算出した乱雑度（小数第2位まで）
*   使用した戦略名と理論計算量
*   総命令手数
*   命令タイプごとの実行回数内訳（`sa`, `sb`, `ss`, `pa`, `pb`, `ra`, `rb`, `rr`, `rra`, `rrb`, `rrr`）

```bash
./push_swap --bench --medium 4 67 3 87 23
```
#### checkerの実行（ボーナス）
標準入力から命令を受け取り、スタックが正しく整列されるかを検証します。
* 整列に成功した場合: `OK`を出力
* 整列に失敗した場合: `KO`を出力
* 無効な命令が入力された場合: 標準エラー出力に`Error`を出力して終了

#### パイプラインでの確認
`push_swap`の出力を`checker`に渡して、スタックが正しく整列するかを確認できます。
```bash
ARG="4 67 3 87 23"; ./push_swap $ARG | ./checker $ARG
```

#### エラー処理
重複した値や整数の範囲外の値など、不正な引数が渡された場合は、標準エラー出力（stderr）に`Error`と改行を出力します。
```bash
./push_swap 1 2 3 2
Error
```

---

## 役割分担
次のように分担して開発しました。
* **\<takawaka\>**: Simple、Chunk、Radixの各ソート戦略を設計・実装しました。
* **\<amakino\>**: 引数の解析、エラーチェック、スタックの初期化、座標圧縮、Adaptive戦略の選択、`--bench`、および自作の`get_next_line`を使ったボーナスの`checker`を実装しました。

## 参考資料

* https://ja.wikipedia.org/wiki/%E9%81%B8%E6%8A%9E%E3%82%BD%E3%83%BC%E3%83%88
* https://qiita.com/MoriP-K/items/54ee96dc634148cf40a8
* https://qiita.com/tommyecguitar/items/3c1897bceda4a06beef2
* https://qiita.com/r-ngtm/items/f4fa55c77459f63a5228
* https://qiita.com/wanyawanya/items/45b037375a3a24b620da
* 42のDiscordで共有された議論も参考にしました。

### AIの利用
基数ソートやチャンクソートをスタックに応用する方法の理解、最大値・最小値や不正な引数などの境界条件の洗い出し、READMEの構成と推敲にAIを利用しました。
