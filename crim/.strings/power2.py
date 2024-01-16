import math

# powers2: list[int] = []
# for i in range(0, 16, 1):
    # powers2.append(int(math.pow(2, i)))


def find_upper_power(value: int, base: int) -> int:
    """ 
    GIVEN:      c = 2^3
    QUESTION:   We take base b = 2 and raise it to power k = 3.
                What is the result c?
    ANSWER:     8
                    
    GIVEN:      2^k = 8
    QUESTION:   We have base b = 2 and result c = 8.
                What is exponent k?
    ANSWER:     log base 2 c = k = 3
    """
    if value == 0:
        return 1
    # https://en.wikipedia.org/wiki/Logarithm#Logarithmic_identities
    return math.pow(base, math.ceil(math.log(value) / math.log(base)))


while True:
    try:
        value = int(input("Enter a number: "))
        print(f"Nearest power of 2: {find_upper_power(value, 2)}")
    except (KeyboardInterrupt, EOFError) as error:
        # https://www.wilfred.me.uk/blog/2013/11/03/no-naked-excepts/
        print("Received termination signal, ending program.")
        break
    except ValueError:
        print("What did you input?!")
        continue
