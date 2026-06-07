#!/usr/bin/env python3
from .elements import create_air, create_earth
from elements import create_fire, create_water


def healing_potion() -> str:
    earth_element = create_earth()
    air_element = create_air()

    return f"Healing potion brewed with '{earth_element}' and '{air_element}'"


def strength_potion() -> str:
    fire_element = create_fire()
    water_element = create_water()
    return (
        f"Strength potion brewed with '{fire_element}' "
        f"and '{water_element}'"
    )
