#include "precomp.h"
#include "UrlListTable.h"
#include "WhiteListTable.h"

LIST_ENTRY gWhiteListHead;
FILTER_LOCK gWhiteListLock;

#define __FILENUMBER    'PNPF'

VOID InitializeWhiteListTable()
{
    InitializeListHead(&gWhiteListHead);
    FILTER_INIT_LOCK(&gWhiteListLock);
}

VOID ClearWhiteListTable()
{
    PLIST_ENTRY listEntry;
    struct WhiteInfo* whiteInfo;

    while (gWhiteListHead.Flink != &gWhiteListHead)
    {
        FILTER_ACQUIRE_LOCK(&gWhiteListLock, FALSE);
        listEntry = RemoveHeadList(&gWhiteListHead);
        FILTER_RELEASE_LOCK(&gWhiteListLock, FALSE);
        whiteInfo = CONTAINING_RECORD(listEntry, struct WhiteInfo, listEntry);
        ExFreePool(whiteInfo->url);
        ExFreePool(whiteInfo);
    }
}

VOID UninitializeWhiteListTable()
{
    ClearWhiteListTable();
    RemoveHeadList(&gWhiteListHead);
    FILTER_FREE_LOCK(&gWhiteListLock);
}

VOID AddVirustotal()
{
    UINT32 strSize = 0;
    PCHAR str;

    strSize = 14;
    str = ExAllocatePool(NonPagedPool, strSize);
    RtlCopyMemory(str, "virustotal.com", strSize);
    InsertWhiteInfo(str, strSize);

    /*strSize = 18;
    str = ExAllocatePool(NonPagedPool, strSize);
    RtlCopyMemory(str, "www.virustotal.com", strSize);
    InsertWhiteInfo(str, strSize);*/
}

VOID SaveWhiteList(_In_ PUCHAR data)
{
    UINT32 dataIdx = 0;
    UINT32 strNumber = *(UINT32*)&data[dataIdx];
    UINT32 strSize = 0;
    PCHAR str;
    UINT32 i = 0;
    dataIdx += 4;

    for (; i < strNumber; i++)
    {
        strSize = *(UINT32*)&data[dataIdx];
        dataIdx += 4;
        str = ExAllocatePool(NonPagedPool, strSize);
        RtlCopyMemory(str, &data[dataIdx], strSize);
        dataIdx += strSize;

        InsertWhiteInfo(str, strSize);
    }
}

VOID InsertWhiteInfo(_In_ char* hostString, _In_ UINT32 hostLength)
{
    struct WhiteInfo* whiteInfo = NULL;
    BOOLEAN isDispatchLevel = KeGetCurrentIrql() == DISPATCH_LEVEL;
    whiteInfo = ExAllocatePool(NonPagedPool, sizeof(struct WhiteInfo));

    whiteInfo->url = hostString;
    whiteInfo->hostLength = hostLength;

    KeGetCurrentIrql();
    FILTER_ACQUIRE_LOCK(&gWhiteListLock, isDispatchLevel);
    InsertTailList(&gWhiteListHead, &whiteInfo->listEntry);
    FILTER_RELEASE_LOCK(&gWhiteListLock, isDispatchLevel);
}

BOOLEAN IsInWhiteList(_In_ PCHAR urlString, _In_ UINT32 hostLength)
{
    BOOLEAN result = FALSE;
    PLIST_ENTRY listEntry = gWhiteListHead.Flink;
    struct WhiteInfo* whiteInfo = NULL;

    FILTER_ACQUIRE_LOCK(&gWhiteListLock, FALSE);
    while (listEntry != &gWhiteListHead)
    {
        whiteInfo = CONTAINING_RECORD(listEntry, struct WhiteInfo, listEntry);
        if (whiteInfo->hostLength == hostLength &&
            RtlEqualMemory(whiteInfo->url, urlString, hostLength) == 1)
        {
            result = TRUE;
            break;
        }
        listEntry = listEntry->Flink;
    }
    FILTER_RELEASE_LOCK(&gWhiteListLock, FALSE);

    return result;
}