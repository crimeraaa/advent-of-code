import math

BASE_BINARY: int = 2

def bit_base2_upper_body(value: int, function) -> tuple[int, int]:
    """ 
    Example usages:
    ```py
    # Get a power of 2 normally
    bit_base2_upper_body(9001, lambda power: 2 ** power)

    # Bit shifting magic to mimic getting a power of 2
    bit_base2_upper_body(45000, lambda power: 1 << power)
    ```
    """
    if value < 0:
        raise ValueError("Cannot get upper power of 2 for negative number!")
    elif value == 0:
        return value, 1
    power = (value - 1).bit_length()
    return value, function(power)

def bit_power_upper(value: int) -> tuple[int, int]:
    """ 
    We can assume that `value - 1`'s bit length is the exponent of the nearest
    power of 2. For example:
    ```
    value = 13:
        value - 1    = 12: 
        bit_length() = 4: 
        nearest      = 2^4 = 16
    value = 4:
        value - 1    = 3: 
        bit_length() = 2: 
        nearest      = 2^2 = 4
    ```
    """
    if value < 0:
        raise ValueError("Cannot get upper power of 2 for negative number!")
    elif value == 0:
        return (value, 1)
    else:
        power = (value - 1).bit_length()
        # ** is exponentiation.
        return (value, BASE_BINARY ** power)


def bit_shift_width(value: int) -> tuple[int, int]:
    """ 
    We can approximate exponentiation using left bit shifting.
    """
    if value < 0:
        raise ValueError("Cannot get upper power of 2 for negative numbers!")
    elif value == 0:
        return (value, 1)
    else:
        power = (value - 1).bit_length()
        return (value, 1 << power)


def power_log(value: int) -> tuple[int, int]:
    if value == 0:
        return value, 1
    else:
        # Get exponent needed to represent the nearest upper power of `base`.
        power = math.ceil(math.log2(value)) 
        # ** is exponentiation.
        return value, BASE_BINARY ** power 


while True:
    # * is argument unpacking, ** is keyword-argument-unpacking.
    # https://stackoverflow.com/a/406488
    try:
        test = int(input("Enter a number: "))
        print("value: {}, upper: {}".format(*bit_power_upper(test)))
    except (KeyboardInterrupt, EOFError) as error:
        print("Received termination signal.")
        break
    except ValueError as error:
        print(error)
        continue
