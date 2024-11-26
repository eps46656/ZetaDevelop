import termcolor

def main1():
    x = termcolor.colored("abc", "red")

    print(f"{type(x)=}")

    print(x)

if __name__ == "__main__":
    main1()
