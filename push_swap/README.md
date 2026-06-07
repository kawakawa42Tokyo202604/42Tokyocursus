*This project has been created as part of the 42 curriculum by \<amakino\>,\<takawaka\>.*

# push_swap

## Description
`push_swap` is a C program that sorts a given sequence of integers using two stacks (Stack A and Stack B) and a limited set of instructions, aiming to find the sequence with the minimum number of operations.

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

### 1. Simple Strategy — $O(n^2)$
A straightforward algorithm combining selection sort logic with pattern matching specifically optimized for a small number of elements.
*   **Characteristics:** For extremely small datasets of 2 to 5 elements, it completes the sorting using minimal comparisons and hardcoded branching to achieve the theoretical minimum number of operations (within 3 moves for 3 elements, and 12 moves for 5 elements). For larger sets within this strategy, it uses a selection sort approach: searching for the minimum element in the stack, moving it efficiently to the top using `ra` or `rra`, and pushing it to Stack B (`pb`).

### 2. Medium Strategy (Chunk Sort) — $O(n\sqrt{n})$
An algorithm that divides numbers into several blocks (chunks) and pushes them to Stack B sequentially, starting with the smallest chunks.
*   **Characteristics:** The number of chunks is dynamically adjusted based roughly on the square root of the data size ($\sqrt{n}$) (e.g., chunks of 16 for 100 elements, or chunks of 42 for 500 elements). When pushing elements to Stack B, it separates them into an "upper half" and a "lower half" within each chunk, creating a rough pre-sorted state inside Stack B. This consistently achieves efficiency well below the 42 maximum evaluation thresholds ("under 700 operations for 100 elements" and "under 5500 operations for 500 elements").

### 3. Complex Strategy (Radix Sort) — $O(n \log n)$
An algorithm that treats data at the bit level (binary representation), evaluating numbers from the least significant bit to the most significant bit to sort them into Stack B.
*   **Characteristics:** By pre-processing all values through "coordinate compression (indexing)" starting from 0, this approach seamlessly handles negative numbers and large integer values. The implementation is highly concise and less prone to bugs. Furthermore, its execution step count is completely independent of the initial order of the data (such as worst-case scenarios), always completing in a predictable number of moves.

### 4. Adaptive Strategy (Default Behavior)
This strategy dynamically analyzes the "disorder percentage" of the input data and automatically selects and executes the algorithm deemed most suitable or predicted to yield the fewest operations.

---

## Instructions

### Compilation
The program is compiled using a Makefile. It does not utilize any environment variables or global variables, ensuring no accidental relinking occurs.
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
* **\<amakino\>**: Responsible for the entire system architecture and infrastructure surrounding the algorithms. This includes robust argument parsing, strict error handling (validating duplicates, non-integers, and integer limits), stack initialization, coordinate compression (indexing), the metadata-driven Adaptive logic, the standard error-based benchmark mode (`--bench`), and the complete development of the bonus component (`checker` engineered alongside a custom `get_next_line` system).

## Resources

* https://ja.wikipedia.org/wiki/%E9%81%B8%E6%8A%9E%E3%82%BD%E3%83%BC%E3%83%88
* https://qiita.com/MoriP-K/items/54ee96dc634148cf40a8
* https://qiita.com/tommyecguitar/items/3c1897bceda4a06beef2
* https://qiita.com/r-ngtm/items/f4fa55c77459f63a5228
* https://qiita.com/wanyawanya/items/45b037375a3a24b620da
* Additional insights and discussions were gathered from the 42 Discord community.

### Use of AI

AI was consulted to conceptualize basic sorting algorithms and their practical applications to stacks (specifically concerning Radix Sort and Chunk Sort). It was also utilized to identify edge cases—such as integer limit inputs and invalid arguments—as well as to structure and refine the wording of this README file.

```
```
# push_swap

## Description
`push_swap` は、2つのスタック（Stack A と Stack B）と限定された命令セットを使用して、与えられた整数列を最小の命令数でソートするC言語のプログラムです。

### Detailed explanation
プログラムは引数として整数のリストを受け取り、パースおよびエラーチェック（重複の有無、非整数入力、整数オーバーフローの確認）を行います。有効なデータは Stack A に格納されます。

ソートを達成するために、2つのスタック間で要素を移動・回転させる専用の命令を組み合わせて、データを効率的に並び替えます。最終的に、ソートを完了するまでに実行した命令のシーケンスを標準出力に改行区切りで表示します。

### 使用可能な命令
| 命令 | 内容 |
| :--- | :--- |
| `sa` / `sb` | Stack A または B の先頭2つの要素を入れ替える |
| `ss` | `sa` と `sb` を同時に実行する |
| `pa` / `pb` | Stack B(A) の先頭要素を Stack A(B) の先頭に移動する |
| `ra` / `rb` | Stack A または B の要素を1つ上にずらす（先頭が最後尾になる） |
| `rr` | `ra` と `rb` を同時に実行する |
| `rra` / `rrb` | Stack A または B の要素を1つ下にずらす（最後尾が先頭になる） |
| `rrr` | `rra` と `rrb` を同時に実行する |

---

## Justification of the chosen algorithm
本プロジェクトでは、要素数やデータの無秩序度（Disorder）、指定されたフラグに応じてアルゴリズムを動的に切り替えるアプローチを採用しています。

### 1. Simple Strategy — $O(n^2)$
選択ソート（Selection Sort）のロジックと、少数の要素に特化したパターンマッチングを組み合わせた、シンプルなアルゴリズムです。
*   **特徴:** 2〜5個の極めて少数の要素に対しては、最小限の比較と分岐（ハードコーディング）により、理論上の最小手数（3個は3手、5個は12手以内）でソートを完結させます。要素数がそれ以上の場合には、スタック内から最小の要素を探索し、`ra` や `rra` を使って効率的にトップへ移動させてから Stack B へプッシュ（`pb`）する選択ソートの手法を適用し、確実にソートを行います。

### 2. Medium Strategy (Chunk Sort) — $O(n\sqrt{n})$
数値をいくつかのブロック（チャンク）に分割し、値の小さいチャンクから順に Stack B へ送るアルゴリズムです。
*   **特徴:** チャンク数をデータの平方根（$\sqrt{n}$）を目安に適切に調整（例: 100個なら16個ずつ、500個なら42個ずつ）します。Stack B に送る際、各チャンク内の要素を「上半分」と「下半分」に振り分けることで、Stack B 内でおおまかなソート状態を作ります。42の最高評価基準である「100個で700手未満」「500個で5500手未満」を下回る手数を実現できます。

### 3. Complex Strategy (Radix Sort) — $O(n \log n)$
データをビット（2進数）単位で扱い、下位ビットから順番に評価して Stack B に振り分けるアルゴリズムです。
*   **特徴:** 事前にすべての数値を 0 から順に「座標圧縮（インデックス化）」しておくことで、負の数や大きな値にも対応させます。実装が非常にシンプルでバグが混入しにくく、データの並び順（最悪のケースなど）に依存せず、常に一定のステップ数でソートが完了します。

### 4. Adaptive Strategy (デフォルト動作)
入力データの「無秩序度（Disorder %）」を動的に解析し、上記の戦略から最も手数が少なくなる、あるいは最も適していると判断されたアルゴリズムを自動的に選択して実行します。

---

## Instructions

### Compilation
Makefile を使用してプログラムをコンパイルします。環境変数やグローバル変数は一切使用しておらず、再リンク（relink）は発生しません。
```bash
make        # 実行ファイル push_swap を生成
make clean  # .o（オブジェクトファイル）を削除
make fclean # 生成ファイルをすべて削除
make re     # fclean の後に make を実行
make bonus  # checker file を生成
```

### Execution & Usage
引数にソートしたい整数のリストを渡して実行します。引数が指定されない場合は何も表示せず、そのままプロンプトに戻ります。

#### 基本実行（Adaptive モードが自動選択されます）
```bash
./push_swap 4 67 3 87 23
```

#### 戦略選択フラグ
入力サイズやデータの状態に関わらず、以下のフラグを使用してアルゴリズムを強制指定できます。
*   `--simple`: $O(n^2)$ アルゴリズムを使用
*   `--medium`: $O(n\sqrt{n})$ アルゴリズムを使用
*   `--complex`: $O(n \log n)$ アルゴリズムを使用
*   `--adaptive`: 無秩序度に基づき自動選択（デフォルト）

```bash
./push_swap --medium 4 67 3 87 23
```

#### ベンチマークモード (`--bench`)
`--bench` フラグを有効にすると、ソート用の命令シーケンスを出力したあと、以下の統計情報を**標準エラー出力（stderr）**に表示します。
*   計算された無秩序度（少数第2位までのパーセンテージ）
*   使用された戦略名と、その理論的計算量クラス
*   総命令手数
*   命令タイプごとの実行回数内訳（`sa`, `sb`, `ss`, `pa`, `pb`, `ra`, `rb`, `rr`, `rra`, `rrb`, `rrr`）

```bash
./push_swap --bench --medium 4 67 3 87 23
```
#### 2. checker の実行（ボーナス）
標準入力から命令を受け取り、スタックが正しくソートされるかを検証します。
* ソート成功時: `OK` を出力
* ソート失敗時: `KO` を出力
* 無効な命令が入力された時: 標準エラー出力に `Error` を出力して終了

#### パイプラインによる連携
`push_swap` の出力を直接 `checker` に渡すことで、正確性を自動検証できます。
```bash
ARG="4 67 3 87 23"; ./push_swap $ARG | ./checker $ARG
# 出力: OK

#### パイプラインによる連携
`push_swap` の出力を直接 `checker` に渡すことで、アルゴリズムの正確性を検証できます。
```bash
ARG="4 67 3 87 23"; ./push_swap $ARG | ./checker $ARG
```

#### エラーハンドリング
重複値、整数範囲外の数値などの不正な引数が渡された場合、標準エラー出力（stderr）に `Error` と改行を出力します。
```bash
./push_swap 1 2 3 2
Error
```

---

## 役割分担 (Division of Labor)
本プロジェクトは、明確な役割分担のもとで共同開発を行いました。
* **\<takawaka\>**: コアとなるソートアルゴリズム（$O(n^2)$ Simpleソート、$O(n\sqrt{n})$ チャンクソート、$O(n \log n)$ 基数ソート）の設計および実装を担当、加えて座標圧縮（インデックス化）、チャンクソートの動的対応。あと、amakinoの精神の精神安定剤として機能。
* **\<amakino\>**: アルゴリズム周辺のシステム基盤およびインフラすべての実装を担当。具体的には、複数引数やスペース区切りのパース、オーバーフローや重複等の厳格なエラーハンドリング、スタックの初期化、無秩序度（Disorder）の動的解析アルゴリズム切り替えロジック、標準エラー出力を用いたベンチマークモード（`--bench`）、および自作 `get_next_line` を含むボーナス要素（`checker`）の完全実装。あと、takawakaの精神の精神安定剤として機能。

## Resources

* https://ja.wikipedia.org/wiki/%E9%81%B8%E6%8A%9E%E3%82%BD%E3%83%BC%E3%83%88
* https://qiita.com/MoriP-K/items/54ee96dc634148cf40a8
* https://qiita.com/tommyecguitar/items/3c1897bceda4a06beef2
* https://qiita.com/r-ngtm/items/f4fa55c77459f63a5228
* https://qiita.com/wanyawanya/items/45b037375a3a24b620da
* その他42のdiscordも参考にしました。

### AIの使用
基本的なソートアルゴリズム（基数ソートやチャンクソートのスタック応用）の概念理解について相談しました。また、エッジケース（最大値・最小値の入力や不正な引数）の洗い出しや、このREADMEの構成・推敲をサポートしてもらいました。
