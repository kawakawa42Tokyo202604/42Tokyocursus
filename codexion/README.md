# Codexion

*This project has been created as part of the 42 curriculum by \<takawaka\>.*

## Description
Codexion is a multi-threaded simulation project designed to tackle resource synchronization and timing challenges in C[cite: 10]. The program simulates multiple coders sitting around a table, alternating between compiling quantum code, debugging, and refactoring[cite: 10]. Since compiling requires acquiring two neighboring USB dongles simultaneously out of a limited shared pool, the system must carefully orchestrate access to prevent deadlocks and data races, all while avoiding coder burnout before the designated deadlines[cite: 10].

## Instructions

### Compilation
To compile the project with standard flags (`-Wall -Wextra -Werror -pthread`), run the following command at the root directory[cite: 6, 10]:
```bash
make
```

### Execution
The program requires exactly 8 positional arguments[cite: 10]:
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
*   **POSIX Threads Documentation:** `pthread` manual pages (IEEE Std 1003.1).
*   **Operating Systems Concepts:** Silberschatz, Galvin, and Gagne (References on Dining Philosophers and Coffman's Conditions).
*   **AI Usage Disclosure:**
*   AI was consulted periodically for guidance on the proper usage and implementation of multi-threading and timing functions, specifically `pthread_create`, `pthread_join`, `pthread_mutex_init`, `pthread_mutex_lock`, `pthread_mutex_unlock`, `pthread_mutex_destroy`, `pthread_cond_init`, `pthread_cond_wait`, `pthread_cond_timedwait`, `pthread_cond_signal`, `pthread_cond_broadcast`, `pthread_cond_destroy`, `gettimeofday`, and `usleep`.


## Blocking cases handled

### Deadlock Prevention & Coffman's Conditions
To eliminate deadlocks, we broke the **Circular Wait** condition—one of Coffman's four mandatory conditions for a deadlock. Instead of coders blindly grabbing their left or right dongle first, our `take_dongles` logic dynamically checks the resource indices and forces every coder to always acquire the smaller numerical index (`low`) before requesting the larger index (`high`)[cite: 8]. This absolute resource ordering prevents any potential circular dependency loop across the thread network.

### Starvation Prevention
Under the `edf` scheduler option, starvation is mitigated by implementing a deterministic **Earliest Deadline First** sorting scheme[cite: 10]. Coders facing an imminent burnout deadline (calculated as `last_compile_start + time_to_burnout`) are assigned the highest priority keys within the dongle's binary heap queue[cite: 8, 10]. This ensures that dying threads jump straight to the head of the resource queue, avoiding starvation[cite: 8, 10].

### Cooldown Handling
After a coder releases a dongle, the resource must remain locked out for a specific duration (`dongle_cooldown`)[cite: 10]. To implement this without stalling the entire simulation, our `wait_dongle` routine drops the dongle's mutex lock while sleeping via `usleep` during the cooldown window[cite: 8]. This non-blocking design allows other threads to queue up behind the cooling resource without freezing the execution flow.

### Precise Burnout Detection
The Subject mandates that a coder's burnout log must be printed within 10 ms of the actual expiration[cite: 10]. A dedicated monitor thread runs an optimized observation loop with a micro-sleep interval of 1 ms (`usleep(1000)`), checking every coder's timestamp continuously to achieve hyper-precise termination logs well within the 10 ms window[cite: 7, 10].

### Log Serialization
To guarantee that state transitions are never interleaved or garbled on the stdout stream, a globally shared `print_mutex` is locked before any `printf` statement executes and is released immediately after, satisfying the serialization requirement[cite: 8, 10].

## Thread synchronization mechanisms

### Primitive Types Used
*   `pthread_mutex_t`: Utilized to secure individual dongle states, serialize stdout logging operations, and shield shared state metrics[cite: 8, 10].
*   `pthread_cond_t`: Bound to each individual dongle to put requesting threads into an efficient, low-overhead sleep state until the specific resource is released[cite: 8, 10].

### Coordination & Thread-Safe Communication
*   **Coders to Monitor:** Coders update their local metrics (`last_compile_start` and `compile_count`) inside a critical section protected by `state_mutex`[cite: 7]. The monitor thread locks this exact mutex to read the current values, ensuring a race-free data exchange[cite: 7].
*   **Monitor to Coders:** When the monitor thread detects a burnout or detects that the compile thresholds are met, it safely changes the global `stop_flag` under the protection of `state_mutex`[cite: 7]. It then calls `pthread_cond_broadcast` across all dongle condition variables to awake any sleeping worker threads, ensuring a clean, leak-free unwinding and joining of all active sub-threads[cite: 7, 8].


# Codexion


## 概要
Codexionは、C言語におけるマルチスレッド環境でのリソース同期とタイミングの課題（食事する哲学者問題の発展形）を解決するために設計されたシミュレーションプロジェクトです[cite: 10]。このプログラムは、円卓を囲んで座り、量子コードのコンパイル、デバッグ、リファクタリングを交互に行う複数のコーダーをシミュレートします[cite: 10]。コンパイルを実行するには、制限のある共有プールから隣り合う2つのUSBドングルを両手に同時に確保する必要があるため、デッドロックやデータレースを防ぐための慎重な排他制御が求められます[cite: 10]。これらすべてを、指定された制限時間内にコーダーがバーンアウト（燃え尽き）するのを避けながら処理します[cite: 10]。

## 使用方法

### コンパイル
標準コンパイルフラグ（`-Wall -Wextra -Werror -pthread`）を使用してプロジェクトをビルドするには、ルートディレクトリで以下のコマンドを実行します
```bash
make
```

### 実行方法
プログラムは、正確に8個の位置引数を必要とします
```bash
./codexion <number_of_coders> <time_to_burnout> <time_to_compile> <time_to_debug> <time_to_refactor> <number_of_compiles_required> <dongle_cooldown> <scheduler>
```

*   **実行例 (FIFOモード):**
    ```bash
    ./codexion 4 410 200 100 100 0 0 fifo
    ```
*   **実行例 (EDFモード・コンパイル回数制限あり):**
    ```bash
    ./codexion 5 800 200 100 100 7 0 edf
    ```

## 参照・使用リソース
*   **POSIX Threads Documentation:** `pthread` manual pages (IEEE Std 1003.1).
*   **Operating Systems Concepts:** Silberschatz, Galvin, and Gagne (食事する哲学者問題およびコフマンの条件に関するリファレンス).
*   **AIの使用に関する開示:**
pthread_create, pthread_join, pthread_mutex_init,
pthread_mutex_lock,
pthread_mutex_unlock, pthread_mutex_destroy,
pthread_cond_init,
pthread_cond_wait, pthread_cond_timedwait,
pthread_cond_signal,
pthread_cond_broadcast, pthread_cond_destroy,
gettimeofday, usleep
の使用方法について適時教えてもらいました。

## 解決した競合・ブロッキング問題

### デッドロック防止とコフマンの条件の破壊
デッドロックの発生を完全に排除するため、コフマンの4条件のうち「循環待ち（Circular Wait）」を論理的に破壊しました。コーダーが左右のドングルを無秩序に奪い合うのを防ぐため、`take_dongles` ロジックでは取得対象となる左右のドングルのインデックスを動的に比較し、**常に番号が小さいドングル（`low`）を確保してから、番号が大きいドングル（`high`）を確保しにいくように強制しています**[cite: 8]。この一貫したリソース順序付けにより、スレッド間で循環的な依存が発生するのを100%防止しています[cite: 8]。

### 飢餓（スターベーション）の防止
`edf` スケジューラオプションでは、決定論的な **Earliest Deadline First** ソートアルゴリズムを自作ヒープを用いて実装することで飢餓を防止しています[cite: 2, 10]。次にバーンアウトするまでの残り時間が短い（＝デッドライン `last_compile_start + time_to_burnout` が最も近い）コーダーに対して、ドングルのバイナリヒープキュー内で最も高い優先度（最小のキー値）を割り当てます[cite: 2, 8, 10]。これにより、生命の危機に瀕しているスレッドが常にリソース待ち行列の最優先位置に割り込むため、特定のコーダーがドングルを確保できずに死亡するのを防ぎます[cite: 2, 8, 10]。

### 冷却時間（コールドダウン）の処理
ドングルが解放された後、指定されたミリ秒数（`dongle_cooldown`）は他のコーダーがそのドングルを使用できないように制限する必要があります[cite: 10]。しかし、冷却待機中のスレッドがドングルのMutexロックを保持したまま `usleep` してしまうと、列の後ろに並びたい他のスレッドの邪魔をしてしまい、シミュレーション全体がデッドロックのようにフリーズします。これに対処するため、`wait_dongle` 関数では、冷却時間の待機に入る直前に**ドングルのMutexロックを一度一時的に解放し、`usleep` 明けに再ロックする**非ブロッキング設計を採用しています[cite: 8]。

### 高精度なバーンアウト検出
仕様書では、コーダーの死亡（バーンアウト）ログを、実際の死亡時刻から最大10ms以内に出力することを求めています[cite: 10]。これを実現するため、本システムでは完全に独立した監視員スレッド（モニター）を1つ起動し[cite: 7, 10]、1ms周期（`usleep(1000)`）の超高速チェックループを回すことで[cite: 7]、ミリ秒単位での正確な死亡ログの出力を可能にしています[cite: 7, 10]。

### ログのシリアライズ化
シミュレーション中の状態遷移ログが標準出力（stdout）上で混ざり合ったり、行の途中に割り込まれたりするのを防ぐため、すべての `printf` 出力処理の直前に共有ミューテックス `print_mutex` をロックし、出力が完了した直後にアンロックすることで、ログの完全なシリアライズ（一列化）を保証しています[cite: 8, 10]。

## スレッド同期メカニズム

### 使用したスレッドプリミティブ
*   `pthread_mutex_t`: 各ドングルの排他制御[cite: 8, 10]、ログ出力時の競合防止（`print_mutex`）[cite: 8, 10]、および生存時間などの状態変数の保護（`state_mutex`）に使用します[cite: 7, 10]。
*   `pthread_cond_t`: 各ドングルに紐付けられ、リソースが空く（または冷却時間が明ける）まで、要求中のスレッドをCPU負荷をかけずに効率的に休眠状態（ブロック）させるために使用します[cite: 8, 10]。

### スレッド間の調整と安全な通信
*   **コーダーからモニターへの情報共有:** コーダーが生存タイマー（`last_compile_start`）やコンパイル回数（`compile_count`）を書き換える際は、モニター側の読み取りと競合してデータレースを起こさないよう、必ず `state_mutex` のクリティカルセクション内で安全に値を更新します[cite: 7]。
*   **モニターからコーダーへの終了通知:** モニターが死亡またはコンパイル規定回数の達成を検知すると、`state_mutex` の下で安全にグローバルな `stop_flag` を `1` に書き換えます[cite: 7]。その後、全ドングルの条件変数に `pthread_cond_broadcast` を送り[cite: 8]、休眠中のすべてのコーダースレッドを強制的に起こすことで、デッドロックを起こすことなく全員を安全に `pthread_join`（スレッドの回収・終了）へ導きます[cite: 7, 8]。
