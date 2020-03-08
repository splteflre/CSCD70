int sum(int a, int b)
{
        int res = 1;

        for (int i = a; i < b; i++)
        {
                res *= i;
        }
        return res;
}


int branch(short cond, int val, int a, int b)
{
        int x = val;
        int z = 22;
        if (cond)
        {
                x = a + b;
                z = z+a;
        }
        else
        {
                x = a * b;
                z = z-b;
        }
        int y = x + z;
        return y;
}
