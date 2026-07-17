#!/usr/bin/env python3
import random
import sys
from typing import Generator


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
    WALL_CHAR: str = "#"

    def __init__(self, width: int, height: int, seed: int = 42) -> None:
        self.width: int = width
        self.height: int = height
        self.seed: int = seed
        self.algorithm: str = "Kruskal"

        random.seed(self.seed)

        self.grid: list[list[int]] = [[15] * width for _ in range(height)]
        self.closed_cells: dict[int, str] = {}
        self.warning_msg: str = ""

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
                    char_type = "4" if px < 3 else "2"
                    self.closed_cells[gy * self.width + gx] = char_type
        return True

    def generate(self, perfect: bool = True) -> list[list[int]]:
        if self.algorithm == "DFS":
            return self.generate_dfs(perfect)
        return self.generate_kruskal(perfect)

    def generate_kruskal(self, perfect: bool = True) -> list[list[int]]:
        self.grid = [[15] * self.width for _ in range(self.height)]
        self.closed_cells = {}
        self.warning_msg = ""

        if not self.embed_42():
            self.warning_msg = "Error : Can't draw 42"
            sys.stderr.write(self.warning_msg + "\n")

        walls = []
        for y in range(self.height):
            for x in range(self.width):
                c1_idx = y * self.width + x
                if c1_idx in self.closed_cells:
                    continue

                if x + 1 < self.width:
                    c2_idx_x = y * self.width + (x + 1)
                    if c2_idx_x not in self.closed_cells:
                        walls.append(
                            (c1_idx, c2_idx_x, self.E, self.W,
                             (x, y), (x + 1, y))
                        )

                if y + 1 < self.height:
                    c2_idx_y = (y + 1) * self.width + x
                    if c2_idx_y not in self.closed_cells:
                        walls.append(
                            (c1_idx, c2_idx_y, self.S, self.N,
                             (x, y), (x, y + 1))
                        )

        random.shuffle(walls)
        uf = UnionFind(self.width * self.height)
        remaining_walls = []

        for c1, c2, d1, d2, (x1, y1), (x2, y2) in walls:
            if uf.union(c1, c2):
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

    def generate_dfs(self, perfect: bool = True) -> list[list[int]]:
        self.grid = [[15] * self.width for _ in range(self.height)]
        self.closed_cells = {}
        self.warning_msg = ""

        if not self.embed_42():
            self.warning_msg = "Error : Can't draw 42"
            sys.stderr.write(self.warning_msg + "\n")

        visited = set(self.closed_cells)

        start_x, start_y = 0, 0
        found = False
        for y in range(self.height):
            for x in range(self.width):
                if (y * self.width + x) not in self.closed_cells:
                    start_x, start_y = x, y
                    found = True
                    break
            if found:
                break

        stack = [(start_x, start_y)]
        visited.add(start_y * self.width + start_x)

        directions = {
            "N": (0, -1, self.N, self.S),
            "E": (1, 0, self.E, self.W),
            "S": (0, 1, self.S, self.N),
            "W": (-1, 0, self.W, self.E)
        }

        while stack:
            cx, cy = stack[-1]
            neighbors = []

            for d, (dx, dy, b1, b2) in directions.items():
                nx, ny = cx + dx, cy + dy
                if 0 <= nx < self.width and 0 <= ny < self.height:
                    n_idx = ny * self.width + nx
                    if n_idx not in visited:
                        neighbors.append((nx, ny, b1, b2))

            if neighbors:
                nx, ny, b1, b2 = random.choice(neighbors)
                self.grid[cy][cx] -= b1
                self.grid[ny][nx] -= b2
                visited.add(ny * self.width + nx)
                stack.append((nx, ny))
            else:
                stack.pop()

        if not perfect:
            remaining = []
            for y in range(self.height):
                for x in range(self.width):
                    idx = y * self.width + x
                    if idx in self.closed_cells:
                        continue
                    if x + 1 < self.width:
                        idx2 = y * self.width + (x + 1)
                        is_closed = idx2 in self.closed_cells
                        if not is_closed and (self.grid[y][x] & self.E):
                            remaining.append(
                                ((x, y), (x + 1, y), self.E, self.W)
                            )
                    if y + 1 < self.height:
                        idx2 = (y + 1) * self.width + x
                        is_closed = idx2 in self.closed_cells
                        if not is_closed and (self.grid[y][x] & self.S):
                            remaining.append(
                                ((x, y), (x, y + 1), self.S, self.N)
                            )
            if remaining:
                num_to_break = max(1, len(remaining) // 10)
                for (x1, y1), (x2, y2), d1, d2 in random.sample(
                    remaining, num_to_break
                ):
                    if self.grid[y1][x1] & d1:
                        self.grid[y1][x1] -= d1
                    if self.grid[y2][x2] & d2:
                        self.grid[y2][x2] -= d2

        return self.grid

    def generate_steps(
        self, perfect: bool = True
    ) -> Generator[list[list[int]], None, None]:
        self.grid = [[15] * self.width for _ in range(self.height)]
        self.closed_cells = {}
        self.warning_msg = ""

        if not self.embed_42():
            self.warning_msg = "Error : Can't draw 42"

        yield [row[:] for row in self.grid]

        if self.algorithm == "DFS":
            visited = set(self.closed_cells)
            start_x, start_y = 0, 0
            found = False
            for y in range(self.height):
                for x in range(self.width):
                    if (y * self.width + x) not in self.closed_cells:
                        start_x, start_y = x, y
                        found = True
                        break
                if found:
                    break

            stack = [(start_x, start_y)]
            visited.add(start_y * self.width + start_x)
            directions = {
                "N": (0, -1, self.N, self.S),
                "E": (1, 0, self.E, self.W),
                "S": (0, 1, self.S, self.N),
                "W": (-1, 0, self.W, self.E)
            }

            step_counter = 0
            while stack:
                cx, cy = stack[-1]
                neighbors = []

                for d, (dx, dy, b1, b2) in directions.items():
                    nx, ny = cx + dx, cy + dy
                    if 0 <= nx < self.width and 0 <= ny < self.height:
                        n_idx = ny * self.width + nx
                        if n_idx not in visited:
                            neighbors.append((nx, ny, b1, b2))

                if neighbors:
                    nx, ny, b1, b2 = random.choice(neighbors)
                    self.grid[cy][cx] -= b1
                    self.grid[ny][nx] -= b2
                    visited.add(ny * self.width + nx)
                    stack.append((nx, ny))
                else:
                    stack.pop()

                step_counter += 1
                if step_counter % 2 == 0:
                    yield [row[:] for row in self.grid]
        else:
            walls = []
            for y in range(self.height):
                for x in range(self.width):
                    c1_idx = y * self.width + x
                    if c1_idx in self.closed_cells:
                        continue

                    if x + 1 < self.width:
                        c2_idx_x = y * self.width + (x + 1)
                        if c2_idx_x not in self.closed_cells:
                            walls.append(
                                (c1_idx, c2_idx_x, self.E, self.W,
                                 (x, y), (x + 1, y))
                            )
                    if y + 1 < self.height:
                        c2_idx_y = (y + 1) * self.width + x
                        if c2_idx_y not in self.closed_cells:
                            walls.append(
                                (c1_idx, c2_idx_y, self.S, self.N,
                                 (x, y), (x, y + 1))
                            )

            random.shuffle(walls)
            uf = UnionFind(self.width * self.height)
            step_counter = 0

            for c1, c2, d1, d2, (x1, y1), (x2, y2) in walls:
                if uf.union(c1, c2):
                    self.grid[y1][x1] -= d1
                    self.grid[y2][x2] -= d2
                    step_counter += 1
                    if step_counter % 2 == 0:
                        yield [row[:] for row in self.grid]

        yield [row[:] for row in self.grid]

    def solve(self, start: tuple[int, int], end: tuple[int, int]) -> str:
        start_x, start_y = start
        tmp: list[tuple[int, int, str]] = [(start_x, start_y, "")]
        visited: set[tuple[int, int]] = {(start_x, start_y)}

        vects: dict[int, tuple[int, int, str]] = {
            self.N: (0, -1, "N"),
            self.E: (1, 0, "E"),
            self.S: (0, 1, "S"),
            self.W: (-1, 0, "W"),
        }

        while tmp:
            cx, cy, ans = tmp.pop(0)

            if (cx, cy) == end:
                return ans

            current_walls = self.grid[cy][cx]

            for wall_bit, (vx, vy, char) in vects.items():
                if not (current_walls & wall_bit):
                    nx, ny = cx + vx, cy + vy
                    if (
                        0 <= nx < self.width
                        and 0 <= ny < self.height
                        and (nx, ny) not in visited
                    ):
                        visited.add((nx, ny))
                        tmp.append((nx, ny, ans + char))

        return ""
