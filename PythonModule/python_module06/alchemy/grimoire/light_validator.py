#!/usr/bin/env python3
def validate_ingredients(ingredients: str) -> str:
    from .light_spellbook import (
        light_spell_allowed_ingredients,
    )

    allowed = light_spell_allowed_ingredients()
    ing_lower = ingredients.lower()

    if any(item in ing_lower for item in allowed):
        return f"{ingredients} - VALID"
    return f"{ingredients} - INVALID"
