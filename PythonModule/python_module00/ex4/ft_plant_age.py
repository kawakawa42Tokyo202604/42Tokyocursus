def ft_plant_age():
    res = int(input("Enter plant age in days: "))
    if res > 60:
        print("Plant is ready to harvest!")
    else:
        print("Plant needs more time to grow.")
