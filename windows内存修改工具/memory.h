

/*宏定义部分*/
#define MYCOMMITED MEM_COMMIT
#define MYFREE MEM_FREE
#define MYRESERVED MEM_RESERVE
#define MYIMAGE MEM_IMAGE
#define MYMAPPED MEM_MAPPED
#define MYPRIVATE MEM_PRIVATE
#define MYRO PAGE_READONLY
#define MYRW PAGE_READWRITE
#define MYE PAGE_EXECUTE
#define MYER PAGE_EXECUTE_READ
#define MYERW PAGE_EXECUTE_READWRITE

/*API错误类型*/
#define NO_ERROR_IN_OSCE 0x0 //无错误
#define IHV_IN_GPM 0x1 //获取进程内存块信息时传入了无效的进程句柄
#define ATTACH_TO_PROCESS_FAIL 0x2 //打开进程失败
#define inherit_last_error_in_alloc 0x3 //分配虚存失败，通过系统getlasterror获取更多信息
#define inherit_last_error_in_free 0x4 //释放虚存失败，通过系统getlasterror获取更多信息
#define PageSizeErrorIn_S_MEM_BLOCK 0x5 //页面大小不是合法值，请使用windows api获取pagesize
#define RPM_FAIL 0x6 //readprocessmemory失败
#define StringAppendZeroNUM_TOOBIG 0x7 //字符串追加结尾过长，要求追加后总长度小于4096
#define WriteDataFalse 0x8
#define NULL_POINTER 0x9
#define NextScan_READFAIL 0xa
#define WrongSearchType 0xb
#define INVALID_HANDLE 0xc
#define FAIL_TO_CHANGE_PAGE_INF 0xd//修改页面属性失败，即VirtualProtectEx失败

/*支持的搜索类型*/
#define INT_T 0x100
#define DOUBLE_T 0x200
#define FLOAT_T 0x400
#define STRING_T 0x800

/*读写缓冲区大小*/
#define READ_BUFFER_SIZE 4096

/*包含文件部分*/
#include <iostream>
#include <windows.h>
#include <stdlib.h>
#include <Psapi.h>
#include <TlHelp32.h>
#include <shlwapi.h>
#include <vector>
#include <math.h>
#include <string>
#include <Windows.h>
#include <stdio.h>
#include <wchar.h>
#include <stddef.h> 
#include <tchar.h>
using std::string;
using std::vector;

/*数据结构*/
typedef struct search_result_i{
	LPCVOID baseAddress;
	DWORD offset;
	int i_val;
}search_i_list;
typedef struct search_result_d{
	LPCVOID baseAddress;
	DWORD offset;
	double d_val;
}search_d_list;
typedef struct search_result_f{
	LPCVOID baseAddress;
	DWORD offset;
	float f_val;
}search_f_list;
typedef struct search_result_s{
	LPCVOID baseAddress;
	DWORD offset;
	string s_val;
}search_s_list;
typedef struct search_info{
	DWORD s_type;
	vector <struct search_result_i> i_list;
	vector <struct search_result_d> d_list;
	vector <struct search_result_f> f_list;
	vector <struct search_result_s> s_list;
	int i_exp;
	double d_exp;
	float f_exp;
	string s_exp;
}search_info;






/*导出函数声明*/
DWORD  WINAPI getSystemPageSize();
string  WINAPI getSystemInfo_toString();
string  WINAPI getPerformanceInfo_toString();
string  WINAPI processInfoToString(PROCESSENTRY32 pe, PROCESS_MEMORY_COUNTERS *pmc);
string  WINAPI getProcessList(vector<PROCESS_MEMORY_COUNTERS>& vct_pmc, vector<PROCESSENTRY32>& vct_pe32);
string  WINAPI getProtectionInfoString(DWORD dwTarget);
BOOL WINAPI EnablePrivileges();
string  WINAPI memBlockInfoToString(MEMORY_BASIC_INFORMATION memBasicInfo, HANDLE handle);
BOOL WINAPI search_in_buffer(char * buffer, LPCVOID baseAddress, search_info& v_s_rslt, char *expval, int *surplus);
BOOL WINAPI search_in_buffer(char * buffer, LPCVOID baseAddress, search_info& v_s_rslt, int *expval);
BOOL WINAPI search_in_buffer(char * buffer, LPCVOID baseAddress, search_info& v_s_rslt, float *expval);
BOOL WINAPI search_in_buffer(char * buffer, LPCVOID baseAddress, search_info& v_s_rslt, double *expval);
BOOL WINAPI search_one_mem_block(MEMORY_BASIC_INFORMATION memBasicInfo, search_info& v_s_rslt, HANDLE handle, DWORD PageSize);
BOOL WINAPI searchAll(vector<MEMORY_BASIC_INFORMATION> memInfo, search_info &v_s_rslt, HANDLE handle, DWORD s_type, DWORD pagesize, char* charexpval, int *intexpval, float *floatexpval, double *doubleexpval);
BOOL WINAPI attachToProcess(DWORD Pid, HANDLE & hProcess);
BOOL WINAPI getProcessMemBlockEX(HANDLE hProcess, vector<MEMORY_BASIC_INFORMATION>& memInfo, DWORD stateFilter, DWORD typeFilter, DWORD protectFilter, DWORD* memSize);
LPVOID WINAPI AllocVirtual(int size, HANDLE handle);
BOOL WINAPI FreeVirtual(LPVOID Buffer, HANDLE handle);
BOOL WINAPI nextScan(HANDLE hProcess, search_info & v_s_rslt, LPVOID expected_val);
BOOL WINAPI writeData(HANDLE hProcess, LPVOID address, void * write_val, DWORD valType, int stringAppendZeroNUM, char endSet);
void mySetLastError(DWORD e);
DWORD myGetLastError();
void init_for_mem_manage();
void check_mem_align_scheme();

/*
只支持在单页内查找，最长4096bytes的字符串
*/

