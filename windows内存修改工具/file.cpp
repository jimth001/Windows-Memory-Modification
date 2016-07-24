#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include "file.h"

#define STRING_MAX_LENGTH 2000

struct OverlappedWithTips
{
    OVERLAPPED overlapped;
    TCHAR tips[STRING_MAX_LENGTH];
    void *tmp;
};

BOOL WriteFileWithFileMappingObject(void *address, size_t length, TCHAR* filename)
{
    BOOL returnValue = TRUE;

    HANDLE hFile = CreateFile(filename,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        _tprintf(_T("Create file failed with error: %d\n"), GetLastError());
        returnValue = FALSE;
        goto ErrorHandle;
    }

    DWORD lLength = length & 0xFFFFFFFF;

#if _WIN64
    DWORD hLength = length >> 32;
#else
    DWORD hLength = 0;
#endif

    HANDLE hMapping = CreateFileMapping(hFile,
        NULL,
        PAGE_READWRITE,
        hLength,
        lLength,
        NULL);
    if (hMapping == NULL)
    {
        _tprintf(_T("Create file mapping object failed with error: %d\n"), GetLastError());
        returnValue = FALSE;
        goto ErrorHandle;
    }

    void *mem = MapViewOfFile(hMapping,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        0);
    if (mem == NULL)
    {
        _tprintf(_T("Map view of file failed with error: %d\n"), GetLastError());
        returnValue = FALSE;
        goto ErrorHandle;
    }

    memcpy(mem, address, length);


ErrorHandle:
    if (hFile)
    {
        CloseHandle(hFile);
        if (hMapping)
        {
            CloseHandle(hMapping);
            if (mem)
            {
                UnmapViewOfFile(mem);
            }
        }
    }

    return returnValue;
}

void* ReadFileWithFileMappingObject(TCHAR* filename)
{
    void* returnValue = NULL;

    HANDLE hFile = CreateFile(filename,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        _tprintf(_T("Open file failed with error: %d\n"), GetLastError());
        goto ErrorHandle;
    }

    DWORD lLength, hLength;
    lLength = GetFileSize(hFile, &hLength);

    HANDLE hMapping = CreateFileMapping(hFile,
        NULL,
        PAGE_READONLY,
        hLength,
        lLength,
        NULL);
    if (hMapping == NULL)
    {
        _tprintf(_T("Create file mapping object failed with error: %d\n"), GetLastError());
        goto ErrorHandle;
    }

    void *mem = MapViewOfFile(hMapping,
        FILE_MAP_READ,
        0,
        0,
        0);
    if (mem == NULL)
    {
        _tprintf(_T("Map view of file failed with error: %d\n"), GetLastError());
        goto ErrorHandle;
    }

#if _WIN64
    size_t size = (hLength << 32) | lLength;
#else
    size_t size = lLength;
#endif

    returnValue = malloc(size);
    if (returnValue == NULL)
    {
        _tprintf(_T("Malloc failed with error.\n"));
        goto ErrorHandle;
    }
    memcpy(returnValue, mem, size);


ErrorHandle:
    if (hFile)
    {
        CloseHandle(hFile);
        if (hMapping)
        {
            CloseHandle(hMapping);
            if (mem)
            {
                UnmapViewOfFile(mem);
            }
        }
    }

    return returnValue;
}

VOID WINAPI CompletedRoutine(DWORD dwErr, DWORD cbBytesWrite,
    LPOVERLAPPED lpOverLap)
{
    _tprintf(_T("%s"), ((struct OverlappedWithTips*)lpOverLap)->tips);
    if (((struct OverlappedWithTips*)lpOverLap)->tmp != NULL)
    {
        free(((struct OverlappedWithTips*)lpOverLap)->tmp);
    }
    free(lpOverLap);
}

void WriteFileWithAsync(void *address, size_t length, TCHAR* filename, TCHAR* tips)
{
    HANDLE hFile = CreateFile(filename,
        GENERIC_WRITE,
        0,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
        NULL);
    //_tprintf(_T("%d %d\n"), GetFileSize(hFile, NULL), length);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        _tprintf(_T("Create file failed with error: %d\n"), GetLastError());
        goto ErrorHandle;
    }
    //if (!SetFileValidData(hFile, length))
    //{
    //    _tprintf(_T("error"));
    //}

    void* buffer = malloc(length);
    if (buffer == NULL)
    {
        _tprintf(_T("Malloc failed with error.\n"));
        goto ErrorHandle;
    }
    memcpy(buffer, address, length);

    struct OverlappedWithTips * owt = (struct OverlappedWithTips*)malloc(sizeof(struct OverlappedWithTips));
    ZeroMemory(owt, sizeof(struct OverlappedWithTips));
    _tcscpy(owt->tips, tips);
    owt->tmp = buffer;

    BOOL bResult = WriteFileEx(hFile,
        buffer,
        length,
        (LPOVERLAPPED)owt,
        CompletedRoutine);

    //_tprintf(_T("End: %d\n"), GetLastError());

ErrorHandle:
    if (hFile)
    {
        CloseHandle(hFile);
    }
}

void* ReadFileWithAsnc(TCHAR* filename, TCHAR* tips)
{
    void* returnValue = NULL;

    HANDLE hFile = CreateFile(filename,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
        NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        _tprintf(_T("Open file failed with error: %d\n"), GetLastError());
        goto ErrorHandle;
    }

    DWORD lLength, hLength;
    lLength = GetFileSize(hFile, &hLength);
#if _WIN64
    size_t size = (hLength << 32) | lLength;
#else
    size_t size = lLength;
#endif

    returnValue = malloc(size);
    if (returnValue == NULL)
    {
        _tprintf(_T("Malloc failed with error.\n"));
        goto ErrorHandle;
    }

    struct OverlappedWithTips * owt = (struct OverlappedWithTips *)malloc(sizeof(struct OverlappedWithTips));
    ZeroMemory(owt, sizeof(struct OverlappedWithTips));
    _tcscpy(owt->tips, tips);
    owt->tmp = NULL;

    BOOL bResult = ReadFileEx(hFile,
        returnValue,
        size,
        (LPOVERLAPPED)owt,
        CompletedRoutine);
    if (bResult != TRUE)
    {
        _tprintf(_T("WriteFileEx failed with error: %d\n"), GetLastError());
    }

ErrorHandle:
    if (hFile)
    {
        CloseHandle(hFile);
    }

    return returnValue;
}