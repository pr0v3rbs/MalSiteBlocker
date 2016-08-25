#include "precomp.h"
#include "UrlListTable.h"
#include "UrlListThread.h"

HANDLE gThreadHandle;
PVOID gThreadObject;
BOOLEAN gTerminateThread = FALSE;

NTSTATUS CreateDeleteUrlListThread()
{
    NTSTATUS status = STATUS_UNSUCCESSFUL;

    status = PsCreateSystemThread(&gThreadHandle, THREAD_ALL_ACCESS, NULL, NULL, NULL, DeleteUrlListThreadProc, NULL);

    if (NT_SUCCESS(status))
    {
        status = ObReferenceObjectByHandle(gThreadHandle, 0, NULL, KernelMode, &gThreadObject, NULL);
    }

    return status;
}

VOID DeleteUrlListThreadProc(_In_ PVOID param)
{
    TIME time;
    struct UrlInfo* urlInfo;

    UNREFERENCED_PARAMETER(param);

    time.QuadPart = -1000000; // 0.1√ 

    while (gTerminateThread == FALSE)
    {
        if (gFilterEnable)
        {
            // check send rst packet and recv redirect packet
            if (GetNeedToDeleteListEntry(&urlInfo))
            {
                DeleteUrlInfo(urlInfo);
            }
        }

        KeDelayExecutionThread(KernelMode, FALSE, &time);
    }
}

VOID TerminateDeleteUrlListThread()
{
    gTerminateThread = TRUE;

    KeWaitForSingleObject(gThreadObject, Executive, KernelMode, FALSE, NULL);

    ObDereferenceObject(gThreadObject);

    ZwClose(gThreadHandle);
}