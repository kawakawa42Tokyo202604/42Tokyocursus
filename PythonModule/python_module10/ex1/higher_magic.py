#!/usr/bin/env python3
from typing import Callable
SpellType = Callable[[str, int], str]


def spell_combiner(
    spell1: SpellType, spell2: SpellType
) -> Callable[[str, int], tuple[str, str]]:
    if not callable(spell1) or not callable(spell2):
        raise TypeError("Both inputs must be callable spells")

    def combined(target: str, power: int) -> tuple[str, str]:
        return (spell1(target, power), spell2(target, power))
    return combined


def power_amplifier(base_spell: SpellType, multiplier: int) -> SpellType:
    if not callable(base_spell):
        raise TypeError("The base spell must be callable")

    def amplified(target: str, power: int) -> str:
        return base_spell(target, power * multiplier)
    return amplified


def conditional_caster(
    condition: Callable[[str, int], bool], spell: SpellType
) -> SpellType:
    if not callable(condition) or not callable(spell):
        raise TypeError("Condition and spell must be callable")

    def caster(target: str, power: int) -> str:
        if condition(target, power):
            return spell(target, power)
        return "Spell fizzled"
    return caster


def spell_sequence(spells: list[SpellType]) -> Callable[[str, int], list[str]]:
    for s in spells:
        if not callable(s):
            raise TypeError("All items in the sequence must be callable")

    def sequence(target: str, power: int) -> list[str]:
        return [s(target, power) for s in spells]
    return sequence


def fireball(target: str, power: int) -> str:
    return f"Fireball hits {target}"


def heal(target: str, power: int) -> str:
    return f"Heals {target}"


def main() -> None:
    print("Testing spell combiner...")
    combined = spell_combiner(fireball, heal)
    res = combined("Dragon", 10)
    print(f"Combined spell result: {res[0]}, {res[1]}")

    print("Testing power amplifier...")
    base_p = 10
    mult = 3
    print(f"Original: {base_p}, Amplified: {base_p * mult}")


if __name__ == "__main__":
    main()
