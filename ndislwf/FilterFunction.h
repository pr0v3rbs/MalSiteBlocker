#pragma once

#include "UrlListTable.h"

BOOLEAN IsWhiteListUrl(_In_ PCHAR urlString, _In_ UINT32 hostLength);

BOOLEAN AnalyzePacketAndParseUrl(_In_ PCHAR httpPacket, _In_ NDIS_HANDLE ndisHandle, _Outptr_ PCHAR* urlString, _Outptr_ UINT32* hostLength, _Outptr_ UINT32* urlLength);

VOID CopyRedirectPage(_Inout_ CHAR* httpPacket, _In_ USHORT port);

BOOLEAN CopyNetBufferLists(_In_ PNET_BUFFER_LIST netBufferLists, _Outptr_ PNET_BUFFER_LIST* outNetBufferList);

VOID FreeNetBufferLists(_In_ PNET_BUFFER_LIST netBufferList);