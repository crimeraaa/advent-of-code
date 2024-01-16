import math

# powers2: list[int] = []
# for i in range(0, 16, 1):
    # powers2.append(int(math.pow(2, i)))


def upper_powerA(value: int, base: int) -> int:
    """ 
    Naive solution to find the upper power of `base` in relation to `value`.
    WARNING: This only properly works for positive numbers!
    """
    power: int = 1
    while (power < value):
        power *= base
    return power

def upper_powerB(value: int, base: int) -> int:
    """ 
    Mathematical solution to find the upper power of `base` relative to `value`.
        - https://stackoverflow.com/a/466256
        - https://www.reddit.com/r/learnpython/comments/7ga2ng/comment/dqhyxdd/?utm_source=share&utm_medium=web3x&utm_name=web3xcss&utm_term=1&utm_content=share_button 

    e.g. `upper_powerB(10, 2) = 16`, `upper_powerB(3, 2) = 4`, and so on.

    How does this work? Let's look at `value = 10`, `base = 2`.
    Remember that `log_base2(10) = x` is the same as `2 ^ x = 10`.
    
    log_base2(10) or math.log(10, 2) is approx. 3.32.
    We know that the nearest upper power of 2 relative to 10 is 16,
    so we can assume that we need to round up 3.32 to get 4, as 2^4 = 16.
    """
    # Powers of 0 are always 1
    if value == 0:
        return 1
    log = math.ceil(math.log(value) / math.log(base))
    # https://en.wikipedia.org/wiki/Logarithm#Logarithmic_identities
    return math.pow(base, log)


while True:
    try:
        value = int(input("Enter a number: "))
        print(f"Nearest power of 2: {upper_powerB(value, 2)}")
    except (KeyboardInterrupt, EOFError) as error:
        # https://www.wilfred.me.uk/blog/2013/11/03/no-naked-excepts/
        print("Received termination signal, ending program.")
        break
    except ValueError:
        print("What did you input?!")
        continue
