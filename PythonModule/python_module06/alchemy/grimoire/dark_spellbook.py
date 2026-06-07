#!/usr/bin/env python3
from .dark_validator import validate_ingredients


def dark_spell_allowed_ingredients() -> list[str]:
    return ["bats", "frogs", "arsenic", "eyeball"]


def dark_spell_record(spell_name: str, ingredients: str) -> str:
    status = validate_ingredients(ingredients)
    if "VALID" in status:
        return f"Spell recorded: {spell_name} ({status})"
    return f"Spell rejected: {spell_name} ({status})"
