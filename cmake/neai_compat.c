#include <math.h>

float __hardfp_expf(float x)
{
    return expf(x);
}

int *__errno(void)
{
    static int s_errno;
    return &s_errno;
}
