#include <stdio.h>
#include <Windows.h>
#include <tchar.h>
#include <process.h>
#include <ntddndis.h>
#include "MalSiteBlocker.h"
#include "../ndislwf/filteruser.h"

WCHAR gDeviceName[] = _T("\\\\.\\NDISLWF");
HANDLE gDeviceHandle;
BOOL gThreadTerminated;

bool InitDevice()
{
    bool result = false;

    gDeviceHandle = CreateFile(gDeviceName,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (gDeviceHandle != INVALID_HANDLE_VALUE)
    {
        result = true;
    }

    return result;
}

BOOL DeviceFilterSetting(bool isEnable)
{
    DWORD deviceReturn;

    return DeviceIoControl(gDeviceHandle, IOCTL_FILTER_SETTING, &isEnable, sizeof(isEnable), NULL, 0, &deviceReturn, 0);
}

unsigned __stdcall ThreadFunction(PVOID parameter)
{
    char buffer[1024];
    DWORD deviceReturn;
    char *url;
    struct ScanInfo scanInfo;

    while (gThreadTerminated == FALSE)
    {
        if (DeviceIoControl(gDeviceHandle, IOCTL_FILTER_READ_URL, NULL, 0, buffer, sizeof(buffer), &deviceReturn, 0))
        {
            scanInfo.localPort = *(USHORT*)buffer;
            url = &buffer[2];
            scanInfo.scanResult = kClean;
            printf("url - %s\n", url);

            // check url

            // send result
            DeviceIoControl(gDeviceHandle, IOCTL_FILTER_SEND_SCAN_RESULT, &scanInfo, sizeof(struct ScanInfo), NULL, 0, &deviceReturn, 0);
        }

        Sleep(100);
    }

    return 0;
}

int main()
{
    char userInput[256];
    HANDLE threadHandle = NULL;
    unsigned threadID;

    if (InitDevice())
    {
        while (1)
        {
            printf("1. Filter Enable\n");
            printf("2. Filter Disable\n");
            printf("3. Load Mal Site\n");
            printf("4. Exit\n");

            gets_s(userInput, 256);

            if (userInput[0] == '1')
            {
                if (threadHandle == NULL && DeviceFilterSetting(TRUE))
                {
                    gThreadTerminated = FALSE;
                    threadHandle = (HANDLE)_beginthreadex(NULL, 0, &ThreadFunction, NULL, 0, &threadID);
                }
            }
            else if (userInput[0] == '2')
            {
                if (threadHandle && DeviceFilterSetting(FALSE))
                {
                    gThreadTerminated = TRUE;
                    WaitForSingleObject(threadHandle, INFINITE);
                    threadHandle = NULL;
                }
            }
            else if (userInput[0] == '3')
            {
                //LoadMalSite();
            }
            else if (userInput[0] == '4')
                break;
        }
    }
    else
    {
        printf("Init Device Fail!\n");
    }

    return 0;
}