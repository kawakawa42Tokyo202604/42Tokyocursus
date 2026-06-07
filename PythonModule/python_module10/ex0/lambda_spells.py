#!/usr/bin/env python3
from typing import Any


def artifact_sorter(artifacts: list[dict[str, Any]]) -> list[dict[str, Any]]:
    return sorted(artifacts, key=lambda x: x["power"], reverse=True)


def power_filter(
    mages: list[dict[str, Any]], min_power: int
) -> list[dict[str, Any]]:
    return list(filter(lambda x: x["power"] >= min_power, mages))


def spell_transformer(spells: list[str]) -> list[str]:
    return list(map(lambda x: f"* {x} *", spells))


def mage_stats(mages: list[dict[str, Any]]) -> dict[str, Any]:
    if not mages:
        return {"max_power": 0, "min_power": 0, "avg_power": 0.0}

    max_power = max(mages, key=lambda x: x["power"])["power"]
    min_power = min(mages, key=lambda x: x["power"])["power"]

    total_power = sum(map(lambda x: x["power"], mages))
    avg_power = round(total_power / len(mages), 2)

    return {
        "max_power": max_power,
        "min_power": min_power,
        "avg_power": avg_power,
    }


def main() -> None:
    print("Testing artifact sorter...")
    artifacts = [
        {"name": "Crystal Orb", "power": 85, "type": "orb"},
        {"name": "Fire Staff", "power": 92, "type": "staff"},
    ]
    sorted_arts = artifact_sorter(artifacts)
    if len(sorted_arts) >= 2:
        n0, p0 = sorted_arts[0]["name"], sorted_arts[0]["power"]
        n1, p1 = sorted_arts[1]["name"], sorted_arts[1]["power"]
        print(f"{n0} ({p0} power) comes before {n1} ({p1} power)")

    print("Testing spell transformer...")
    spells = ["fireball", "heal", "shield"]
    transformed = spell_transformer(spells)
    print(" ".join(transformed))


if __name__ == "__main__":
    main()
