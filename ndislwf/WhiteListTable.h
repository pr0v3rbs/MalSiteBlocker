#pragma once

struct WhiteInfo
{
    LIST_ENTRY listEntry;
    UINT32 hostLength;
    PCHAR url;
};

VOID InitializeWhiteListTable();

VOID ClearWhiteListTable();

VOID UninitializeWhiteListTable();

VOID AddVirustotal();

VOID SaveWhiteList(_In_ PUCHAR data);

VOID InsertWhiteInfo(_In_ char* hostString, _In_ UINT32 hostLength);

BOOLEAN IsInWhiteList(_In_ PCHAR urlString, _In_ UINT32 hostLength);