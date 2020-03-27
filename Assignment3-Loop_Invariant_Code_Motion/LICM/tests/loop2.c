int loop(int a, int b, int c)
{
        int i = 0, ret = 0;

        int d = a << 2;

        do {
            int e = d;
            d = b + c;
        } while (i < a);

        return ret + c;
}
