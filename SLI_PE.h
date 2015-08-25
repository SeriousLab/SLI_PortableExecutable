#pragma once
#include <windows.h>
#include <vector>

using std::vector;


//Stores detailed information of imported functions.
typedef struct _SLI_IMPORT_DETAILS
{
	ULONGLONG uAddress_rVA;
	ULONGLONG uAddress_Offset;
	DWORD     dwOrdinal;
	wchar_t   szFunctionName[MAX_PATH];
}SLI_IMPORT_DETAILS, *PSLI_IMPORT_DETAILS;


//Stores information about imported DLLs.
typedef struct _SLI_IMPORT_MODULES
{
	ULONGLONG uAddress_INT;
	ULONGLONG uAddress_IAT;
	wchar_t   szModuleName[MAX_PATH];
	vector<SLI_IMPORT_DETAILS> m_vecDetais;
}SLI_IMPORT_MODULES, *PSLI_IMPORT_MODULES;


//Stores the info acquired from DATA_DIR section of the PE structure.
typedef struct _SLI_DATA_DIR
{
	ULONGLONG uAddress_rVA;
	ULONGLONG uAddress_Offset;
	ULONGLONG uSize;
	wchar_t szDataDirName[MAX_PATH];
}SLI_DATA_DIR, *PSLI_DATA_DIR;


//section info
typedef struct _SLI_SECTION
{
	ULONGLONG uAddress_rVA;
	ULONGLONG uAddress_Offset;
	ULONGLONG uSize_file;
	ULONGLONG uSize_virtual;
	ULONGLONG uAttributes;
	wchar_t szSectionName[IMAGE_SIZEOF_SHORT_NAME + 1];
}SLI_SECTION, *PSLI_SECTION;


//Info in optional header
typedef struct _SLI_OPTIONAL_HEADER
{
	wchar_t szItem_Name[32];
	wchar_t szItem_Value[32];
}SLI_OPTIONAL_HEADER, *PSLI_OPTIONAL_HEADER;


//Export Functions
typedef struct _SLI_EXPORT_FUNCTIONS
{
	DWORD dwIndex;
	ULONGLONG uFunction_Address;
	wchar_t szFunctionName[120];
}SLI_EXPORT_FUNCTIONS, *PSLI_EXPORT_FUNCTIONS;

//Export table
typedef struct _SLI_EXPORT_TABLE
{
	wchar_t szFileName[MAX_PATH];
	ULONGLONG uIndex_Base;
	ULONGLONG uFunction_Count;
	ULONGLONG uName_Count;
	ULONGLONG uFunction_Address_Table;
	ULONGLONG uIndex_Table_Address;
	ULONGLONG uName_Table_Address;
	vector<SLI_EXPORT_FUNCTIONS> m_vecExportFunc;
}SLI_EXPORT_TABLE, *PSLI_EXPORT_TABLE;


//Resource table
typedef struct _SLI_RESOURCE_TABLE
{
	wchar_t szTypeName[32];
	DWORD dwType;

}SLI_RESOURCE_TABLE, *PSLI_RESOURCE_TABLE;

class CSLI_PE
{
public:
	CSLI_PE();
	~CSLI_PE();

private:
	BOOL SLI_is_PE(ULONGLONG uAddress, DWORD dwSize);

	BOOL SLI_acquire_Data_Dir(ULONGLONG uAddress);

	BOOL SLI_acquire_Sections(ULONGLONG uAddress);

	BOOL SLI_acquire_ExportTable(ULONGLONG uAddress);

	BOOL SLI_acquire_ImportTable(ULONGLONG uAddress);

	BOOL SLI_acquire_Resources(ULONGLONG uAddress);

	BOOL SLI_acquire_DebugInfo(ULONGLONG uAddress);

	BOOL SLI_acquire_TLS(ULONGLONG uAddress);

	BOOL SLI_acquire_Header_Info(ULONGLONG uAddress);

	BOOL SLI_is_x64(ULONGLONG uAddress);

	ULONGLONG SLI_rVA_2_Offset(ULONGLONG rVA, ULONGLONG uAddress);

	ULONGLONG SLI_Offset_2_rVA(ULONGLONG Offset, ULONGLONG uAddress);

	ULONGLONG SLI_rVA_2_VA(ULONGLONG rVA, ULONGLONG uAddress);

	wchar_t* SLI_acquire_Architecture(WORD wMachine);

	BOOL SLI_Load_File(wchar_t* szFilePath);

public:

	BOOL SLI_acquire_PE(wchar_t* szFilePath);

	vector<SLI_DATA_DIR> m_vecDataDir;

	vector<SLI_IMPORT_MODULES> m_vecImportTable;

	vector<SLI_SECTION> m_vecSection;

	vector<SLI_OPTIONAL_HEADER> m_vecOptional;

	BOOL m_x64 = FALSE;

	ULONGLONG m_LoadedAddress;

	SLI_EXPORT_TABLE m_Export;

	DWORD m_dwSize;
};

