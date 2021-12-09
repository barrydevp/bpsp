#include "util.h"

int main (int argc, char *argv[])
{
    char date_now[33];

    date_now_utc(date_now, 33);

    printf("%s\n", date_now);
    
    return 0;
}
