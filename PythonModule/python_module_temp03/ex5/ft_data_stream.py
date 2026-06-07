#!/usr/bin/env python3
import random
from typing import Generator, Tuple

PLAYERS = ["alice", "bob", "charlie", "dylan"]
ACTIONS = [
    "run",
    "eat",
    "sleep",
    "grab",
    "move",
    "climb",
    "swim",
    "use",
    "release",
]


def gen_event() -> Generator[Tuple[str, str], None, None]:
    while True:
        player = random.choice(PLAYERS)
        action = random.choice(ACTIONS)
        yield (player, action)


def main() -> None:
    print("=== Game Data Stream Processor ===")
    stream = gen_event()
    for i in range(1000):
        player, action = next(stream)
        print(f"Event {i}: Player {player} did action {action}")

    event_list = []
    for _ in range(10):
        event = next(stream)
        event_list.append(event)
    print(f"Built list of 10 events: {event_list}")

    while event_list:
        chosen_event = random.choice(event_list)
        print(f"Got event from list: {chosen_event}")
        event_list.remove(chosen_event)
        print(f"Remains in list: {event_list}")


if __name__ == "__main__":
    main()
