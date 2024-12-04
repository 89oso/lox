#include <stdio.h>

int main(void) {
    printf("--- example 1 ---\n");
    {
        int x = 0;
        int y = 0;

        int a = x == y ? 42 : 0;
        printf("a = %d\n", a);
    }

    printf("--- example 2 ---\n");
    {
        int x = 0;
        int y = 0;

        int a = x == y ? x == 0 ? 56 : 42 : 0;
        printf("a = %d\n", a);
    }

    printf("--- example 3 ---\n");
    {
        int x = 0;
        int y = 1;

        int a = x == y ? 42 : 0 ? 21 : 1;
        // int a = 1 == 0 ? 42 : 0 ? 21 : 1;
        printf("a = %d\n", a);
    }

    return 0;
}
