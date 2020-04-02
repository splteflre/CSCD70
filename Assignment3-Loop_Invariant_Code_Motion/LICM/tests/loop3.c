int loop(int a, int b, int c)
{
        int i = 0, ret = 0;

        int d = a << 2;
        int e;

        do {
            if (a > 3) 
            {               
                e = c;
            }else
            {
                e = b;
            }               
            d = e + 5;
        } while (i < a);

        return d + ret;
}
