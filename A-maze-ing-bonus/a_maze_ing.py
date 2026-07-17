#!/usr/bin/env python3
import sys
from src.mazegen import (
    MazeGenerator,
    check_error,
    write_output,
    interactive_menu,
)


def main() -> None:
    conf = check_error(sys.argv)
    generator = MazeGenerator(conf["WIDTH"], conf["HEIGHT"], conf["SEED"])

    if "ALGORITHM" in conf:
        generator.algorithm = conf["ALGORITHM"]

    maze = generator.generate(perfect=conf["PERFECT"])


    if conf["ENTRY"] == conf["EXIT"]:
        sys.stderr.write("Error: ENTRY and EXIT cannot be the same.\n")
        sys.exit(1)

    ans = generator.solve(conf["ENTRY"], conf["EXIT"])

    write_output(maze, ans, conf)

    if not ans:
        sys.stderr.write(
            "Error: ENTRY or EXIT cannot be inside the 42-bounds.\n"
        )
        sys.exit(1)

    interactive_menu(generator, maze, conf)


if __name__ == "__main__":
    main()
