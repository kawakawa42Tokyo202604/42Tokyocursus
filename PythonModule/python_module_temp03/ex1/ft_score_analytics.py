#!/usr/bin/env python3
import sys


def main() -> None:
    print("=== Player Score Analytics ===")
    if len(sys.argv) < 2:
        print(
            "No scores provided. Usage: python3 ft_score_analytics.py "
            "<score1> <score2> ..."
        )
        return

    scores = []
    for arg in sys.argv[1:]:
        try:
            score = int(arg)
            scores.append(score)
        except ValueError:
            print(f"Invalid parameter: '{arg}'")
    print(f"Scores processed: {scores}")

    if not scores:
        print(
            "No scores provided. Usage: python3 ft_score_analytics.py "
            "<score1> <score2> ..."
        )
        return

    count = len(scores)
    total = sum(scores)
    average = float(total / count)
    high_score = max(scores)
    low_score = min(scores)
    score_range = high_score - low_score

    print(f"Total players: {count}")
    print(f"Total score: {total}")
    print(f"Average score: {average}")
    print(f"High score: {high_score}")
    print(f"Low score: {low_score}")
    print(f"Score range: {score_range}")


if __name__ == "__main__":
    main()
