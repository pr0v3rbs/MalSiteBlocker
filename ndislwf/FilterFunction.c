#include "precomp.h"
#include "FilterFunction.h"
#include "WhiteListTable.h"

#define __FILENUMBER    'PNPF'

#define ISDOUBLE_ENTER(data, idx) (data[idx] == '\x0d' && data[idx + 1] == '\x0a' && data[idx + 2] == '\x0d' && data[idx + 3] == '\x0a')
#define IS_ENTER(data, idx) (data[idx] == '\x0d' && data[idx + 1] == '\x0a')

//CHAR gAcceptTextString[37] = "Accept: application/x-ms-application"; // internet explorer 8에서 페이지 요청시
CHAR gAcceptTextString[18] = "Accept: text/html";
CHAR gRefererTextString[10] = "Referer: ";
CHAR gHostString[7] = "Host: ";

BOOLEAN AnalyzePacketAndParseUrl(_In_ PCHAR httpPacket, _In_ NDIS_HANDLE ndisHandle, _Outptr_ PCHAR* urlString, _Outptr_ UINT32* urlLength)
{
    BOOLEAN result = FALSE;
    BOOLEAN isAcceptText = FALSE;
    int packetIdx = 4;
    int hostIdx = 0;

    if (NdisEqualMemory(httpPacket, "GET", 3) == 1 ||
        NdisEqualMemory(httpPacket, "POST", 4) == 1) // check GET/POST method
    {
        // 패킷의 끝인 "\x0d\x0a\x0d\x0a" 인지 확인한다.
        while (!ISDOUBLE_ENTER(httpPacket, packetIdx))
        {
            if (IS_ENTER(httpPacket, packetIdx))
            {
                packetIdx += 2;
            }

            // Host 헤더를 체크한 후 값을 가져온다.
            if (NdisEqualMemory(&httpPacket[packetIdx], gHostString, sizeof(gHostString) - 1) == 1)
            {
                packetIdx += 6;
                while (!IS_ENTER(httpPacket, packetIdx))
                {
                    ++packetIdx;
                    ++hostIdx;
                }

                *urlString = (PCHAR)FILTER_ALLOC_MEM(ndisHandle, hostIdx + 1);
                if (*urlString != NULL)
                {
                    NdisMoveMemory(*urlString, &httpPacket[packetIdx - hostIdx], hostIdx);
                    (*urlString)[hostIdx] = 0;
                    *urlLength = hostIdx + 1;
                }
            }
            // 강제접속을 하려는 시도인지 확인한다.
            else if (NdisEqualMemory(&httpPacket[packetIdx], gRefererTextString, sizeof(gRefererTextString) - 1) == 1)
            {
                if (NdisEqualMemory(&httpPacket[packetIdx + 9], "http://127.0.0.1:8012/", 22) == 1)
                {
                    isAcceptText = FALSE;
                    break;
                }
            }

            // 패킷의 현재라인의 맨 끝으로 이동한다. "\x0d\x0a"
            while (!IS_ENTER(httpPacket, packetIdx))
            {
                ++packetIdx;
            }
        }
    }

    if (*urlString)
    {
        result = TRUE;
    }

    return result;
}

VOID CopyDangerPage(_Inout_ CHAR* httpPacket, _In_ USHORT port)
{
    ULONG idx = 87;
    USHORT divideNum = 10000;

    // 경고페이지를 출력해줄 로컬서버로 리다이렉트 해주는 페이지로 변경
    NdisMoveMemory(httpPacket, "HTTP/1.1 200 OK\x0d\x0a\x0d\x0a<script type='text/javascript'>location.href='http://127.0.0.1:8012/", 87);
    
    for (; divideNum != 0; divideNum /= 10)
    {
        httpPacket[idx++] = '0' + (UCHAR)(port / divideNum);
        port = port % divideNum;
    }

    NdisMoveMemory(&httpPacket[idx], "';</script>", 11);
    idx += 11;

    while (httpPacket[idx] != '\x00')
    {
        httpPacket[idx++] = '\x00';
    }
}

BOOLEAN CopyNetBufferLists(_In_ PNET_BUFFER_LIST netBufferLists, _Outptr_ PNET_BUFFER_LIST* outNetBufferList)
{
    BOOLEAN result = FALSE;

    if (netBufferLists)
    {
        *outNetBufferList = NdisAllocateCloneNetBufferList(netBufferLists, NULL, NULL, NDIS_CLONE_FLAGS_USE_ORIGINAL_MDLS);

        if (*outNetBufferList)
        {
            NdisSetNblFlag(*outNetBufferList, 0x10000000);

            result = TRUE;
        }
    }

    return result;
}

VOID FreeNetBufferLists(_In_ PNET_BUFFER_LIST netBufferList)
{
    NdisFreeCloneNetBufferList(netBufferList, NDIS_CLONE_FLAGS_USE_ORIGINAL_MDLS);
}

BOOLEAN IsTcpPacket(_In_ struct ETH* eth, _In_ USHORT dstPort, _Out_ USHORT* srcPort)
{
    BOOLEAN result = FALSE;
    struct IP* ip = NULL;
    struct TCP* tcp = NULL;

    if (0x0800 == ntohs(eth->type)) // IPv4
    {
        ip = (struct IP*)((PUCHAR)eth + sizeof(struct ETH));
        tcp = (struct TCP*)((PUCHAR)ip + ip->ip_hl * 4);
        *srcPort = ntohs(tcp->th_sport);
        if (ntohs(tcp->th_dport) == dstPort)
        {
            result = TRUE;
        }
    }

    return result;
}

USHORT ntohs(_In_ USHORT networkUShort)
{
    return (((networkUShort & 0xff) << 8) + ((networkUShort >> 8) & 0xff));
}