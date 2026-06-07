#!/usr/bin/env python3
import random

ALL_ACHIEVEMENTS = [
    "Crafting Genius", "World Savior", "Master Explorer", "Collector Supreme",
    "Untouchable", "Boss Slayer", "Strategist", "Unstoppable", "Speed Runner",
    "Survivor", "Treasure Hunter", "First Steps", "Sharp Mind",
    "Hidden Path Finder"
]


def gen_player_achievements() -> set[str]:
    achievements = random.randint(4, 9)
    chosen = random.sample(ALL_ACHIEVEMENTS, achievements)
    return set(chosen)


def main() -> None:
    print("=== Achievement Tracker System ===")
    print()
    players_list = ["Alice", "Bob", "Charlie", "Dylan"]
    player_data = {}
    for name in players_list:
        player_data[name] = gen_player_achievements()
        print(f"Player {name}: {player_data[name]}")
    print()

    all_distinct: set[str] = set()
    for name in players_list:
        all_distinct = all_distinct.union(player_data[name])
    print(f"All distinct achievements: {all_distinct}")
    print()
    common = set(player_data[players_list[0]])

    for name in players_list[1:]:
        common = common.intersection(player_data[name])
    print(f"Common achievements: {common}")
    print()

    for name in players_list:
        other_union: set[str] = set()
        for other in players_list:
            if other != name:
                other_union = other_union.union(player_data[other])
        only_has = player_data[name].difference(other_union)
        print(f"Only {name} has: {only_has}")
    print()
    for name in players_list:
        missing = set(ALL_ACHIEVEMENTS).difference(player_data[name])
        print(f"{name} is missing: {missing}")


if __name__ == "__main__":
    main()
