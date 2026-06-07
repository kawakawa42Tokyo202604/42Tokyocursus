#!/usr/bin/env python3
import time
from functools import wraps
from typing import Any, Callable

InnerFunc = Callable[..., Any]
DecoratorType = Callable[[InnerFunc], InnerFunc]


def spell_timer(func: InnerFunc) -> InnerFunc:
    @wraps(func)
    def wrapper(*args: Any, **kwargs: Any) -> Any:
        print(f"Casting {func.__name__}...")
        start_time = time.perf_counter()
        result = func(*args, **kwargs)
        end_time = time.perf_counter()
        duration = end_time - start_time
        print(f"Spell completed in {duration:.3f} seconds")
        return result
    return wrapper
