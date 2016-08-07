#pragma once

NTSTATUS CreateDeleteUrlListThread();

VOID DeleteUrlListThreadProc(_In_ PVOID param);

VOID TerminateDeleteUrlListThread();