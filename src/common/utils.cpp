#include "utils.hpp"

i32 is_power_of_two(usize x) {
    return (x & (x - 1)) == 0;
}
