#define DLL
#include "memory.h"
/*全局变量*/
DWORD my_error = 0;
BOOL is_xd_machine = TRUE;//默认小端机器

/*检查机器内存对齐类型*/
void check_mem_align_scheme()
{
	//
	short x;
	char x0;
	x = 0x1122;
	x0 = ((char*)&x)[0]; //低地址单元
	if (x0 == 0x11)
		is_xd_machine = FALSE;
	else is_xd_machine = TRUE;
}
/*必要的初始化函数，必须先初始化，目前只有检查对齐类型操作*/
void init_for_mem_manage()
{
	check_mem_align_scheme();
}
/*内部的error code函数，通过此获取错误类型，头文件有错误类型定义*/
DWORD myGetLastError()
{
	return my_error;
}
/**/
void mySetLastError(DWORD e)
{
	my_error = e;
}

DWORD  WINAPI getSystemPageSize()
{
	SYSTEM_INFO system_info;
	GetSystemInfo(&system_info);
	return system_info.dwPageSize;
}
string  WINAPI getSystemInfo_toString()
{
	SYSTEM_INFO system_info;
	GetSystemInfo(&system_info);
	string strInfo = "系统信息：\r\n";
	string strTemp = "";
	// OemId
	char trans[MAX_PATH] = { 0 };
	sprintf_s(trans, "OemId : %u\r\n", system_info.dwOemId);
	strTemp = trans;
	strInfo += strTemp;
	// 处理器架构
	sprintf_s(trans, "处理器架构 : %u\r\n", system_info.wProcessorArchitecture);
	strTemp = trans;
	strInfo += strTemp;
	// 页面大小
	sprintf_s(trans, "页面大小 : %u\r\n", system_info.dwPageSize);
	strTemp = trans;
	strInfo += strTemp;
	// 应用程序最小地址
	sprintf_s(trans, "应用程序最小地址 : %p\r\n", system_info.lpMinimumApplicationAddress);
	strTemp = trans;
	strInfo += strTemp;
	// 应用程序最大地址
	sprintf_s(trans, "应用程序最大地址 : %p\r\n", system_info.lpMaximumApplicationAddress);
	strTemp = trans;
	strInfo += strTemp;
	// 处理器掩码
	sprintf_s(trans, "处理器掩码 : %u\r\n", system_info.dwActiveProcessorMask);
	strTemp = trans;
	strInfo += strTemp;
	// 处理器数量
	sprintf_s(trans, "处理器数量 : %u\r\n", system_info.dwNumberOfProcessors);
	strTemp = trans;
	strInfo += strTemp;
	// 处理器类型
	sprintf_s(trans, "处理器类型 : %u\r\n", system_info.dwProcessorType);
	strTemp = trans;
	strInfo += strTemp;
	// 虚拟内存分配粒度
	sprintf_s(trans, "虚拟内存分配粒度 : %u\r\n", system_info.dwAllocationGranularity);
	strTemp = trans;
	strInfo += strTemp;
	// 处理器级别
	sprintf_s(trans, "处理器级别 : %u\r\n", system_info.wProcessorLevel);
	strTemp = trans;
	strInfo += strTemp;
	// 处理器版本
	sprintf_s(trans, "处理器版本 : %u\r\n", system_info.wProcessorRevision);
	strTemp = trans;
	strInfo += strTemp;
	return strInfo;
}
string  WINAPI getPerformanceInfo_toString()
{
	PERFORMANCE_INFORMATION pi;
	pi.cb = sizeof(pi);
	GetPerformanceInfo(&pi, sizeof(pi));
	string pInfo = "";
	/*typedef struct _PERFORMANCE_INFORMATION {
	DWORD  cb;
	SIZE_T CommitTotal;
	SIZE_T CommitLimit;
	SIZE_T CommitPeak;
	SIZE_T PhysicalTotal;
	SIZE_T PhysicalAvailable;
	SIZE_T SystemCache;
	SIZE_T KernelTotal;
	SIZE_T KernelPaged;
	SIZE_T KernelNonpaged;
	SIZE_T PageSize;
	DWORD  HandleCount;
	DWORD  ProcessCount;
	DWORD  ThreadCount;
	} PERFORMANCE_INFORMATION, *PPERFORMANCE_INFORMATION;*/
	char trans[MAX_PATH] = { 0 };
	string sTmp;
	sprintf_s(trans, "系统当前提交的页面总数 : %u\r\n", pi.CommitTotal);
	sTmp = trans;
	pInfo += trans;
	sprintf_s(trans, "系统可提交的页面总数 : %u\r\n", pi.CommitLimit);
	sTmp = trans;
	pInfo += trans;
	sprintf_s(trans, "系统历史提交页面峰值 : %u\r\n", pi.CommitPeak);
	sTmp = trans;
	pInfo += trans;
	sprintf_s(trans, "按页分配的总的物理内存 : %u\r\n", pi.PhysicalTotal);
	sTmp = trans;
	pInfo += trans;
	sprintf_s(trans, "系统当前提交的页面总数 : %u\r\n", pi.PhysicalAvailable);
	sTmp = trans;
	pInfo += trans;
	return pInfo;
}

string  WINAPI processInfoToString(PROCESSENTRY32 pe, PROCESS_MEMORY_COUNTERS *pmc)
{
	char trans[MAX_PATH] = { 0 };
	string ID, Name, VMemory, cntThreads, th32ParentProcessID;
	sprintf_s(trans, "进程ID： %u\r\n", pe.th32ProcessID);
	ID = trans;
	sprintf_s(trans, "父进程ID： %u\r\n", pe.th32ParentProcessID);
	th32ParentProcessID = trans;
	sprintf_s(trans, "进程名称： %s\r\n", pe.szExeFile);//此处待测验%u->%s
	Name = trans;
	sprintf_s(trans, "线程数： %u\r\n", pe.cntThreads);
	cntThreads = trans;
	if (pmc != NULL) sprintf_s(trans, "虚存大小： %u KB\r\n", pmc->WorkingSetSize / 1024);
	VMemory = trans;
	return ID + th32ParentProcessID + Name + cntThreads + VMemory;
}
/*
获取进程列表
*/
string  WINAPI getProcessList(vector<PROCESS_MEMORY_COUNTERS>& vct_pmc, vector<PROCESSENTRY32>& vct_pe32)
{
	/*
	typedef struct tagPROCESSENTRY32 {
	DWORD     dwSize;//该结构的大小，使用前要初始化，否则Process32First将失败
	DWORD     cntUsage;//不再使用的值
	DWORD     th32ProcessID;//process id
	ULONG_PTR th32DefaultHeapID;//不再使用的值
	DWORD     th32ModuleID;//不再使用的值
	DWORD     cntThreads;//线程数
	DWORD     th32ParentProcessID;//父进程id
	LONG      pcPriClassBase;//线程的基础优先权
	DWORD     dwFlags;//不再使用的值
	TCHAR     szExeFile[MAX_PATH];//
	} PROCESSENTRY32, *PPROCESSENTRY32;
	*/
	/*
	typedef struct _PROCESS_MEMORY_COUNTERS {
	DWORD  cb;
	DWORD  PageFaultCount;//The number of page faults.
	SIZE_T PeakWorkingSetSize;//虚存大小峰值
	SIZE_T WorkingSetSize;//虚存大小
	SIZE_T QuotaPeakPagedPoolUsage;//分页池使用的峰值
	SIZE_T QuotaPagedPoolUsage;//当前使用的分页池
	SIZE_T QuotaPeakNonPagedPoolUsage;//非分页池使用的峰值
	SIZE_T QuotaNonPagedPoolUsage;//
	SIZE_T PagefileUsage;//提交的内存总量 in bytes
	SIZE_T PeakPagefileUsage;//PagefileUsage的峰值
	} PROCESS_MEMORY_COUNTERS, *PPROCESS_MEMORY_COUNTERS;
	*/
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(pe);
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	BOOL bMore = Process32First(hProcessSnap, &pe);
	PROCESS_MEMORY_COUNTERS pmc;
	string pceInfo = "";
	while (bMore)
	{
		HANDLE hP = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
		ZeroMemory(&pmc, sizeof(pmc));
		if (GetProcessMemoryInfo(hP, &pmc, sizeof(pmc)) == TRUE)
		{
			vct_pe32.push_back(pe);
			vct_pmc.push_back(pmc);
		}
		pceInfo = pceInfo + processInfoToString(pe, &pmc);
		bMore = Process32Next(hProcessSnap, &pe);
		CloseHandle(hP);
	}
	CloseHandle(hProcessSnap);
	return pceInfo;
}
string  WINAPI getProtectionInfoString(DWORD dwTarget)
{
	string rt = "";
	if (dwTarget == PAGE_READONLY)
	{
		rt = "READONLY";
	}

	if (dwTarget == PAGE_GUARD)
	{
		rt = "GUARD";
	}

	if (dwTarget == PAGE_NOCACHE)
	{
		rt = "NOCACHE";
	}

	if (dwTarget == PAGE_READWRITE)
	{
		rt = "READWRITE";
	}

	if (dwTarget == PAGE_WRITECOPY)
	{
		rt = "WRITECOPY";
	}

	if (dwTarget == PAGE_EXECUTE)
	{
		rt = "EXECUTE";
	}

	if (dwTarget == PAGE_EXECUTE_READ)
	{
		rt = "EXECUTE_READ";
	}

	if (dwTarget == PAGE_EXECUTE_READWRITE)
	{
		rt = "EXECUTE_READWRITE";
	}

	if (dwTarget == PAGE_EXECUTE_WRITECOPY)
	{
		rt = "EXECUTE_WRITECOPY";
	}
	return rt;
}
BOOL WINAPI EnablePrivileges()
{
	//提权：
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		return(FALSE);
	LookupPrivilegeValue(NULL, SE_DEBUG_NAME,
		&tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
		(PTOKEN_PRIVILEGES)NULL, 0);
	CloseHandle(hToken);
	if (GetLastError() != ERROR_SUCCESS)
		return FALSE;
	return TRUE;
}
string  WINAPI memBlockInfoToString(MEMORY_BASIC_INFORMATION memBasicInfo, HANDLE handle)
{
	if (handle == INVALID_HANDLE_VALUE)
	{
		mySetLastError(INVALID_HANDLE);
		return FALSE;
	}
	string Info = "";
	char trans[MAX_PATH];
	char szSize[MAX_PATH];
	LPCVOID pEnd = (PBYTE)memBasicInfo.BaseAddress + memBasicInfo.RegionSize;
	sprintf_s(szSize, "%u", memBasicInfo.RegionSize);
	//显示块基地址和长度
	string Start, End;
	const LONG64 S = (LONG64)memBasicInfo.BaseAddress, E = (LONG64)pEnd;
	sprintf_s(trans, "%llx", S);
	Start = trans;
	sprintf_s(trans, "%llx", E);
	End = trans;
	while (Start.length() < 16)
		Start = "0" + Start;
	while (End.length() < 16)
		End = "0" + End;
	Info = Info + Start + "-" + End + " ( " + szSize + " BYTES )";
	//显示块的状态
	switch (memBasicInfo.State)
	{
	case MEM_COMMIT:
		Info += " , committed";
		break;
	case MEM_FREE:
		Info += " , free";
		break;
	case MEM_RESERVE:
		Info += " , reserved";
		break;
	}
	//显示保护
	if (memBasicInfo.Protect == 0 && memBasicInfo.State != MEM_FREE)
	{
		memBasicInfo.Protect = PAGE_READONLY;
	}
	Info = Info + " , " + getProtectionInfoString(memBasicInfo.Protect);//
	//显示类型
	switch (memBasicInfo.Type)
	{
	case MEM_IMAGE:
		Info += " , Image";
		break;
	case MEM_MAPPED:
		Info += " , Mapped";
		break;
	case MEM_PRIVATE:
		Info += " , Private";
		break;
	}
	//检测可执行的映像
	char szFilename[MAX_PATH];
	if (GetModuleFileNameEx(handle, (HMODULE)memBasicInfo.BaseAddress, szFilename, MAX_PATH) > 0)
	{
		//my_PathStripPath(szFilename);//自己实现的PathStripPath //Obsolete opt,现在显示完整的路径
		Info += " , Module: ";
		Info += szFilename;
	}
	Info += "\r\n";
	return Info;
}
BOOL WINAPI search_in_buffer(unsigned char * buffer, LPCVOID baseAddress, search_info& v_s_rslt, char *expval, int *surplus)
{
	*surplus = sizeof(expval);
	int i = 0, j = 0;
	int ri, rs = *surplus;//record i,surplus
	search_s_list tmp_s_rslt;
	while (i < READ_BUFFER_SIZE&&*surplus>0)
	{
		ri = i;
		while (i < READ_BUFFER_SIZE&&*surplus>0)
		{
			if (buffer[i] == expval[j])
			{
				i++;
				j++;
				(*surplus)--;
			}
			else{
				break;
			}
		}
		if ((*surplus) == 0)
		{
			tmp_s_rslt.baseAddress = baseAddress;
			tmp_s_rslt.offset = ri;
			tmp_s_rslt.s_val = expval;
			v_s_rslt.s_list.push_back(tmp_s_rslt);
		}
		i = ri + 1;
		j = 0;
		*surplus = rs;
	}
	return TRUE;
}
BOOL WINAPI search_in_buffer(unsigned char * buffer, LPCVOID baseAddress, search_info& v_s_rslt, int *expval)
{
	int t = 0;
	int i = 0;
	search_i_list s;
	int msize = sizeof(int);
	while (i < READ_BUFFER_SIZE)
	{
		t = 0;
		for (int j = 0; j < msize; j++)
		{
			if (is_xd_machine == FALSE)
			{
				int tmp = 0;
				tmp = tmp | buffer[i + j];
				t = (tmp << (msize - j - 1) * 8) | t;
			}
			else
			{
				int tmp = 0;//
				tmp = tmp | buffer[i + j];
				int tt = (tmp << (8 * j));
				t = tt | t;
			}
		}
		if (t == *expval)
		{
			s.baseAddress = baseAddress;
			s.offset = i;
			s.i_val = t;
			v_s_rslt.i_list.push_back(s);
		}
		if (i > 2560)
		{
			int qq = i;
			qq++;
		}
		i += msize;
	}
	return TRUE;
}
BOOL WINAPI search_in_buffer(unsigned char * buffer, LPCVOID baseAddress, search_info& v_s_rslt, float *expval)
{
	float f = 0;
	int *t = (int *)&f;
	int i = 0;
	search_f_list s;
	int msize = sizeof(float);
	while (i < READ_BUFFER_SIZE)
	{
		memset(&f, 0, sizeof(f));
		for (int j = 0; j < msize; j++)
		{
			if (is_xd_machine == FALSE)
			{
				int tmp = 0;
				tmp = tmp | (unsigned int)buffer[i + j];
				*t = (tmp << (msize - j - 1) * 8) | *t;
			}
			else
			{
				int tmp = 0;
				tmp = tmp | (unsigned int)buffer[i + j];
				int tt = (tmp << (8 * j));
				*t = tt | *t;
			}
		}
		if (fabs(f - *expval)<1)
		{
			s.baseAddress = baseAddress;
			s.offset = i;
			s.f_val = f;
			v_s_rslt.f_list.push_back(s);
		}
		if (i >= 3460)
		{
			int debug = 0;
		}
		i += msize;
	}
	return TRUE;
}
BOOL WINAPI search_in_buffer(unsigned char * buffer, LPCVOID baseAddress, search_info& v_s_rslt, double *expval)
{
	double f = 0;
	LONG64 *t = (LONG64 *)&f;
	int i = 0;
	search_d_list s;
	int msize = sizeof(double);
	while (i < READ_BUFFER_SIZE)
	{
		memset(&f, 0, sizeof(f));
		for (int j = 0; j < msize; j++)
		{
			if (is_xd_machine == FALSE)
			{
				LONG64 tmp = 0;
				tmp = tmp | (unsigned int)buffer[i + j];
				*t = (tmp << (msize - j - 1) * 8) | *t;
			}
			else
			{
				LONG64 tmp = 0;
				tmp = tmp | (unsigned int)buffer[i + j];
				*t = (tmp << (8 * j)) | *t;

			}
		}
		if (fabs(f - *expval)<1)
		{
			s.baseAddress = baseAddress;
			s.offset = i;
			s.d_val = f;
			v_s_rslt.d_list.push_back(s);
		}
		i += msize;
	}
	return TRUE;
}
BOOL WINAPI search_one_mem_block(MEMORY_BASIC_INFORMATION memBasicInfo, search_info& v_s_rslt, HANDLE handle, DWORD PageSize)
{
	if (handle == INVALID_HANDLE_VALUE)
	{
		mySetLastError(INVALID_HANDLE);
		return FALSE;
	}
	int i;
	int block_num = memBasicInfo.RegionSize / PageSize;
	if (memBasicInfo.RegionSize%PageSize != 0)
	{
		mySetLastError(PageSizeErrorIn_S_MEM_BLOCK);
		return FALSE;
	}
	SIZE_T readnum = 0;
	unsigned char readBuffer[READ_BUFFER_SIZE] = { 0 };
	DWORD dwOldProt, dwNewProt;
	if (!VirtualProtectEx(handle, (void*)memBasicInfo.BaseAddress, memBasicInfo.RegionSize, PAGE_READWRITE, &dwOldProt) && GetLastError() != 87)
	{
		mySetLastError(FAIL_TO_CHANGE_PAGE_INF);
		return FALSE;
	}
	for (i = 0; i < block_num; i++)
	{
		LPCVOID pBase = (LPCVOID)((unsigned char*)memBasicInfo.BaseAddress + PageSize*i);
		if (!ReadProcessMemory(handle, pBase, readBuffer, (SIZE_T)PageSize, &readnum))
		{
			mySetLastError(RPM_FAIL);
			return FALSE;

		}
		int sur = 0;
		switch (v_s_rslt.s_type)
		{
		case INT_T:search_in_buffer(readBuffer, pBase, v_s_rslt, &v_s_rslt.i_exp); break;
		case FLOAT_T:search_in_buffer(readBuffer, pBase, v_s_rslt, &v_s_rslt.f_exp); break;
		case DOUBLE_T:search_in_buffer(readBuffer, pBase, v_s_rslt, &v_s_rslt.d_exp); break;
		case STRING_T:search_in_buffer(readBuffer, pBase, v_s_rslt, (char *)v_s_rslt.s_exp.c_str(), &sur); break;
		default:
			break;
		}
	}
	if (!VirtualProtectEx(handle, (void*)memBasicInfo.BaseAddress, memBasicInfo.RegionSize, dwOldProt, &dwNewProt) && GetLastError() != 87)
	{
		mySetLastError(FAIL_TO_CHANGE_PAGE_INF);
		return FALSE;
	}
	return TRUE;
}
/*
首次搜索
vector<MEMORY_BASIC_INFORMATION> memInfo,筛选出的待搜索块
search_info &v_s_rslt,保存搜索结果的数据结构
HANDLE handle,进程句柄
DWORD s_type,搜索类型，支持FLOAT_T INT_T DOUBLE_T STRING_T
DWORD pagesize,系统页面大小
char* charexpval,要搜索的STRING_T值
int *intexpval,要搜索的INT_T值
float *floatexpval,要搜索的FLOAT_T值
double *doubleexpval要搜索的DOUBLE_T值
*/
BOOL WINAPI searchAll(vector<MEMORY_BASIC_INFORMATION> memInfo, search_info &v_s_rslt, HANDLE handle, DWORD s_type, DWORD pagesize, char* charexpval, int *intexpval, float *floatexpval, double *doubleexpval)
{
	if (handle == INVALID_HANDLE_VALUE)
	{
		mySetLastError(INVALID_HANDLE);
		return FALSE;
	}
	v_s_rslt.s_type = s_type;
	switch (s_type)
	{
	case INT_T:v_s_rslt.i_exp = *intexpval; break;
	case FLOAT_T:v_s_rslt.f_exp = *floatexpval; break;
	case DOUBLE_T:v_s_rslt.d_exp = *doubleexpval; break;
	case STRING_T:v_s_rslt.s_exp = charexpval; break;
	default:
		break;
	}
	int block_num = memInfo.size();
	int i;
	for (i = 0; i < block_num; i++)
	{
		if (search_one_mem_block(memInfo[i], v_s_rslt, handle, pagesize) == FALSE)
		{
			return FALSE;
		}
	}
	return TRUE;
}
/*
DWORD IN pid,要打开的进程
DWORD OUT hProcess成功后返回的进程句柄
*/
BOOL WINAPI attachToProcess(DWORD Pid, HANDLE & hProcess)
{
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, Pid);
	if (hProcess == NULL)
	{
		mySetLastError(ATTACH_TO_PROCESS_FAIL);
		return FALSE;
	}
	return TRUE;
}
/*
按属性筛选要搜索的内存块
HANDLE IN hProcess 进程句柄
vector OUT memInfo 保存内存块信息的数据结构 vector<MEMORY_BASIC_INFORMATION>
DWORD IN stateFilter 按state类型过滤内存块
DWORD IN typeFilter  按type类型过滤内存块
DWORD IN protectFilter  按protect类型过滤内存块
*/
BOOL WINAPI getProcessMemBlockEX(HANDLE hProcess, vector<MEMORY_BASIC_INFORMATION>& memInfo, DWORD stateFilter, DWORD typeFilter, DWORD protectFilter, DWORD* memSize)
{
	/*
	typedef struct _MEMORY_BASIC_INFORMATION {
	PVOID  BaseAddress;//一个指向该区域基地址的指针。/查询内存块所占的第一个页面基地址
	PVOID  AllocationBase;//内存块所占的第一块区域基地址，小于等于BaseAddress，
	//也就是说BaseAddress一定包含在AllocationBase分配的范围内
	DWORD  AllocationProtect;//区域被初次保留时赋予的保护属性
	SIZE_T RegionSize;//从BaseAddress开始，具有相同属性的页面的大小  这个概念很重要。读取其他进程地址空间时，可以按页的大小读取
	DWORD  State;//页面的状态，有三种可能值：MEM_COMMIT、MEM_FREE和MEM_RESERVE，
	//这个参数对我们来说是最重要的了，从中我们便可知指定内存页面的状态了
	//committed:Indicates committed pages for which physical storage has been allocated, either in memory or in the paging file on disk.
	//已在物理介质上存在
	//free：空闲的，不可访问，可分配的。
	//reserved：预留的
	DWORD  Protect;//页面的属性，其可能的取值与AllocationProtect相同
	DWORD  Type;//该内存块的类型，有三种可能值：MEM_IMAGE、MEM_MAPPED和MEM_PRIVATE
	} MEMORY_BASIC_INFORMATION, *PMEMORY_BASIC_INFORMATION;
	*/
	/*
	typedef struct _SYSTEM_INFO {
	union {
	DWORD dwOemId;          // Obsolete field...do not use
	struct {
	WORD wProcessorArchitecture;
	WORD wReserved;
	} DUMMYSTRUCTNAME;
	} DUMMYUNIONNAME;
	DWORD dwPageSize;
	LPVOID lpMinimumApplicationAddress;
	LPVOID lpMaximumApplicationAddress;
	DWORD_PTR dwActiveProcessorMask;
	DWORD dwNumberOfProcessors;
	DWORD dwProcessorType;
	DWORD dwAllocationGranularity;
	WORD wProcessorLevel;
	WORD wProcessorRevision;
	} SYSTEM_INFO, *LPSYSTEM_INFO;
	*/
	//HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, Pid);
	if (hProcess == INVALID_HANDLE_VALUE){
		mySetLastError(IHV_IN_GPM);
		return FALSE;
	}
	SYSTEM_INFO si;		//系统信息结构
	ZeroMemory(&si, sizeof(si));
	GetSystemInfo(&si);
	MEMORY_BASIC_INFORMATION memBasicInfo;//进程虚拟内存空间的基本信息结构
	ZeroMemory(&memBasicInfo, sizeof(memBasicInfo));
	//循环整个应用程序的地址空间
	LPCVOID pStart = (LPCVOID)si.lpMinimumApplicationAddress;
	if (memSize != NULL) *memSize = 0;
	while (pStart < si.lpMaximumApplicationAddress)
	{
		//获得下一个虚拟内存块的信息
		if (VirtualQueryEx(hProcess, pStart, &memBasicInfo, sizeof(memBasicInfo)) == sizeof(memBasicInfo))
		{
			if (((stateFilter&memBasicInfo.State)>0) && ((typeFilter&memBasicInfo.Type)>0) && ((protectFilter&memBasicInfo.Protect)>0))
			{
				memInfo.push_back(memBasicInfo);
				if (memSize != NULL) *memSize = *memSize + memBasicInfo.RegionSize;
			}
			//计算尾块基地址
			LPCVOID pEnd = (PBYTE)pStart + memBasicInfo.RegionSize;
			//移动块指针以获得下一个模块
			pStart = pEnd;
		}
	}
	return TRUE;
}
/*
int size,申请的虚存大小
HANDLE handle进程句柄
*/
LPVOID WINAPI AllocVirtual(int size, HANDLE handle)
{
	if (handle == INVALID_HANDLE_VALUE)
	{
		mySetLastError(INVALID_HANDLE);
		return FALSE;
	}
	LPVOID Buffer = (LPVOID)VirtualAllocEx(handle, NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (Buffer == NULL)
	{
		mySetLastError(inherit_last_error_in_alloc);
		//cout << "分配失败,getlasterror:" << GetLastError() << endl;
	}
	return Buffer;
}
/*
LPVOID Buffer,释放的虚地址指针
HANDLE handle进程句柄
*/
BOOL WINAPI FreeVirtual(LPVOID Buffer, HANDLE handle)
{
	if (handle == INVALID_HANDLE_VALUE)
	{
		mySetLastError(INVALID_HANDLE);
		return FALSE;
	}
	if (VirtualFreeEx(handle, Buffer, 0, MEM_RELEASE) == 0)
	{
		mySetLastError(inherit_last_error_in_free);
		return FALSE;
	}
	return TRUE;
}
/*
HANDLE hProcess,//进程句柄
search_info & v_s_rslt,//保存上次搜索信息的数据结构
LPVOID expected_val//新的搜索值
*/
BOOL WINAPI nextScan(HANDLE hProcess, search_info & v_s_rslt, LPVOID expected_val)
{
	if (hProcess == INVALID_HANDLE_VALUE)
	{
		mySetLastError(INVALID_HANDLE);
		return FALSE;
	}
	int readsize = 0;
	int listnum = 0;
	SIZE_T real_read_num;
	unsigned char buffer[READ_BUFFER_SIZE];
	vector<search_i_list>::iterator pIIter = v_s_rslt.i_list.begin();;
	vector<search_f_list>::iterator pFIter = v_s_rslt.f_list.begin();;
	vector<search_d_list>::iterator pDIter = v_s_rslt.d_list.begin();;
	vector<search_s_list>::iterator pSIter = v_s_rslt.s_list.begin();;
	switch (v_s_rslt.s_type)
	{
	case INT_T:
		readsize = sizeof(int);
		memset(buffer, 0, sizeof(buffer));
		for (; pIIter != v_s_rslt.i_list.end();)
		{

			DWORD dwOldProt, dwNewProt = 0;
			real_read_num = 0;
			if (!VirtualProtectEx(hProcess, (LPVOID)((unsigned char*)pIIter->baseAddress + pIIter->offset), readsize, PAGE_READWRITE, &dwOldProt) && GetLastError() != 87)
			{
				mySetLastError(FAIL_TO_CHANGE_PAGE_INF);
				return FALSE;
			}

			if (!ReadProcessMemory(hProcess, (LPCVOID)((unsigned char*)pIIter->baseAddress + pIIter->offset), buffer, readsize, &real_read_num))
			{
				mySetLastError(NextScan_READFAIL);
				return FALSE;
			}
			int t = 0;
			for (int j = 0; j < readsize; j++)
			{
				if (is_xd_machine == FALSE)
				{
					int tmp = 0;
					tmp = tmp | buffer[j];
					t = (tmp << (readsize - j - 1) * 8) | t;
				}
				else
				{
					int tmp = 0;//
					tmp = tmp | buffer[j];
					int tt = (tmp << (8 * j));
					t = tt | t;
				}
			}
			if (!VirtualProtectEx(hProcess, (LPVOID)((unsigned char*)pIIter->baseAddress + pIIter->offset), readsize, dwOldProt, &dwNewProt) && GetLastError() != 87)
			{
				mySetLastError(FAIL_TO_CHANGE_PAGE_INF);
				return FALSE;
			}
			if (*(int*)expected_val == t)
			{
				pIIter->i_val = t;
				pIIter++;
			}
			else
			{
				pIIter = v_s_rslt.i_list.erase(pIIter);
			}

		}
		break;
	case FLOAT_T:readsize = sizeof(float);
		memset(buffer, 0, sizeof(buffer));
		for (; pFIter != v_s_rslt.f_list.end();)
		{

			DWORD dwOldProt, dwNewProt = 0;
			if (!VirtualProtectEx(hProcess, (LPVOID)((unsigned char*)pFIter->baseAddress + pFIter->offset), readsize, PAGE_READWRITE, &dwOldProt) && GetLastError() != 87)
			{
				mySetLastError(FAIL_TO_CHANGE_PAGE_INF);
				return FALSE;
			}

			if (!ReadProcessMemory(hProcess, (LPCVOID)((unsigned char*)pFIter->baseAddress + pFIter->offset), buffer, readsize, &real_read_num))
			{
				mySetLastError(NextScan_READFAIL);
				return FALSE;
			}
			float f = 0;
			int *t = (int *)&f;
			for (int j = 0; j < readsize; j++)
			{
				if (is_xd_machine == FALSE)
				{
					int tmp = 0;
					tmp = tmp | buffer[j];
					*t = (tmp << (readsize - j - 1) * 8) | *t;
				}
				else
				{
					int tmp = 0;//
					tmp = tmp | buffer[j];
					int tt = (tmp << (8 * j));
					*t = tt | *t;
				}
			}
			if (!VirtualProtectEx(hProcess, (LPVOID)((unsigned char*)pFIter->baseAddress + pFIter->offset), readsize, dwOldProt, &dwNewProt) && GetLastError() != 87)
			{
				mySetLastError(FAIL_TO_CHANGE_PAGE_INF);
				return FALSE;
			}
			if (fabs(*(float*)expected_val - f)<1)
			{
				pFIter->f_val = f;
				pFIter++;
			}
			else
			{
				pFIter = v_s_rslt.f_list.erase(pFIter);
			}

		}
		break;
	case DOUBLE_T:readsize = sizeof(double);
		memset(buffer, 0, sizeof(buffer));
		for (; pDIter != v_s_rslt.d_list.end();)
		{

			DWORD dwOldProt, dwNewProt = 0;
			if (!VirtualProtectEx(hProcess, (LPVOID)((unsigned char*)pDIter->baseAddress + pDIter->offset), readsize, PAGE_READWRITE, &dwOldProt) && GetLastError() != 87)
			{
				mySetLastError(FAIL_TO_CHANGE_PAGE_INF);
				return FALSE;
			}
			if (ReadProcessMemory(hProcess, (LPCVOID)((unsigned char*)pDIter->baseAddress + pDIter->offset), buffer, readsize, &real_read_num) == FALSE)
			{
				mySetLastError(NextScan_READFAIL);
				return FALSE;
			}
			double f = 0;
			LONG64 *t = (LONG64 *)&f;
			for (int j = 0; j < readsize; j++)
			{
				if (is_xd_machine == FALSE)
				{
					LONG64 tmp = 0;
					tmp = tmp | buffer[j];
					*t = (tmp << (readsize - j - 1) * 8) | *t;
				}
				else
				{
					LONG64 tmp = 0;//
					tmp = tmp | buffer[j];
					LONG64 tt = (tmp << (8 * j));
					*t = tt | *t;
				}
			}
			if (!VirtualProtectEx(hProcess, (LPVOID)((unsigned char*)pDIter->baseAddress + pDIter->offset), readsize, dwOldProt, &dwNewProt) && GetLastError() != 87)
			{
				mySetLastError(FAIL_TO_CHANGE_PAGE_INF);
				return FALSE;
			}
			if (fabs(*(double*)expected_val - f) < 1)
			{
				pDIter->d_val = f;
				pDIter++;
			}
			else
			{
				pDIter = v_s_rslt.d_list.erase(pDIter);
			}

		}
		break;
	case STRING_T:readsize = strlen((char*)expected_val);
		memset(buffer, '0', sizeof(buffer));
		for (; pSIter != v_s_rslt.s_list.end();)
		{

			DWORD dwOldProt, dwNewProt = 0;
			if (!VirtualProtectEx(hProcess, (LPVOID)((unsigned char*)pSIter->baseAddress + pSIter->offset), readsize, PAGE_READWRITE, &dwOldProt) && GetLastError() != 87)
			{
				mySetLastError(FAIL_TO_CHANGE_PAGE_INF);
				return FALSE;
			}
			if (!ReadProcessMemory(hProcess, (LPCVOID)((unsigned char*)pSIter->baseAddress + pSIter->offset), buffer, readsize, &real_read_num))
			{
				mySetLastError(NextScan_READFAIL);
				return FALSE;
			}
			int t = 0;

			if (!VirtualProtectEx(hProcess, (LPVOID)((unsigned char*)pSIter->baseAddress + pSIter->offset), readsize, dwOldProt, &dwNewProt) && GetLastError() != 87)
			{
				mySetLastError(FAIL_TO_CHANGE_PAGE_INF);
				return FALSE;
			}
			if (strcmp((char*)buffer, (char*)expected_val) == 0)
			{
				pSIter->s_val = (char*)expected_val;
				pSIter++;
			}
			else
			{
				pSIter = v_s_rslt.s_list.erase(pSIter);
			}

		}
		break;
	default:
		mySetLastError(WrongSearchType);
		return FALSE;
	}
	return TRUE;
}
/*
HANDLE hProcess,//要写入的进程
LPVOID address,//要写入的地址
void * write_val,//要写入的变量指针，只能是string,int,double,float四种
DWORD valType,//写入的变量类型，可选值见头文件宏定义
int stringAppendZeroNUM,//字符串后追加的字符数量
char endSet//字符串后追加字符，内存中string和char结尾不同
*/
BOOL WINAPI writeData(HANDLE hProcess, LPVOID address, void * write_val, DWORD valType, int stringAppendZeroNUM, char endSet)
{
	if (hProcess == INVALID_HANDLE_VALUE)
	{
		mySetLastError(INVALID_HANDLE);
		return FALSE;
	}
	if (write_val == NULL)
	{
		mySetLastError(NULL_POINTER);
		return FALSE;
	}
	int size = 0;
	SIZE_T real_writenum = 0;
	DWORD dwOldProt, dwNewProt = 0;
	unsigned char buffer[READ_BUFFER_SIZE];
	switch (valType)
	{
	case INT_T:size = sizeof(int);
		if (is_xd_machine == TRUE)
		{
			for (int i = 0; i < size; i++)
			{
				buffer[i] = ((0xff << (i * 8))&(*(unsigned int *)write_val)) >> (i * 8);
			}
		}
		else{
			for (int i = 0; i < size; i++)
			{
				buffer[i] = (((0xff << (8 * size - 1)) >> (i * 8))&(*(unsigned int *)write_val)) >> (size - i - 1);
			}
		}

		if (!VirtualProtectEx(hProcess, address, size, PAGE_READWRITE, &dwOldProt) && GetLastError() != 87)
		{
			mySetLastError(FAIL_TO_CHANGE_PAGE_INF);
			return FALSE;
		}
		while (real_writenum < size)
		{
			if (WriteProcessMemory(hProcess, (LPVOID)((char*)address + real_writenum), buffer + real_writenum, size - real_writenum, &real_writenum) == FALSE)
			{
				mySetLastError(WriteDataFalse);
				return FALSE;
			}
		}
		if (!VirtualProtectEx(hProcess, address, size, dwOldProt, &dwNewProt) && GetLastError() != 87)
		{
			mySetLastError(FAIL_TO_CHANGE_PAGE_INF);
			return FALSE;
		}
		break;
	case FLOAT_T:size = sizeof(float);
		if (is_xd_machine == TRUE)
		{
			for (int i = 0; i < size; i++)
			{
				buffer[i] = ((0xff << (i * 8))&(*(unsigned int *)write_val)) >> (i * 8);
			}
		}
		else{
			for (int i = 0; i < size; i++)
			{
				buffer[i] = (((0xff << (8 * size - 1)) >> (i * 8))&(*(unsigned int *)write_val)) >> (size - i - 1);
			}
		}

		if (!VirtualProtectEx(hProcess, address, size, PAGE_READWRITE, &dwOldProt) && GetLastError() != 87)
		{
			mySetLastError(FAIL_TO_CHANGE_PAGE_INF);
			return FALSE;
		}

		while (real_writenum < size)
		{
			if (WriteProcessMemory(hProcess, (LPVOID)((char*)address + real_writenum), buffer + real_writenum, size - real_writenum, &real_writenum) == FALSE)
			{
				mySetLastError(WriteDataFalse);
				return FALSE;
			}
		}
		if (!VirtualProtectEx(hProcess, address, size, dwOldProt, &dwNewProt) && GetLastError() != 87)
		{
			mySetLastError(FAIL_TO_CHANGE_PAGE_INF);
			return FALSE;
		}
		break;

	case DOUBLE_T:size = sizeof(double);
		if (is_xd_machine == TRUE)
		{
			for (int i = 0; i < size; i++)
			{
				buffer[i] = ((0xff << (i * 8))&(*(unsigned int *)write_val)) >> (i * 8);
			}
		}
		else{
			for (int i = 0; i < size; i++)
			{
				buffer[i] = (((0xff << (8 * size - 1)) >> (i * 8))&(*(unsigned int *)write_val)) >> (size - i - 1);
			}
		}

		if (!VirtualProtectEx(hProcess, address, size, PAGE_READWRITE, &dwOldProt) && GetLastError() != 87)
		{
			mySetLastError(FAIL_TO_CHANGE_PAGE_INF);
			return FALSE;
		}

		while (real_writenum < size)
		{
			if (WriteProcessMemory(hProcess, (LPVOID)((char*)address + real_writenum), buffer + real_writenum, size - real_writenum, &real_writenum) == FALSE)
			{
				mySetLastError(WriteDataFalse);
				return FALSE;
			}
		}
		if (!VirtualProtectEx(hProcess, address, size, dwOldProt, &dwNewProt) && GetLastError() != 87)
		{
			mySetLastError(FAIL_TO_CHANGE_PAGE_INF);
			return FALSE;
		}
		break;

	case STRING_T:size = (*(string *)write_val).length();

		strcpy_s((char*)buffer, sizeof(buffer), (*(string *)write_val).c_str());
		if (size + stringAppendZeroNUM>READ_BUFFER_SIZE)//检查长度是否合法
		{
			mySetLastError(StringAppendZeroNUM_TOOBIG);
			return FALSE;
		}
		for (int i = size; i < stringAppendZeroNUM + size; i++)
		{
			buffer[i] = (unsigned char)endSet;
		}
		if (!VirtualProtectEx(hProcess, address, size, PAGE_READWRITE, &dwOldProt) && GetLastError() != 87)
		{
			mySetLastError(FAIL_TO_CHANGE_PAGE_INF);
			return FALSE;
		}
		while (real_writenum < size + stringAppendZeroNUM)
		{
			if (WriteProcessMemory(hProcess, (LPVOID)((char*)address + real_writenum), buffer + real_writenum, size - real_writenum, &real_writenum) == FALSE)
			{
				mySetLastError(WriteDataFalse);
				return FALSE;
			}
		}
		if (!VirtualProtectEx(hProcess, address, size, dwOldProt, &dwNewProt) && GetLastError() != 87)
		{
			mySetLastError(FAIL_TO_CHANGE_PAGE_INF);
			return FALSE;
		}
		break;
	default:
		mySetLastError(WrongSearchType);
		return FALSE;
	}
	return TRUE;
}
