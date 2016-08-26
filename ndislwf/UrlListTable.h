#pragma once


struct UrlInfo
{
    LIST_ENTRY listEntry;
    PCHAR url;
    UINT32 urlLength;
    UINT16 localPort;
    BOOLEAN isSendToUser;
    ScanResult scanResult;
    TIME packetRequestTime;
    PNET_BUFFER_LIST recvNetBufferLists;
    NDIS_PORT_NUMBER recvPortNumber;
    ULONG recvNumberOfNetBufferList;
    ULONG receiveFlags;
    PNET_BUFFER_LIST sendNetBufferLists;
    NDIS_PORT_NUMBER sendPortNumber;
    ULONG sendFlags;
    BOOLEAN isSendComplete;
    BOOLEAN isRecvComplete;
};

VOID InitializeUrlListTable();

VOID ClearUrlListTable();

VOID UninitializeUrlListTable();

VOID InsertUrl(_In_ NDIS_HANDLE ndisHandle, _In_ char* urlString, _In_ UINT32 urlLength, _In_ USHORT srcPort, _In_ BOOLEAN isDispatchLevel);

BOOLEAN IsScanningUrlPort(_In_ UINT16 port, _Outptr_ struct UrlInfo** urlInfo);

BOOLEAN GetUrlInfo(_Outptr_ struct UrlInfo** urlInfo);

BOOLEAN GetNeedToDeleteListEntry(_Outptr_ struct UrlInfo** urlInfo);

BOOLEAN GetNeedToSendPacketListEntry(_Outptr_ struct UrlInfo** urlInfo);

BOOLEAN GetNeedToReceivePacketListEntry(_Outptr_ struct UrlInfo** urlInfo);

VOID SaveScanResult(_In_ struct ScanInfo* scanInfo);

VOID DeleteUrlInfo(_In_ struct UrlInfo* urlInfo);