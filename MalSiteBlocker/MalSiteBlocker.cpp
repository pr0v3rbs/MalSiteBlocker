#include <stdio.h>
#include <Windows.h>
#include <tchar.h>
#include <process.h>
#include <ntddndis.h>
#include <share.h>
#include "MalSiteBlocker.h"
#include "DictionaryTree.h"
#include "../ndislwf/filteruser.h"

WCHAR gDeviceName[] = _T("\\\\.\\NDISLWF");
HANDLE gDeviceHandle;
bool gThreadTerminated;
bool gLocalServerTerminated;

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
    struct ScanInfo scanInfo;
    FILE *fp;

    fp = _fsopen("detect_list.txt", "a+", _SH_DENYWR);

    while (gThreadTerminated == FALSE)
    {
        if (DeviceIoControl(gDeviceHandle, IOCTL_FILTER_READ_URL, NULL, 0, buffer, sizeof(buffer), &deviceReturn, 0))
        {
            scanInfo.localPort = *(USHORT*)buffer;

            if (CheckString(&buffer[2]))
            {
                scanInfo.scanResult = kBad;
                fprintf(fp, "%s\n", &buffer[2]);
            }
            else
            {
                scanInfo.scanResult = kClean;
            }

            // send result
            DeviceIoControl(gDeviceHandle, IOCTL_FILTER_SEND_SCAN_RESULT, &scanInfo, sizeof(struct ScanInfo), NULL, 0, &deviceReturn, 0);
        }
        else
        {
            Sleep(100);
        }
    }

    fclose(fp);

    return 0;
}

unsigned __stdcall LocalServerThread(PVOID parameter)
{
    bool isFailed = false;
    WSADATA wsadata;
    SOCKET serverSocket;
    SOCKADDR_IN addr;
    char buffer[1024];

    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
    {
        printf("WSAStartup fail! : %d", WSAGetLastError());
        isFailed = true;
    }

    if (isFailed == false)
    {
        serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (serverSocket == SOCKET_ERROR)
        {
            printf("create socket fail!\n");
            isFailed = true;
        }
    }

    if (isFailed == false)
    {
        addr.sin_family = AF_INET;
        addr.sin_port = htons(8012);
        addr.sin_addr.s_addr = INADDR_ANY;
        if (bind(serverSocket, (SOCKADDR*)&addr, sizeof(addr)) == -1)
        {
            printf("Failed binding\n");
            isFailed = true;
        }
    }

    if (isFailed == false && listen(serverSocket, 5) != -1)
    {
        while (!gLocalServerTerminated)
        {
            int size = sizeof(addr);
            SOCKET c_s = accept(serverSocket, (SOCKADDR*)&addr, &size);
            recv(c_s, buffer, 1024, 0);
            send(c_s, "<html><body><meta http-equiv=\"Content-Type\" content=\"text/html;charset=UTF-8\">Danger Page</body></html>", 104, 0);
            closesocket(c_s);
        }
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}

bool LoadMalSite()
{
    bool result = false;
    FILE *fp;
    char buffer[100];

    if (fopen_s(&fp, "black_list.txt", "r") == 0)
    {
        while (fscanf_s(fp, "%s", buffer, 100) > 0)
        {
            InsertString(buffer);
        }

        result = true;
        fclose(fp);
    }

    return result;
}

int main()
{
    char userInput[256];
    HANDLE threadHandle = NULL;
    HANDLE localServerThreadHandle = NULL;
    unsigned threadID;

    InitTree();

    if (InitDevice())
    {
        localServerThreadHandle = (HANDLE)_beginthreadex(NULL, 0, &LocalServerThread, NULL, 0, &threadID);

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
                    gThreadTerminated = false;
                    threadHandle = (HANDLE)_beginthreadex(NULL, 0, &ThreadFunction, NULL, 0, &threadID);
                }
            }
            else if (userInput[0] == '2')
            {
                if (threadHandle && DeviceFilterSetting(FALSE))
                {
                    gThreadTerminated = true;
                    WaitForSingleObject(threadHandle, INFINITE);
                    threadHandle = NULL;
                }
            }
            else if (userInput[0] == '3')
            {
                if (LoadMalSite())
                {
                    printf("LoadMalSite Success!\n");
                }
                else
                {
                    printf("LoadMalSite Fail!\n");
                }
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