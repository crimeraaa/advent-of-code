import math
from typing import Callable, TypeAlias

# Creates a type alias w/ argument and return type-hinting
bitfn:   TypeAlias = Callable[[int], int]
bitpair: TypeAlias = tuple[int, int]

BASE_BINARY: int = 2

# We need the Callable type to help type-hint our lambdas.
# See: https://stackoverflow.com/a/46105207
def bit_callback(value: int, func: Callable[[int], int]) -> tuple[int, int]:
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
    return value, func(power)


def bit_power_length(value: int):
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
    return bit_callback(value, lambda power: BASE_BINARY ** power)


def bit_shift_length(value: int):
    """ 
    We can approximate exponentiation using left bit shifting.
    """
    return bit_callback(value, lambda power: 1 << power)


def bit_power_log(value: int):
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
        print("\tvalue: {}, upper: {}".format(*bit_shift_length(test)))
    except (KeyboardInterrupt, EOFError) as error:
        print("Received termination signal.")
        break
    except ValueError as error:
        print(error)
        continue
