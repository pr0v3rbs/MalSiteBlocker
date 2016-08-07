#pragma once


struct UrlInfo
{
    LIST_ENTRY listEntry;
    PCHAR url;
    UINT32 hostLength;
    UINT32 urlLength;
    UINT16 localPort;
    BOOLEAN isSendToUser;
    ScanResult scanResult;
    TIME packetRequestTime;
    BOOLEAN isCopiedReceivePacket;
    NDIS_HANDLE filterHandle;
    PNET_BUFFER_LIST netBufferList;
    NDIS_PORT_NUMBER portNumber;
    ULONG numberOfNetBufferList;
    ULONG receiveFlags;
};

VOID InitializeUrlListTable();

VOID ClearUrlListTable();

VOID UninitializeUrlListTable();

VOID InsertUrl(_In_ NDIS_HANDLE ndisHandle, _In_ char* urlString, _In_ UINT32 hostLength, _In_ UINT32 urlLength, _In_ PCHAR framePacket, _In_ BOOLEAN isDispatchLevel);

BOOLEAN IsScanningUrlPort(_In_ UINT16 port, _Outptr_ struct UrlInfo** urlInfo);

BOOLEAN GetUrlInfo(_Outptr_ struct UrlInfo** urlInfo);

BOOLEAN GetNeedToDeleteListEntry(_Outptr_ struct UrlInfo** urlInfo);

BOOLEAN GetNeedToSendPacketListEntry(_Outptr_ struct UrlInfo** urlInfo);

VOID SaveScanResult(_In_ struct ScanInfo* scanInfo);

VOID DeleteUrlInfo(_In_ struct UrlInfo* urlInfo);