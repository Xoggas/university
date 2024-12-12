#include <assert.h>
#include <stdio.h>

int absolute(const int a)
{
    if (a < 0)
    {
        return -a;
    }

    return a == 0 ? 1 : a;
}

int power(const int a, const int p)
{
    int result = 1;

    for (int i = 0; i < p; i++)
    {
        result *= a;
    }

    return result;
}

int k(const int i, const int j)
{
    int result = 0;

    if (i % 3 == 0)
    {
        result = i / 3;
    }
    else
    {
        result = 5 * ((i + j) / absolute(i - j));
    }

    return absolute(result);
}

int main()
{
    int x[] = { 2, 4, 6, 8 };
    int y[] = { 1, 2, 3, 4 };
    int m = 4;
    int n = 4;
    int sum = 0;

    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            const int left = x[i] / k(i, j);
            const int numerator = power(y[i], 3) * x[i] * i + 6;
            const int denominator = power(x[i], 2);
            sum += left - numerator / denominator;
        }
    }

    printf("%d", sum);

    return 0;
}
