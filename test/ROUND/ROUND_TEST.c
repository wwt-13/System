#include <stdio.h>
#include <stdlib.h>
typedef unsigned long long u_long;
#define ROUND(a, n) (((((u_long)(a)) + (n)-1)) & ~((n)-1))

int main()
{
    double a;
    int n;
    scanf("%lf %d", &a, &n);
    printf("%d", ROUND(a, n));
    // printf("Hello");
    return 0;
}