#!/usr/bin/env python3
import math
import sys
import time
from typing import Any
from .generator import MazeGenerator
from .parser import write_output


def draw_maze(
    generator: MazeGenerator,
    grid: list[list[int]],
    path_str: str,
    conf: dict[str, Any],
    show_path: bool,
    color_code: str,
    gaming_offset: float = -1.0,
    player_pos: tuple[int, int] | None = None,
) -> None:
    w, h = conf["WIDTH"], conf["HEIGHT"]
    en_x, en_y = conf["ENTRY"]
    ex_x, ex_y = conf["EXIT"]
    ch = generator.WALL_CHAR

    disp = [[" "] * (2 * w + 1) for _ in range(2 * h + 1)]

    for y in range(2 * h + 1):
        for x in range(2 * w + 1):
            if y % 2 == 0 and x % 2 == 0:
                disp[y][x] = ch

    for y in range(h):
        for x in range(w):
            cell_val = grid[y][x]
            cy, cx = 2 * y + 1, 2 * x + 1

            if cell_val & generator.N:
                disp[cy - 1][cx] = ch
                disp[cy - 1][cx - 1] = ch
                disp[cy - 1][cx + 1] = ch
            if cell_val & generator.E:
                disp[cy][cx + 1] = ch
                disp[cy - 1][cx + 1] = ch
                disp[cy + 1][cx + 1] = ch
            if cell_val & generator.S:
                disp[cy + 1][cx] = ch
                disp[cy + 1][cx - 1] = ch
                disp[cy + 1][cx + 1] = ch
            if cell_val & generator.W:
                disp[cy][cx - 1] = ch
                disp[cy - 1][cx - 1] = ch
                disp[cy + 1][cx - 1] = ch

    for cell_idx, char_42 in generator.closed_cells.items():
        cy, cx = (
            2 * (cell_idx // w) + 1,
            2 * (cell_idx % w) + 1
        )
        disp[cy][cx] = char_42
        disp[cy - 1][cx] = ch
        disp[cy + 1][cx] = ch
        disp[cy][cx - 1] = ch
        disp[cy][cx + 1] = ch

    if show_path and path_str:
        curr_x, curr_y = en_x, en_y
        disp[2 * curr_y + 1][2 * curr_x + 1] = "."
        vectors = {"N": (0, -1), "E": (1, 0), "S": (0, 1), "W": (-1, 0)}
        for char in path_str:
            dx, dy = vectors[char]
            disp[2 * curr_y + 1 + dy][2 * curr_x + 1 + dx] = "."
            curr_x += dx
            curr_y += dy
            disp[2 * curr_y + 1][2 * curr_x + 1] = "."

    disp[2 * en_y + 1][2 * en_x + 1] = "S"
    disp[2 * ex_y + 1][2 * ex_x + 1] = "G"

    if player_pos is not None:
        px, py = player_pos
        disp[2 * py + 1][2 * px + 1] = "@"

    color_reset = "\033[0m"
    green_start = "\033[32m"
    red_exit = "\033[31m"
    cyan_path = "\033[36m"
    yellow_42 = "\033[33;1m"
    bright_white = "\033[97;1m"

    title_line = (
        f"=== A-Maze-ing Visualization (Seed: {generator.seed}, "
        f"Algo: {generator.algorithm}) ===\n"
    )
    out_buf = ["\033[H\033[J", title_line]

    if generator.warning_msg:
        out_buf.append(f"\033[31m{generator.warning_msg}\033[0m\n")

    for y in range(2 * h + 1):
        row_str = []
        for x in range(2 * w + 1):
            char = disp[y][x]
            if char == ch:
                if gaming_offset >= 0:
                    r = int(
                        math.sin(0.3 * x + 0.3 * y + gaming_offset)
                        * 127 + 128
                    )
                    g = int(
                        math.sin(0.3 * x + 0.3 * y + gaming_offset + 2)
                        * 127 + 128
                    )
                    b = int(
                        math.sin(0.3 * x + 0.3 * y + gaming_offset + 4)
                        * 127 + 128
                    )
                    row_str.append(f"\033[38;2;{r};{g};{b}m{ch}\033[0m")
                else:
                    row_str.append(f"{color_code}{ch}\033[0m")
            elif char in ("4", "2"):
                row_str.append(f"{yellow_42}{char}\033[0m")
            elif char == "S":
                row_str.append(f"{green_start}S\033[0m")
            elif char == "G":
                row_str.append(f"{red_exit}G\033[0m")
            elif char == ".":
                row_str.append(f"{cyan_path}.\033[0m")
            elif char == "@":
                row_str.append(f"{bright_white}@\033[0m")
            else:
                row_str.append(" ")
        out_buf.append("".join(row_str) + "\n")

    out_buf.append(color_reset)
    sys.stdout.write("".join(out_buf))
    sys.stdout.flush()


def run_animation(
    generator: MazeGenerator,
    conf: dict[str, Any],
    color_code: str
) -> list[list[int]]:
    print("\033[?25l", end="")
    last_grid = generator.grid
    try:
        steps = generator.generate_steps(perfect=conf["PERFECT"])
        for intermediate_grid in steps:
            draw_maze(
                generator,
                intermediate_grid,
                "",
                conf,
                show_path=False,
                color_code=color_code
            )
            time.sleep(0.04)
            last_grid = intermediate_grid
    except KeyboardInterrupt:
        pass
    finally:
        print("\033[?25h", end="")
    return last_grid


def play_game(
    generator: MazeGenerator,
    grid: list[list[int]],
    conf: dict[str, Any],
    color_code: str
) -> None:
    px, py = conf["ENTRY"]
    gx, gy = conf["EXIT"]
    moves_history = 0
    solution_path = generator.solve(conf["ENTRY"], conf["EXIT"])
    optimal_len = len(solution_path)

    while True:
        draw_maze(
            generator,
            grid,
            "",
            conf,
            show_path=False,
            color_code=color_code,
            player_pos=(px, py)
        )
        print(
            f"迷路脱出ゲーム"
            f"[現在の歩数: {moves_history} 歩 | "
            f"最短理想歩数: {optimal_len}]"
        )
        print("移動: [W]:北  [A]:西  [S]:南  [D]:東  (Qで戻る)")

        try:
            cmd = input("あなたのコマンド? (W/A/S/D/Q): ").strip().upper()
        except (KeyboardInterrupt, EOFError):
            break

        if cmd == "Q":
            break

        if not cmd:
            continue

        for char in cmd:
            if px == gx and py == gy:
                break

            current_walls = grid[py][px]
            if char == "W" and not (current_walls & generator.N):
                py -= 1
                moves_history += 1
            elif char == "S" and not (current_walls & generator.S):
                py += 1
                moves_history += 1
            elif char == "D" and not (current_walls & generator.E):
                px += 1
                moves_history += 1
            elif char == "A" and not (current_walls & generator.W):
                px -= 1
                moves_history += 1

        if px == gx and py == gy:
            draw_maze(
                generator,
                grid,
                "",
                conf,
                show_path=False,
                color_code=color_code,
                player_pos=(px, py)
            )
            print("\nゴール！！")
            print(
                f"あなたの総歩数: {moves_history} 歩 "
                f"(最短経路: {optimal_len} 歩)"
            )
            if moves_history == optimal_len:
                print("評価: 神")
            elif moves_history <= optimal_len + 5:
                print("評価: 素晴らしい")
            else:
                print("評価: 頑張りましょう")
            input("\nEnterキーを押してメニューに戻ります...")
            break


def interactive_menu(
    generator: MazeGenerator, grid: list[list[int]], conf: dict[str, Any]
) -> None:
    show_path = False
    colors = [
        "\033[34m", "\033[35m", "\033[36m", "\033[31m", "\033[32m", "\033[37m"
    ]
    color_idx = 0
    current_grid = grid

    themes = ["#", "█", "▓", "░", "*", "+"]
    theme_idx = 0
    generator.WALL_CHAR = themes[theme_idx]

    solution_path = generator.solve(conf["ENTRY"], conf["EXIT"])

    while True:
        draw_maze(
            generator,
            current_grid,
            solution_path,
            conf,
            show_path,
            colors[color_idx]
        )

        print("A-Maze-ing Extended Menu (Bonuses Active!):")
        print("1. Re-generate a new maze (迷路を新しく再生成)")
        print("2. Show/Hide solution path (最短ルートの表示/非表示)")
        print("3. Rotate wall colors (壁の色を変更)")
        print("4. Quit program (プログラムを終了)")
        print("5. RGB Rainbow wave (ゲーミングモード起動)")
        print("6. Switch Algorithm (Kruskal <-> DFS)")
        print("7. Watch construction animation (生成アニメ表示)")
        print("8. Play Interactive Mode (WASDで遊ぶゲーム)")
        print("9. Switch Wall Theme (壁のテーマを変更)")

        try:
            choice = input("Choice? (1-9): ").strip()
        except (KeyboardInterrupt, EOFError):
            print("\nExiting program.")
            break

        if choice == "1":
            current_grid = generator.generate(perfect=conf["PERFECT"])
            solution_path = generator.solve(conf["ENTRY"], conf["EXIT"])
            write_output(current_grid, solution_path, conf)

        elif choice == "2":
            show_path = not show_path

        elif choice == "3":
            color_idx = (color_idx + 1) % len(colors)

        elif choice == "4" or choice.lower() in ("q", "quit", "exit"):
            print("Goodbye!")
            break

        elif choice == "5":
            print("\033[?25l", end="")
            try:
                for i in range(60):
                    draw_maze(
                        generator,
                        current_grid,
                        solution_path,
                        conf,
                        show_path,
                        "",
                        gaming_offset=i * 0.4,
                    )
                    time.sleep(0.05)
            except KeyboardInterrupt:
                pass
            finally:
                print("\033[?25h", end="")

        elif choice == "6":
            if generator.algorithm == "Kruskal":
                generator.algorithm = "DFS"
            else:
                generator.algorithm = "Kruskal"
            current_grid = generator.generate(perfect=conf["PERFECT"])
            solution_path = generator.solve(conf["ENTRY"], conf["EXIT"])
            write_output(current_grid, solution_path, conf)

        elif choice == "7":
            current_grid = run_animation(generator, conf, colors[color_idx])
            solution_path = generator.solve(conf["ENTRY"], conf["EXIT"])
            write_output(current_grid, solution_path, conf)

        elif choice == "8":
            play_game(generator, current_grid, conf, colors[color_idx])

        elif choice == "9":
            theme_idx = (theme_idx + 1) % len(themes)
            generator.WALL_CHAR = themes[theme_idx]
