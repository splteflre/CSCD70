int loop(int a, int b, int c)
{
    int i = 0, ret = 0;

    int d = a << 2;
    int e = 3;

    do {
        d = d + e;
        e = 2;
    } while (i < a);

    return d;
}
