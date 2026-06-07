def ft_print_count_harvest(i: int, res: int):
    if res > i:
        print(f"Day {i + 1}")
        ft_print_count_harvest(i + 1, res)
    else:
        return


def ft_count_harvest_recursive():
    res = int(input("Days until harvest: "))
    ft_print_count_harvest(0, res)
    print("Harvest time!")
