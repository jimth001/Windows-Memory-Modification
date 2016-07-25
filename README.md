# Windows Memory Modification

## windows平台下内存修改API，实现修改其他进程的地址空间

内存管理及修改API（made by myself）说明：

1)   获取机器分页大小

     DWORD  WINAPI getSystemPageSize();

2)   提升权限

     BOOL WINAPI EnablePrivileges();

3)   获取进程列表

    /* OUTvector<PROCESS_MEMORY_COUNTERS>&
     OUTvector<PROCESSENTRY32>&
     return string描述进程信息*/
     string  WINAPI getProcessList(vector<PROCESS_MEMORY_COUNTERS>& vct_pmc,vector<PROCESSENTRY32>& vct_pe32);

4)   首次搜索

    /*
     vector<MEMORY_BASIC_INFORMATION>memInfo,筛选出的待搜索块
    
     search_info&v_s_rslt,保存搜索结果的数据结构
    
     HANDLE handle,进程句柄
    
     DWORD s_type,搜索类型，支持FLOAT_T INT_TDOUBLE_T STRING_T
    
     DWORD pagesize,系统页面大小
    
     char* charexpval,要搜索的STRING_T值
    
     int *intexpval,要搜索的INT_T值
    
     float* floatexpval,要搜索的FLOAT_T值
    
     double* doubleexpval要搜索的DOUBLE_T值
    
     */
    
     BOOL WINAPI searchAll(vector<MEMORY_BASIC_INFORMATION> memInfo, search_info&v_s_rslt, HANDLE handle, DWORD s_type, DWORD pagesize, char* charexpval,int *intexpval, float *floatexpval, double *doubleexpval);

5)   附加到进程（打开进程）

    /*
     DWORD IN pid,要打开的进程
    
     DWORD OUT hProcess成功后返回的进程句柄
    
     */
    
     BOOL WINAPI attachToProcess(DWORD Pid, HANDLE & hProcess);

6)   按属性筛选要搜索的内存块

    /*
    HANDLE IN hProcess 进程句柄
    
    vector OUT memInfo 保存内存块信息的数据结构vector<MEMORY_BASIC_INFORMATION>
    
    DWORD IN stateFilter 按state类型过滤内存块
    
    DWORD IN typeFilter 按type类型过滤内存块
    
    DWORD IN protectFilter 按protect类型过滤内存块
    
    */
    
    BOOL WINAPI getProcessMemBlockEX(HANDLE hProcess, 
    
    vector<MEMORY_BASIC_INFORMATION>& memInfo,DWORD stateFilter, DWORD 
    
    typeFilter, DWORD protectFilter, DWORD* memSize);

7)   申请一块虚存区

    /*
     int size,申请的虚存大小
    
     HANDLE handle进程句柄
    
     */
    
     LPVOID WINAPI AllocVirtual(intsize, HANDLE handle);

8)   释放虚存区

    /*
     LPVOID Buffer,释放的虚地址指针
    
     HANDLE handle进程句柄
    
     */
    
     BOOL WINAPI FreeVirtual(LPVOID Buffer, HANDLE handle);

9)   再次搜索

    /*
     HANDLE hProcess,//进程句柄
    
     search_info &v_s_rslt,//保存上次搜索信息的数据结构
    
     LPVOIDexpected_val//新的搜索值，对字符串传入char*而不是string *
    
     */
    
    BOOL WINAPI nextScan(HANDLE hProcess,search_info & v_s_rslt, LPVOID 
    
    expected_val);

10) 将数据写入指定线性地址

    /*
    HANDLEhProcess,//要写入的进程
    
    LPVOID address,//要写入的地址
    
    void * write_val,//要写入的变量指针，只能是string,int,double,
    
    float四种
    
    DWORD valType,//写入的变量类型，可选值见头文件宏定义
    
    int stringAppendZeroNUM,//字符串后追加的字符数量
    
    char endSet//字符串后追加字符，内存中string和char结尾不同
    
    */
    
    BOOL WINAPI writeData(HANDLE hProcess, LPVOID address,void * 
    
    write_val, DWORD valType, int stringAppendZeroNUM,char endSet);

11) 设置mylasterror的函数

     void mySetLastError(DWORD e);

12) 内部的error code函数，通过此获取错误类型，头文件有错误类型定义

     DWORD myGetLastError();

13) 必要的初始化函数，必须先初始化，目前只有检查对齐类型操作

     void init_for_mem_manage();

14) 检查机器内存对齐类型

     void check_mem_align_scheme();

15) 获取内存使用的系统信息

    string WINAPI getSystemInfo_toString();

16) 获取内存性能使用相关信息

    string WINAPI getPerformanceInfo_toString();

17) 获取进程私有地址空间信息

    string WINAPI processInfoToString(PROCESSENTRY32 pe, PROCESS_MEMORY_COUNTERS*pmc);

内存修改API相关数据结构的定义：

    typedef struct search_result_i{
    
       LPCVOID baseAddress;//基址
    
       DWORD offset;//偏移量
    
       int i_val;//内存中的值
    
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
    
       DWORDs_type;//搜索类型
    
       vector<struct search_result_i> i_list;//int搜索结果
    
       vector<struct search_result_d> d_list;//double搜索结果
    
       vector<struct search_result_f> f_list;//float搜索结果
    
       vector<struct search_result_s> s_list;//字符串搜索结果
    
       int i_exp;//搜索值
    
       double d_exp; //搜索值
    
       float f_exp; //搜索值
    
       string s_exp; //搜索值
    
    }search_info;//保存搜索信息的数据结构

内存修改API调用的基本流程：

EnablePrivileges提权

init_for_mem_manage必要的初始化工作，目前只是判断大小端机器

getSystemPageSize获取系统页大小，后面做参数

getProcessList获取进程列表

用户输入pid

attachToProcess附加到指定进程

getProcessMemBlockEX获取指定state，type，protect属性的内存块，

后面搜索只会在这些块里搜

用户选择类型和搜索值

searchAll首次搜索，结果保存在struct search_info

再次输入

nextScan再次搜索

当确认搜索到想要的数据时，根据其地址使用writeData进行修改。
