def ft_seed_inventory(seed_type: str, quantity: int, unit: str) -> None:
    arr = seed_type.capitalize()
    if unit == 'packets':
        print(f"{arr} seeds: {quantity} {unit} available")
    elif unit == 'grams':
        print(f"{arr} seeds: {quantity} {unit} total")
    elif unit == 'area':
        print(f"{arr} seeds: covers {quantity} square meters")
    else:
        print("Unknown unit type")
