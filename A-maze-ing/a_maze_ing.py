#!/usr/bin/env python3
import sys
from typing import Any
from src.mazegen.generator import MazeGenerator

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
                    raise ValueError(
                        f"Bad syntax at line {line_num}: '{line}'"
                    )
                key, value = line.split("=", 1)
                key = key.strip()
                value = value.strip()

                if key in seen_keys:
                    raise ValueError(f"Duplicate key found: '{key}'")

                seen_keys.add(key)
                config_dict[key] = value

        missing_keys: list[str] = []
        for k in MUST_KEYS:
            if k not in config_dict:
                missing_keys.append(k)

        if missing_keys:
            joined_keys: str = ", ".join(missing_keys)
            raise ValueError(f"Missing mandatory key(s): {joined_keys}")

        try:
            perfect_val: bool
            perfect_raw: str = config_dict["PERFECT"]
            if perfect_raw == "TRUE" or perfect_raw == "True":
                perfect_val = True
            elif perfect_raw == "FALSE" or perfect_raw == "False":
                perfect_val = False
            else:
                raise ValueError(
                    "PERFECT must be 'TRUE' or 'True', 'FALSE', or 'False'."
                )

            seed_raw: str | None = config_dict.get("SEED")
            seed_val: int = 42
            if seed_raw is not None:
                seed_val = int(seed_raw)

            width_val: int = int(config_dict["WIDTH"])
            height_val: int = int(config_dict["HEIGHT"])

            entry_parts: list[str] = config_dict["ENTRY"].split(",")
            if len(entry_parts) != 2:
                raise ValueError("ENTRY must be 'x,y' format.")
            entry_x: int = int(entry_parts[0])
            entry_y: int = int(entry_parts[1])

            exit_parts: list[str] = config_dict["EXIT"].split(",")
            if len(exit_parts) != 2:
                raise ValueError("EXIT must be 'x,y' format.")
            exit_x: int = int(exit_parts[0])
            exit_y: int = int(exit_parts[1])

            if width_val <= 0 or height_val <= 0:
                raise ValueError("WIDTH and HEIGHT must be positive integers.")
            if not (0 <= entry_x < width_val and 0 <= entry_y < height_val):
                raise ValueError("ENTRY coordinates are out of maze bounds.")
            if not (0 <= exit_x < width_val and 0 <= exit_y < height_val):
                raise ValueError("EXIT coordinates are out of maze bounds.")

            parsed_data["WIDTH"] = width_val
            parsed_data["HEIGHT"] = height_val
            parsed_data["ENTRY"] = (entry_x, entry_y)
            parsed_data["EXIT"] = (exit_x, exit_y)
            parsed_data["OUTPUT_FILE"] = config_dict["OUTPUT_FILE"]
            parsed_data["PERFECT"] = perfect_val
            parsed_data["SEED"] = seed_val

        except ValueError as num_err:
            raise ValueError(f"Invalid configuration value. {num_err}")

    except FileNotFoundError:
        sys.stderr.write(
            f"Error: Configuration file '{filepath}' not found.\n"
        )
        sys.exit(1)
    except PermissionError:
        sys.stderr.write(
            f"Error: Permission denied. Cannot read '{filepath}'.\n"
        )
        sys.exit(1)
    except IsADirectoryError:
        sys.stderr.write(
            f"Error: '{filepath}' is a directory, not a file.\n"
        )
        sys.exit(1)
    except Exception as e:
        sys.stderr.write(
            "Error: An unexpected error occurred while reading config: "
            f"{e}\n"
        )
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
        sys.stderr.write(f"Error: Failed to write to output file: {e}\n")
        sys.exit(1)


def main() -> None:
    conf: dict[str, Any] = check_error(sys.argv)
    generator = MazeGenerator(conf["WIDTH"], conf["HEIGHT"], conf["SEED"])
    maze = generator.generate(perfect=conf["PERFECT"])
    ans = generator.solve(conf["ENTRY"], conf["EXIT"])
    write_output(maze, ans, conf)


if __name__ == "__main__":
    main()
