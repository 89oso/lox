#include <stdio.h>

int main(void) {
    printf("--- example 1 ---\n");
    {
        int a = 4;
        int b = 1;
        int c = 1;

        if (a == 1 || b == 1 && c == 1)
            printf("1\n");
    }

    return 0;
}
