#!/usr/bin/env python3
import sys
from typing import Any

MUST_KEYS: list[str] = [
    "WIDTH", "HEIGHT", "ENTRY", "EXIT", "OUTPUT_FILE", "PERFECT"
]


def check_error(av: list[str]) -> dict[str, Any]:
    if len(av) != 2:
        sys.stderr.write("Usage: python3 a_maze_ing.py <config_file>\n")
        sys.exit(1)

    filepath: str = av[1]
    parsed_data: dict[str, Any] = {}

    try:
        config_dict: dict[str, str] = {}
        seen_keys: set[str] = set()

        with open(filepath, "r", encoding="utf-8") as f:
            for line_num, line in enumerate(f, 1):
                line = line.strip()
                if not line or line.startswith("#"):
                    continue

                if "=" not in line:
                    raise ValueError(f"{line_num}行目の構文エラー: '{line}'")
                key, value = line.split("=", 1)
                key = key.strip()
                value = value.strip()

                if key in seen_keys:
                    raise ValueError(f"重複するキー定義を検出しました: '{key}'")

                seen_keys.add(key)
                config_dict[key] = value

        missing_keys = [k for k in MUST_KEYS if k not in config_dict]
        if missing_keys:
            raise ValueError(f"必須キーが不足しています: {', '.join(missing_keys)}")

        try:
            perfect_raw = config_dict["PERFECT"]
            if perfect_raw in ("TRUE", "True"):
                perfect_val = True
            elif perfect_raw in ("FALSE", "False"):
                perfect_val = False
            else:
                raise ValueError(
                    "PERFECT は True または False である必要があります。"
                )

            seed_raw = config_dict.get("SEED")
            seed_val = int(seed_raw) if seed_raw is not None else 42

            width_val = int(config_dict["WIDTH"])
            height_val = int(config_dict["HEIGHT"])

            entry_parts = config_dict["ENTRY"].split(",")
            if len(entry_parts) != 2:
                raise ValueError("ENTRY は 'x,y' の形式である必要があります。")
            entry_x, entry_y = int(entry_parts[0]), int(entry_parts[1])

            exit_parts = config_dict["EXIT"].split(",")
            if len(exit_parts) != 2:
                raise ValueError("EXIT は 'x,y' の形式である必要があります。")
            exit_x, exit_y = int(exit_parts[0]), int(exit_parts[1])

            # アルゴリズムの指定（オプション）
            algo_val = config_dict.get("ALGORITHM", "Kruskal")

            if width_val <= 0 or height_val <= 0:
                raise ValueError("WIDTH/HEIGHT は正の整数である必要があります。")
            if not (0 <= entry_x < width_val and 0 <= entry_y < height_val):
                raise ValueError("ENTRY の座標が迷路境界外です。")
            if not (0 <= exit_x < width_val and 0 <= exit_y < height_val):
                raise ValueError("EXIT の座標が迷路境界外です。")

            parsed_data["WIDTH"] = width_val
            parsed_data["HEIGHT"] = height_val
            parsed_data["ENTRY"] = (entry_x, entry_y)
            parsed_data["EXIT"] = (exit_x, exit_y)
            parsed_data["OUTPUT_FILE"] = config_dict["OUTPUT_FILE"]
            parsed_data["PERFECT"] = perfect_val
            parsed_data["SEED"] = seed_val
            parsed_data["ALGORITHM"] = algo_val

        except ValueError as num_err:
            raise ValueError(f"値が無効です: {num_err}")

    except FileNotFoundError:
        sys.stderr.write(f"Error: 設定ファイル '{filepath}' が見つかりません。\n")
        sys.exit(1)
    except PermissionError:
        sys.stderr.write(f"Error: 読み込み権限がありません: '{filepath}'\n")
        sys.exit(1)
    except IsADirectoryError:
        sys.stderr.write(f"Error: '{filepath}' はディレクトリです。\n")
        sys.exit(1)
    except Exception as e:
        sys.stderr.write(f"Error: 予期せぬエラーが発生しました: {e}\n")
        sys.exit(1)

    return parsed_data


def write_output(
    grid: list[list[int]], path_str: str, conf: dict[str, Any]
) -> None:
    filepath = conf["OUTPUT_FILE"]
    en_x, en_y = conf["ENTRY"]
    ex_x, ex_y = conf["EXIT"]

    try:
        with open(filepath, "w", encoding="utf-8") as f:
            for row in grid:
                hex_row = "".join(f"{cell:X}" for cell in row)
                f.write(hex_row + "\n")
            f.write("\n")
            f.write(f"{en_x},{en_y}\n")
            f.write(f"{ex_x},{ex_y}\n")
            f.write(f"{path_str}\n")
    except Exception as e:
        sys.stderr.write(f"Error: 出力ファイルへの書き込みに失敗しました: {e}\n")
        sys.exit(1)
