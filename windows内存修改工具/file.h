#pragma once

BOOL WriteFileWithFileMappingObject(void * address, size_t length, TCHAR * filename);

void * ReadFileWithFileMappingObject(TCHAR * filename);

void WriteFileWithAsync(void * address, size_t length, TCHAR * filename, TCHAR * tips);

void * ReadFileWithAsnc(TCHAR * filename, TCHAR * tips);

