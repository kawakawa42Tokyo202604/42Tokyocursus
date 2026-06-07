def ft_water_reminder():
    res = int(input("Days since last watering: "))
    if res > 2:
        print("Water the plants!")
    else:
        print("Plants are fine")
