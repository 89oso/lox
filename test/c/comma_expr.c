#include <stdio.h>

int value() {
    return 5 + 5, 10, 4 + 4;
}

int value2() {
    return 5 + 5, 10, 1 + 1;
}

int main(void) {
    printf("--- example 1 ---\n");
    {
        int a, b;
        a = value(), value2();
        printf("a = %d\n", a);

        b = (value(), value2());
        printf("b = %d\n", b);

        a = b = 0;
        a, b = (value(), value2());
        printf("a = %d, b = %d\n", a, b);
    }

    printf("--- example 2 ---\n");
    {
        int x, y, z;

        x = 1;
        y = 2;
        z = x++, y++, x + y; // in this case z = x++ -> 1

        printf("x = %d, y = %d, z = %d\n", x, y, z);

        x = 1;
        y = 2;
        z = (x++, y++, x + y); // in this case everything on the left of the group is evaluated and z = x + y -> 5

        printf("x = %d, y = %d, z = %d\n", x, y, z);

        x = 1;
        y = 1;

        if (x--, y--, x == y) {
            printf("entered condition\n");
        }

        printf("x = %d, y = %d, z = %d\n", x, y, z);
    }

    return 0;
}
