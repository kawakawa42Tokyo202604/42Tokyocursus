#!/usr/bin/env python3
import random


class UnionFind:
    def __init__(self, size: int) -> None:
        self.parent: list[int] = list(range(size))

    def find(self, i: int) -> int:
        if self.parent[i] == i:
            return i
        self.parent[i] = self.find(self.parent[i])
        return self.parent[i]

    def union(self, i: int, j: int) -> bool:
        root_i = self.find(i)
        root_j = self.find(j)
        if root_i != root_j:
            self.parent[root_i] = root_j
            return True
        return False


class MazeGenerator:

    N: int = 1
    E: int = 2
    S: int = 4
    W: int = 8

    def __init__(self, width: int, height: int, seed: int = 42) -> None:
        self.width: int = width
        self.height: int = height
        self.seed: int = seed
        self.grid: list[list[int]] = [[15] * width for _ in range(height)]
        self.closed_cells: set[int] = set()

    def embed_42(self) -> bool:
        grid_42 = [
            [1, 0, 1, 0, 1, 1, 1],
            [1, 0, 1, 0, 0, 0, 1],
            [1, 1, 1, 0, 1, 1, 1],
            [0, 0, 1, 0, 1, 0, 0],
            [0, 0, 1, 0, 1, 1, 1],
        ]
        p_h = len(grid_42)
        p_w = len(grid_42[0])

        if self.width < p_w or self.height < p_h:
            return False

        start_x = (self.width - p_w) // 2
        start_y = (self.height - p_h) // 2

        for py in range(p_h):
            for px in range(p_w):
                if grid_42[py][px] == 1:
                    gx = start_x + px
                    gy = start_y + py
                    self.grid[gy][gx] = 15
                    self.closed_cells.add(gy * self.width + gx)
        return True

    def generate(self, perfect: bool = True) -> list[list[int]]:
        random.seed(self.seed)
        self.embed_42()

        walls = []
        for y in range(self.height):
            for x in range(self.width):
                current1_idx = y * self.width + x
                if current1_idx in self.closed_cells:
                    continue
                if x + 1 < self.width and (
                    (y * self.width + (x + 1)) not in self.closed_cells
                ):
                    walls.append(
                        (
                            current1_idx,
                            y * self.width + (x + 1),
                            self.E,
                            self.W,
                            (x, y),
                            (x + 1, y),
                        )
                    )
                if y + 1 < self.height and (
                    ((y + 1) * self.width + x) not in self.closed_cells
                ):
                    walls.append(
                        (
                            current1_idx,
                            (y + 1) * self.width + x,
                            self.S,
                            self.N,
                            (x, y),
                            (x, y + 1),
                        )
                    )

        random.shuffle(walls)
        uf = UnionFind(self.width * self.height)

        remaining_walls = []

        for current_1, current_2, d1, d2, (x1, y1), (x2, y2) in walls:
            if uf.union(current_1, current_2):
                self.grid[y1][x1] -= d1
                self.grid[y2][x2] -= d2
            else:
                remaining_walls.append((d1, d2, (x1, y1), (x2, y2)))

        if not perfect and remaining_walls:
            num_to_break = max(1, len(remaining_walls) // 10)
            for d1, d2, (x1, y1), (x2, y2) in random.sample(
                remaining_walls, num_to_break
            ):
                if self.grid[y1][x1] & d1:
                    self.grid[y1][x1] -= d1
                if self.grid[y2][x2] & d2:
                    self.grid[y2][x2] -= d2

        return self.grid

    def solve(self, start: tuple[int, int], end: tuple[int, int]) -> str:
        start_x, start_y = start
        tmp: list[tuple[int, int, str]] = [(start_x, start_y, "")]
        visited: set[tuple[int, int]] = {(start_x, start_y)}

        vecters: dict[int, tuple[int, int, str]] = {
            self.N: (0, -1, "N"),
            self.E: (1, 0, "E"),
            self.S: (0, 1, "S"),
            self.W: (-1, 0, "W"),
        }

        while tmp:
            current_x, current_y, ans = tmp.pop(0)

            if (current_x, current_y) == end:
                return ans

            current_walls = self.grid[current_y][current_x]
            for wall_bit, (vecter_x, vecter_y, char) in vecters.items():
                if not (current_walls & wall_bit):
                    new_x, new_y = current_x + vecter_x, current_y + vecter_y
                    if (
                        0 <= new_x < self.width
                        and 0 <= new_y < self.height
                        and (new_x, new_y) not in visited
                    ):
                        visited.add((new_x, new_y))
                        tmp.append((new_x, new_y, ans + char))

        return ""
