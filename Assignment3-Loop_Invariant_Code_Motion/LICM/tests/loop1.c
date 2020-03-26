int loop(int a, int b, int c)
{
        int i = 0, ret = 0;

        int d;
        if (b == 0xd70)
        {
            d = 469;
        }
        else
        {
            d = 369;
        }
        

        do {
                c = d * 2;
                i++;
        } while (i < a);

        return ret + c;
}
