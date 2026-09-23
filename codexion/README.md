# Codexion

*This project has been created as part of the 42 curriculum by \<takawaka\>.*

## Description
Codexion is a multi-threaded simulation project about resource synchronization and timing in C. It simulates coders who alternate between compiling quantum code, debugging, and refactoring while sharing neighboring USB dongles. The program coordinates dongle access and monitors each coder's burnout deadline.

## Instructions

### Compilation
To compile the project with standard flags (`-Wall -Wextra -Werror -pthread`), run the following command at the root directory:
```bash
make
```

### Execution
The program requires exactly 8 positional arguments:
```bash
./codexion <number_of_coders> <time_to_burnout> <time_to_compile> <time_to_debug> <time_to_refactor> <number_of_compiles_required> <dongle_cooldown> <scheduler>
```

*   **Example (FIFO mode):**
    ```bash
    ./codexion 4 410 200 100 100 0 0 fifo
    ```
*   **Example (EDF mode with explicit compile limit):**
    ```bash
    ./codexion 5 800 200 100 100 7 0 edf
    ```

## Resources
- **POSIX threads:** `pthread` manual pages (IEEE Std 1003.1).
- **Operating Systems Concepts:** Silberschatz, Galvin, and Gagne; references on Dining Philosophers and Coffman's conditions.

### AI Usage Disclosure
AI was used to review pthread and timing API usage and to help organize and proofread this README.


## Blocking cases handled

### Deadlock Prevention & Coffman's Conditions
The `take_dongles` function compares the two dongle indices and always requests the smaller index (`low`) before the larger one (`high`). This consistent ordering prevents circular wait during dongle acquisition.

### EDF Priority Policy
Under the `edf` option, each dongle's heap queue prioritizes the coder with the earliest estimated burnout deadline (`last_compile_start + time_to_burnout`). This gives urgent requests priority, but does not guarantee starvation-free scheduling.

### Cooldown Handling
After a coder releases a dongle, it remains unavailable for `dongle_cooldown`. During this wait, `wait_dongle` releases the dongle mutex before sleeping with `usleep`, then locks it again to recheck the queue and cooldown.

### Burnout Monitoring
The monitor checks coder timestamps in a loop and calls `usleep(1000)` between checks. This requests a 1 ms sleep; actual check and log timing can be longer because of processing time and operating-system scheduling.

### Log Serialization
The `print_log` function locks the shared `print_mutex` while writing a log line, so concurrent log messages do not interleave.

## Thread synchronization mechanisms

### Primitive Types Used
*   `pthread_mutex_t`: Utilized to secure individual dongle states, serialize stdout logging operations, and shield shared state metrics.
*   `pthread_cond_t`: Bound to each individual dongle to put requesting threads into an efficient, low-overhead sleep state until the specific resource is released.

### Shared State and Shutdown
*   **Coder metrics:** Coders update `last_compile_start` and `compile_count` while holding `state_mutex`; the monitor reads these values under the same mutex.
*   **Shutdown:** When the monitor detects a burnout or completion, it sets `stop_flag` while holding `state_mutex` and broadcasts on the dongle condition variables to wake waiting threads.


# Codexion


## 概要
Codexionは、C言語のマルチスレッド環境におけるリソース同期とタイミングを扱うシミュレーションプロジェクトです。複数のコーダーが円卓を囲み、量子コードのコンパイル、デバッグ、リファクタリングを行います。コンパイルには隣り合う2つのUSBドングルが必要です。プログラムはドングルの利用順序を調整し、各コーダーのバーンアウト期限を監視します。

## 使用方法

### コンパイル
プロジェクトのルートディレクトリで、以下のコマンドを実行してビルドします。標準コンパイルフラグとして `-Wall -Wextra -Werror -pthread` を使用します。
```bash
make
```

### 実行方法
実行時には、次の8つのコマンドライン引数を指定します。
```bash
./codexion <number_of_coders> <time_to_burnout> <time_to_compile> <time_to_debug> <time_to_refactor> <number_of_compiles_required> <dongle_cooldown> <scheduler>
```

*   **実行例（FIFOモード）:**
    ```bash
    ./codexion 4 410 200 100 100 0 0 fifo
    ```
*   **実行例（コンパイル回数を指定したEDFモード）:**
    ```bash
    ./codexion 5 800 200 100 100 7 0 edf
    ```

## 参照・使用リソース
- **POSIX threads:** `pthread` のマニュアルページ（IEEE Std 1003.1）。
- **Operating Systems Concepts:** Silberschatz、Galvin、Gagne 著。食事する哲学者問題とコフマンの条件の参考にしました。

### AIの使用
pthreadと時間計測関数の使い方の確認、READMEの構成と推敲にAIを利用しました。

## 同期と待機の設計

### ドングル取得順序によるデッドロック防止
`take_dongles` は左右のドングルの番号を比較し、必ず小さい番号（`low`）から大きい番号（`high`）の順に取得します。この順序付けにより、ドングル取得時の循環待ちを防ぎます。

### EDFによる優先順位付け
`edf` オプションでは、各ドングルのヒープキューで推定バーンアウト期限（`last_compile_start + time_to_burnout`）が最も早いコーダーを優先します。緊急度の高い要求を先に扱う仕組みですが、飢餓が起きないことを保証するものではありません。

### 冷却時間の管理
ドングルは解放後、設定された冷却時間（`dongle_cooldown`）が過ぎるまで使用できません。`wait_dongle` は待機中にドングルのミューテックスを解放して `usleep` を呼び、起床後に再取得してキューと冷却時間を確認します。

### バーンアウト監視
`usleep(1000)` を各チェックの間に呼び、モニタースレッドがコーダーの時刻を繰り返し確認します。これは1msのスリープを要求しますが、処理時間やOSのスケジューリングによって実際の検出・出力は遅れる場合があります。

### ログ出力の排他制御
`print_log` はログ行を出力する間、共有ミューテックス `print_mutex` をロックします。これにより、複数スレッドからのログ出力が交錯しないようにしています。

## スレッド同期メカニズム

### 使用したスレッドプリミティブ
*   `pthread_mutex_t`: ドングルの排他制御、ログ出力の保護（`print_mutex`）、コーダーの状態変数の保護（`state_mutex`）に使用します。
*   `pthread_cond_t`: 各ドングルに対応する条件変数です。ドングルが使用可能になるまで、要求スレッドを待機させるために使用します。

### 共有状態と終了処理
*   **コーダーの状態:** `last_compile_start` と `compile_count` の更新と読み取りには、どちらも `state_mutex` を使用します。
*   **終了通知:** モニターはバーンアウト、または必要なコンパイル回数への到達を検知すると、`state_mutex` をロックして `stop_flag` を設定します。続いて、各ドングルの条件変数にブロードキャスト通知を送り、待機中のスレッドを起こします。
