#!/usr/bin/env python3
from .factory import (
    CreatureFactory as CreatureFactory,
    FlameFactory as FlameFactory,
    AquaFactory as AquaFactory,
)
from .creature import Creature as Creature

__all__ = [
    "Creature",
    "CreatureFactory",
    "FlameFactory",
    "AquaFactory",
]
