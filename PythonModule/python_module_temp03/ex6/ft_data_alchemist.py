#!/usr/bin/env python3
import random


def main() -> None:
    print("=== Game Data Alchemist ===")
    print()
    initial_players = [
        "Alice",
        "bob",
        "Charlie",
        "dylan",
        "Emma",
        "Gregory",
        "john",
        "kevin",
        "Liam",
    ]

    print(f"Initial list of players: {initial_players}")
    capitalized_list = []
    for name in initial_players:
        capitalized_list.append(name.capitalize())
    print(f"New list with all names capitalized: {capitalized_list}")

    title_list = []
    for name in initial_players:
        if name.istitle():
            title_list.append(name)
    print(f"New list of capitalized names only: {title_list}")
    print()

    score_dict = {name: random.randint(1, 1000) for name in capitalized_list}
    print(f"Score dict: {score_dict}")

    average = sum(score_dict.values()) / len(score_dict)
    print(f"Score average is {average:.2f}")

    high_scores = {}
    for name, score in score_dict.items():
        if score > average:
            high_scores[name] = score
    print(f"High scores: {high_scores}")


if __name__ == "__main__":
    main()
