#include "stdafx.h"
#include "SLI_PE.h"
#include <strsafe.h>


//It actually wastes quite some computing power to get the pointer of 
//all these structures every time a function is called. 
//I am not sure why I code it this way, but this makes these functions
//more independent. Deal with it.


//Names of Data_Directories
static wchar_t szData_Directories[16][32] = {
	L"Export Directory",
	L"Import Directory",
	L"Resource Directory",
	L"Exception Directory",
	L"Security Directory",
	L"Base Relocation Table",
	L"Debug Directory",
	L"Architecture Specific Data",
	L"rVA of Global Pointers",
	L"TLS Directory",
	L"Load Config Directory",
	L"Bound Import Directory",
	L"Import Address Table",
	L"Delay Load Import Descriptors",
	L"COM Runtime Descriptor",
	L"System Reserved"
};


static wchar_t szHeader_Info[20][32] = {
	L"File_Type",
	L"Linker_Version",
	L"Entry_Point",
	L"Image_Base",
	L"Image_Size",
	L"Header_Size",
	L"File_Alignment",
	L"Section_Alignment",
	L"OS_Version",
	L"Base_Of_Code",
	L"Base_Of_Data",
	L"Check_Sum",
	L"DLL_Characteristics",
	L"Stack_Reserve",
	L"Stack_Commit",
	L"Heap_Reserve",
	L"Heap_Commit",
	L"Loader_Flag",
	L"Architecture",
	L"Section_Count"
};



CSLI_PE::CSLI_PE()
{
}


CSLI_PE::~CSLI_PE()
{
}



//BOOL CSLI_PE::SLI_is_PE(ULONGLONG uAddress,DWORD dwSize)
//Test if the file loaded is a valid PE file.
//|-uAddress is the loaded address acquired from the file loading function.
//|-dwSize is the total image size.
//The function returns the result of such test.

BOOL CSLI_PE::SLI_is_PE(ULONGLONG uAddress, DWORD dwSize)
{
	//Convert the argument passed in to a pointer of type IMAGE_DOS_HEADER
	PIMAGE_DOS_HEADER pDOS_Header = (PIMAGE_DOS_HEADER)uAddress;

	//For a PE format file to run under MS-Windows NT, there should be a DOS header.
	if (pDOS_Header->e_magic != IMAGE_DOS_SIGNATURE)
	{
		return FALSE;
	}

	//This is to prevent the pure DOS application.As this class only takes the new PE format
	if (pDOS_Header->e_lfanew<0 || pDOS_Header->e_lfanew>dwSize)
	{
		return FALSE;
	}

	//Get the NT header pointer. 
	//Here I use the 32 bit version is that the NT signature is always the first 4 bytes that
	//the DOS_HEADER->e_lfanew points to.
	//In this function, the only purpose is to determine if this is valid PE file. So it doesn't 
	//matter if the type of this pointer is for 32 bit or for 64 bit.
	PIMAGE_NT_HEADERS32 pNT_Header = (PIMAGE_NT_HEADERS32)(pDOS_Header->e_lfanew + uAddress);

	if (pNT_Header->Signature != IMAGE_NT_SIGNATURE)
	{
		return FALSE;
	}

	//If all conditions above is not satisfied, then it should be a proper PE file.
	//Here it also eliminates the ROM file.
	return TRUE;
}



//BOOL CSLI_PE::SLI_is_x64(ULONGLONG uAddress)
//Test if the file is for x64 Platform
//|-uAddress is the Loaded address of this file to be tested.
//The function returns the result of such test.

BOOL CSLI_PE::SLI_is_x64(ULONGLONG uAddress)
{
	//Acquire the pointer of type IMAGE_DOS_HEADER
	PIMAGE_DOS_HEADER pDOS_Header = (PIMAGE_DOS_HEADER)uAddress;

	//Acquire the pointer of type IMAGE_NT_HEADER
	//The problem is, at this stage, there is no evidence to show if the application 
	//is to run on a x86 platform or is it set for the x64 platform.
	//There is a slight difference between the two. But here I will use the 32 bit config anyway.
	PIMAGE_NT_HEADERS32 pNT_Header = (PIMAGE_NT_HEADERS32)(uAddress + pDOS_Header->e_lfanew);

	//The true difference between the two version is its optional_header structure.
	//So, before it is decided, I will use the 32 bit version as always.
	PIMAGE_OPTIONAL_HEADER32 pOPTIONAL_Header = &(pNT_Header->OptionalHeader);

	//Here I only care about the 64 bit signature.
	if (pOPTIONAL_Header->Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
	{
		return TRUE;
	}

	//If it's not x64, it x86.
	return FALSE;
}



//BOOL CSLI_PE::SLI_acquire_Data_Dir(ULONGLONG uAddress)
//Acquire Data_directory information and stores them in m_vecDataDir
//|-uAddress is the loaded address.
//This function doesn't really need a return value, but anyway, I put a bool
//value here for a easier way to locate the default if there is any.

BOOL CSLI_PE::SLI_acquire_Data_Dir(ULONGLONG uAddress)
{
	//Acquire DOS header, as always
	PIMAGE_DOS_HEADER pDOS_Header = (PIMAGE_DOS_HEADER)uAddress;

	//Set an empty pointer to be adjusted accordingly.
	PIMAGE_DATA_DIRECTORY pDATA_Directory = nullptr;

	//At this stage, it should be clear if the file is for x86 or x64
	if (m_x64)
	{
		PIMAGE_NT_HEADERS64 pNT_Header = (PIMAGE_NT_HEADERS64)(pDOS_Header->e_lfanew + uAddress);
		PIMAGE_OPTIONAL_HEADER64 pOPTIONAL_Header = &(pNT_Header->OptionalHeader);
		pDATA_Directory = (PIMAGE_DATA_DIRECTORY)pOPTIONAL_Header->DataDirectory;
	}
	else
	{
		PIMAGE_NT_HEADERS32 pNT_Header = (PIMAGE_NT_HEADERS32)(pDOS_Header->e_lfanew + uAddress);
		PIMAGE_OPTIONAL_HEADER32 pOPTIONAL_Header = &(pNT_Header->OptionalHeader);
		pDATA_Directory = (PIMAGE_DATA_DIRECTORY)pOPTIONAL_Header->DataDirectory;
	}

	//Loop to fill up m_vecDataDir.
	for (DWORD i = 0; i < IMAGE_NUMBEROF_DIRECTORY_ENTRIES; i++)
	{
		SLI_DATA_DIR stcDate_Directory = { 0 };
		stcDate_Directory.uAddress_rVA = pDATA_Directory[i].VirtualAddress;
		stcDate_Directory.uSize = pDATA_Directory[i].Size;
		wsprintf(stcDate_Directory.szDataDirName, L"%ls", szData_Directories[i]);
		stcDate_Directory.uAddress_Offset = SLI_rVA_2_Offset(pDATA_Directory[i].VirtualAddress, uAddress);
		m_vecDataDir.push_back(stcDate_Directory);
	}

	return TRUE;
}



//ULONGLONG CSLI_PE::SLI_rVA_2_Offset(ULONGLONG rVA)
//To convert the rVA into Offset
//|-rVA is the relative virtual address that need to be converted.
//Returns offset towards the start of the file.(position in file.

ULONGLONG CSLI_PE::SLI_rVA_2_Offset(ULONGLONG rVA, ULONGLONG uAddress)
{
	//Acquire DOS header.
	PIMAGE_DOS_HEADER pDOS_Header = (PIMAGE_DOS_HEADER)uAddress;

	//actually, image_file_header is independent from platform specification.
	//But I feel like assign it from different NT_header declaration with no reason.
	PIMAGE_FILE_HEADER pFILE_Header = nullptr;

	PIMAGE_SECTION_HEADER pSECTION_Header = nullptr;

	//Now, according to Microsoft, this macro returns the right address 
	//regardless the platform specification. 
	//But just in case, I will write it separately.
	if (m_x64)
	{
		PIMAGE_NT_HEADERS64 pNT_Header = (PIMAGE_NT_HEADERS64)(uAddress + pDOS_Header->e_lfanew);
		pFILE_Header = &(pNT_Header->FileHeader);
		pSECTION_Header = IMAGE_FIRST_SECTION(pNT_Header);
	}
	else
	{
		PIMAGE_NT_HEADERS32 pNT_Header = (PIMAGE_NT_HEADERS32)(uAddress + pDOS_Header->e_lfanew);
		pFILE_Header = &(pNT_Header->FileHeader);
		pSECTION_Header = IMAGE_FIRST_SECTION(pNT_Header);
	}

	//Loop through to get the right number.
	for (WORD i = 0; i < pFILE_Header->NumberOfSections; i++)
	{
		ULONGLONG rVA_Begin = pSECTION_Header[i].VirtualAddress;
		ULONGLONG rVA_End = pSECTION_Header[i].Misc.VirtualSize + pSECTION_Header[i].VirtualAddress;
		if (rVA >= rVA_Begin&&rVA < rVA_End)
		{
			return rVA - pSECTION_Header[i].VirtualAddress + pSECTION_Header[i].PointerToRawData;
		}
	}

	//Return -1 indicates a failure in calculating offset.
	//The return value should be unsigned, but anyway, it would work.
	return -1;
}



//BOOL CSLI_PE::SLI_acquire_Sections(ULONGLONG uAddress)
//Retrieve detailed section information from the PE structure.
//|-uAddress is the loaded address.
//The bool type return value is just for fast locating of error in test.(well, mainly.

BOOL CSLI_PE::SLI_acquire_Sections(ULONGLONG uAddress)
{
	//This one is quite like the rVA_Offset conversion, the only difference is to obtain 
	//difference kind of information.

	PIMAGE_DOS_HEADER pDOS_Header = (PIMAGE_DOS_HEADER)uAddress;

	PIMAGE_FILE_HEADER pFILE_Header = nullptr;

	PIMAGE_SECTION_HEADER pSECTION_Header = nullptr;

	if (m_x64)
	{
		PIMAGE_NT_HEADERS64 pNT_Header = (PIMAGE_NT_HEADERS64)(pDOS_Header->e_lfanew + uAddress);
		pFILE_Header = &(pNT_Header->FileHeader);
		pSECTION_Header = IMAGE_FIRST_SECTION(pNT_Header);
	}
	else
	{
		PIMAGE_NT_HEADERS32 pNT_Header = (PIMAGE_NT_HEADERS32)(pDOS_Header->e_lfanew + uAddress);
		pFILE_Header = &(pNT_Header->FileHeader);
		pSECTION_Header = IMAGE_FIRST_SECTION(pNT_Header);
	}

	//It's still a loop here, just that the information acquired will be stored in m_vecSection.
	for (WORD i = 0; i < pFILE_Header->NumberOfSections; i++)
	{
		SLI_SECTION stcSection = { 0 };
		stcSection.uAddress_rVA = pSECTION_Header[i].VirtualAddress;
		stcSection.uAttributes = pSECTION_Header[i].Characteristics;
		stcSection.uSize_file = pSECTION_Header[i].SizeOfRawData;
		stcSection.uSize_virtual = pSECTION_Header[i].Misc.VirtualSize;
		stcSection.uAddress_Offset = SLI_rVA_2_Offset(pSECTION_Header[i].VirtualAddress, uAddress);
		wsprintf(stcSection.szSectionName, L"%8S", pSECTION_Header[i].Name);
		m_vecSection.push_back(stcSection);
	}

	return TRUE;
}



//BOOL CSLI_PE::SLI_acquire_Header_Info(ULONGLONG uAddress)
//Retrieve information stored in optional header.
//|-uAddress is the loaded address
//Return value is for test purpose.

BOOL CSLI_PE::SLI_acquire_Header_Info(ULONGLONG uAddress)
{
	//Clear the container.
	m_vecOptional.clear();

	PIMAGE_DOS_HEADER pDOS_Header = (PIMAGE_DOS_HEADER)uAddress;

	//Optional header is affected by platform specification.
	//Most part of the format is the same, but I didn't find a way to convert between
	//the two. So there is going to be a large pile of redundant code...
	if (m_x64)
	{
		PIMAGE_NT_HEADERS64 pNT_Header = (PIMAGE_NT_HEADERS64)(pDOS_Header->e_lfanew + uAddress);
		PIMAGE_OPTIONAL_HEADER64 pOPTIONAL_Header = &(pNT_Header->OptionalHeader);

		SLI_OPTIONAL_HEADER stcOptional_Header = { 0 };

		//The thing is, values stored in optional header are not always the same size.
		//So...... We will have to manually extract them, one by one...
		//Be aware, I did not retrieve these values according to the ordinal as are they defined.

		//From here onwards in this function is pure labor...

		//First, File type.
		//Apparently, in this branch, it's x64.
		wsprintf(stcOptional_Header.szItem_Name, L"%ls", szHeader_Info[0]);
		StringCchCopy(stcOptional_Header.szItem_Value, 32, L"64 bit Executable");
		m_vecOptional.push_back(stcOptional_Header);

		//Second, Linker version.
		wsprintf(stcOptional_Header.szItem_Name, L"%ls", szHeader_Info[1]);
		wsprintf(stcOptional_Header.szItem_Value, L"%d.%d", pOPTIONAL_Header->MajorLinkerVersion, pOPTIONAL_Header->MinorLinkerVersion);
		m_vecOptional.push_back(stcOptional_Header);

		//Third, Entry point.
		wsprintf(stcOptional_Header.szItem_Name, L"%ls", szHeader_Info[2]);
		wsprintf(stcOptional_Header.szItem_Value, L"%#08X", pOPTIONAL_Header->AddressOfEntryPoint);
		m_vecOptional.push_back(stcOptional_Header);

		//Forth, Image base.
		wsprintf(stcOptional_Header.szItem_Name, L"%ls", szHeader_Info[3]);
		wsprintf(stcOptional_Header.szItem_Value, L"%#08X", pOPTIONAL_Header->ImageBase);
		m_vecOptional.push_back(stcOptional_Header);

		//Fifth, Image size.
		wsprintf(stcOptional_Header.szItem_Name, L"%ls", szHeader_Info[4]);
		wsprintf(stcOptional_Header.szItem_Value, L"%#08X", pOPTIONAL_Header->SizeOfImage);
		m_vecOptional.push_back(stcOptional_Header);

		//Sixth, Header size.
		wsprintf(stcOptional_Header.szItem_Name, L"%ls", szHeader_Info[5]);
		wsprintf(stcOptional_Header.szItem_Value, L"%#08X", pOPTIONAL_Header->SizeOfHeaders);
		m_vecOptional.push_back(stcOptional_Header);

		//Seventh, File alignment.
		wsprintf(stcOptional_Header.szItem_Name, L"%ls", szHeader_Info[6]);
		wsprintf(stcOptional_Header.szItem_Value, L"%#08X", pOPTIONAL_Header->FileAlignment);
		m_vecOptional.push_back(stcOptional_Header);

		//Eighth, Section alignment.
		wsprintf(stcOptional_Header.szItem_Name, L"%ls", szHeader_Info[7]);
		wsprintf(stcOptional_Header.szItem_Value, L"%#08X", pOPTIONAL_Header->SectionAlignment);
		m_vecOptional.push_back(stcOptional_Header);

		//Ninth, OS version.
		wsprintf(stcOptional_Header.szItem_Name, L"%ls", szHeader_Info[8]);
		wsprintf(stcOptional_Header.szItem_Value, L"%d.%d", pOPTIONAL_Header->MajorOperatingSystemVersion, pOPTIONAL_Header->MinorOperatingSystemVersion);
		m_vecOptional.push_back(stcOptional_Header);

		//Tenth, Code base.
		wsprintf(stcOptional_Header.szItem_Name, L"%ls", szHeader_Info[9]);
		wsprintf(stcOptional_Header.szItem_Value, L"%#08X", pOPTIONAL_Header->BaseOfCode);
		m_vecOptional.push_back(stcOptional_Header);

		//Eleventh, Data base.
		//x64 does not have BaseOfData.

		//Twelfth, Check sum.
		wsprintf(stcOptional_Header.szItem_Name, L"%ls", szHeader_Info[11]);
		wsprintf(stcOptional_Header.szItem_Value, L"%#08X", pOPTIONAL_Header->CheckSum);
		m_vecOptional.push_back(stcOptional_Header);

		//Thirteenth, DLL Characteristics.
		wsprintf(stcOptional_Header.szItem_Name, L"%ls", szHeader_Info[12]);
		wsprintf(stcOptional_Header.szItem_Value, L"%#08X", pOPTIONAL_Header->DllCharacteristics);
		m_vecOptional.push_back(stcOptional_Header);

		//Fourteenth, Stack reserve.
		wsprintf(stcOptional_Header.szItem_Name, L"%ls", szHeader_Info[13]);
		wsprintf(stcOptional_Header.szItem_Value, L"%#08X", pOPTIONAL_Header->SizeOfStackReserve);
		m_vecOptional.push_back(stcOptional_Header);

		//Fifteenth, Stack commit.
		wsprintf(stcOptional_Header.szItem_Name, L"%ls", szHeader_Info[14]);
		wsprintf(stcOptional_Header.szItem_Value, L"%#08X", pOPTIONAL_Header->SizeOfStackCommit);
		m_vecOptional.push_back(stcOptional_Header);

		//Sixteenth, Heap reserve.
		wsprintf(stcOptional_Header.szItem_Name, L"%ls", szHeader_Info[15]);
		wsprintf(stcOptional_Header.szItem_Value, L"%#08X", pOPTIONAL_Header->SizeOfHeapReserve);
		m_vecOptional.push_back(stcOptional_Header);

		//Seventeenth, Heap commit.
		wsprintf(stcOptional_Header.szItem_Name, L"%ls", szHeader_Info[16]);
		wsprintf(stcOptional_Header.szItem_Value, L"%#08X", pOPTIONAL_Header->SizeOfHeapCommit);
		m_vecOptional.push_back(stcOptional_Header);

		//Eighteenth, Loader flags.
		wsprintf(stcOptional_Header.szItem_Name, L"%ls", szHeader_Info[17]);
		wsprintf(stcOptional_Header.szItem_Value, L"%#08X", pOPTIONAL_Header->LoaderFlags);
		m_vecOptional.push_back(stcOptional_Header);

		//Nineteenth, Architecture.
		//Note that this one is in IMAGE_FILE_HEADER, Putting it here is not exactly right. But only to simplify it.
		wsprintf(stcOptional_Header.szItem_Name, L"%ls", szHeader_Info[18]);
		wsprintf(stcOptional_Header.szItem_Value, L"%ls", SLI_acquire_Architecture(pNT_Header->FileHeader.Machine));
		m_vecOptional.push_back(stcOptional_Header);

		//Twentieth, Section count.
		//This one is also in IMAGE_FILE_HEADER.
		wsprintf(stcOptional_Header.szItem_Name, L"%ls", szHeader_Info[19]);
		wsprintf(stcOptional_Header.szItem_Value, L"%#04X", pNT_Header->FileHeader.NumberOfSections);
		m_vecOptional.push_back(stcOptional_Header);

		return TRUE;
	}
	else
	{
		PIMAGE_NT_HEADERS32 pNT_Header = (PIMAGE_NT_HEADERS32)(pDOS_Header->e_lfanew + uAddress);
		PIMAGE_OPTIONAL_HEADER32 pOPTIONAL_Header = &(pNT_Header->OptionalHeader);

		SLI_OPTIONAL_HEADER stcOptional_Header = { 0 };

		//First, File type.
		//Apparently, in this branch, it's x86.
		wsprintf(stcOptional_Header.szItem_Name, L"%ls", szHeader_Info[0]);
		StringCchCopy(stcOptional_Header.szItem_Value, 32, L"32 bit Executable");
		m_vecOptional.push_back(stcOptional_Header);

		//Second, Linker version.
		wsprintf(stcOptional_Header.szItem_Name, L"%ls", szHeader_Info[1]);
		wsprintf(stcOptional_Header.szItem_Value, L"%d.%d", pOPTIONAL_Header->MajorLinkerVersion, pOPTIONAL_Header->MinorLinkerVersion);
		m_vecOptional.push_back(stcOptional_Header);

		//Third, Entry point.
		wsprintf(stcOptional_Header.szItem_Name, L"%ls", szHeader_Info[2]);
		wsprintf(stcOptional_Header.szItem_Value, L"%#08X", pOPTIONAL_Header->AddressOfEntryPoint);
		m_vecOptional.push_back(stcOptional_Header);

		//Forth, Image base.
		wsprintf(stcOptional_Header.szItem_Name, L"%ls", szHeader_Info[3]);
		wsprintf(stcOptional_Header.szItem_Value, L"%#08X", pOPTIONAL_Header->ImageBase);
		m_vecOptional.push_back(stcOptional_Header);

		//Fifth, Image size.
		wsprintf(stcOptional_Header.szItem_Name, L"%ls", szHeader_Info[4]);
		wsprintf(stcOptional_Header.szItem_Value, L"%#08X", pOPTIONAL_Header->SizeOfImage);
		m_vecOptional.push_back(stcOptional_Header);

		//Sixth, Header size.
		wsprintf(stcOptional_Header.szItem_Name, L"%ls", szHeader_Info[5]);
		wsprintf(stcOptional_Header.szItem_Value, L"%#08X", pOPTIONAL_Header->SizeOfHeaders);
		m_vecOptional.push_back(stcOptional_Header);

		//Seventh, File alignment.
		wsprintf(stcOptional_Header.szItem_Name, L"%ls", szHeader_Info[6]);
		wsprintf(stcOptional_Header.szItem_Value, L"%#08X", pOPTIONAL_Header->FileAlignment);
		m_vecOptional.push_back(stcOptional_Header);

		//Eighth, Section alignment.
		wsprintf(stcOptional_Header.szItem_Name, L"%ls", szHeader_Info[7]);
		wsprintf(stcOptional_Header.szItem_Value, L"%#08X", pOPTIONAL_Header->SectionAlignment);
		m_vecOptional.push_back(stcOptional_Header);

		//Ninth, OS version.
		wsprintf(stcOptional_Header.szItem_Name, L"%ls", szHeader_Info[8]);
		wsprintf(stcOptional_Header.szItem_Value, L"%d.%d", pOPTIONAL_Header->MajorOperatingSystemVersion, pOPTIONAL_Header->MinorOperatingSystemVersion);
		m_vecOptional.push_back(stcOptional_Header);

		//Tenth, Code base.
		wsprintf(stcOptional_Header.szItem_Name, L"%ls", szHeader_Info[9]);
		wsprintf(stcOptional_Header.szItem_Value, L"%#08X", pOPTIONAL_Header->BaseOfCode);
		m_vecOptional.push_back(stcOptional_Header);

		//Eleventh, Data base.
		wsprintf(stcOptional_Header.szItem_Name, L"%ls", szHeader_Info[10]);
		wsprintf(stcOptional_Header.szItem_Value, L"%#08X", pOPTIONAL_Header->BaseOfData);
		m_vecOptional.push_back(stcOptional_Header);

		//Twelfth, Check sum.
		wsprintf(stcOptional_Header.szItem_Name, L"%ls", szHeader_Info[11]);
		wsprintf(stcOptional_Header.szItem_Value, L"%#08X", pOPTIONAL_Header->CheckSum);
		m_vecOptional.push_back(stcOptional_Header);

		//Thirteenth, DLL Characteristics.
		wsprintf(stcOptional_Header.szItem_Name, L"%ls", szHeader_Info[12]);
		wsprintf(stcOptional_Header.szItem_Value, L"%#08X", pOPTIONAL_Header->DllCharacteristics);
		m_vecOptional.push_back(stcOptional_Header);

		//Fourteenth, Stack reserve.
		wsprintf(stcOptional_Header.szItem_Name, L"%ls", szHeader_Info[13]);
		wsprintf(stcOptional_Header.szItem_Value, L"%#08X", pOPTIONAL_Header->SizeOfStackReserve);
		m_vecOptional.push_back(stcOptional_Header);

		//Fifteenth, Stack commit.
		wsprintf(stcOptional_Header.szItem_Name, L"%ls", szHeader_Info[14]);
		wsprintf(stcOptional_Header.szItem_Value, L"%#08X", pOPTIONAL_Header->SizeOfStackCommit);
		m_vecOptional.push_back(stcOptional_Header);

		//Sixteenth, Heap reserve.
		wsprintf(stcOptional_Header.szItem_Name, L"%ls", szHeader_Info[15]);
		wsprintf(stcOptional_Header.szItem_Value, L"%#08X", pOPTIONAL_Header->SizeOfHeapReserve);
		m_vecOptional.push_back(stcOptional_Header);

		//Seventeenth, Heap commit.
		wsprintf(stcOptional_Header.szItem_Name, L"%ls", szHeader_Info[16]);
		wsprintf(stcOptional_Header.szItem_Value, L"%#08X", pOPTIONAL_Header->SizeOfHeapCommit);
		m_vecOptional.push_back(stcOptional_Header);

		//Eighteenth, Loader flags.
		wsprintf(stcOptional_Header.szItem_Name, L"%ls", szHeader_Info[17]);
		wsprintf(stcOptional_Header.szItem_Value, L"%#08X", pOPTIONAL_Header->LoaderFlags);
		m_vecOptional.push_back(stcOptional_Header);

		//Nineteenth, Architecture.
		//Note that this one is in IMAGE_FILE_HEADER, Putting it here is not exactly right. But only to simplify it.
		wsprintf(stcOptional_Header.szItem_Name, L"%ls", szHeader_Info[18]);
		wsprintf(stcOptional_Header.szItem_Value, L"%ls", SLI_acquire_Architecture(pNT_Header->FileHeader.Machine));
		m_vecOptional.push_back(stcOptional_Header);

		//Twentieth, Section count.
		//This one is also in IMAGE_FILE_HEADER.
		wsprintf(stcOptional_Header.szItem_Name, L"%ls", szHeader_Info[19]);
		wsprintf(stcOptional_Header.szItem_Value, L"%#04X", pNT_Header->FileHeader.NumberOfSections);
		m_vecOptional.push_back(stcOptional_Header);

		return TRUE;
	}
}



//wchar_t* CSLI_PE::SLI_acquire_Architecture(WORD wMachine)
//This is where I get Architecture info from.
//|-[wMachine] is acquired from IMAGE_FILE_HEADER.
//Returns a wide-character string.
wchar_t* CSLI_PE::SLI_acquire_Architecture(WORD wMachine)
{
	static wchar_t szArchitecture[32] = { 0 };
	ZeroMemory(szArchitecture, 32 * sizeof(wchar_t));

	switch (wMachine)
	{
	case IMAGE_FILE_MACHINE_UNKNOWN:
	{
									   wsprintf(szArchitecture, L"Unknown");
									   break;
	}
	case IMAGE_FILE_MACHINE_I386:
	{
									wsprintf(szArchitecture, L"Intel 386");
									break;
	}
	case IMAGE_FILE_MACHINE_POWERPC:
	{
									   wsprintf(szArchitecture, L"IBM Power PC Little-Endian");
									   break;
	}
	case IMAGE_FILE_MACHINE_IA64:
	{
									wsprintf(szArchitecture, L"Intel x64");
									break;
	}
	case IMAGE_FILE_MACHINE_AMD64:
	{
									 wsprintf(szArchitecture, L"AMD x64(K8)");
									 break;
	}
	case IMAGE_FILE_MACHINE_ARM:
	{
								   wsprintf(szArchitecture, L"ARM Little-Endian");
								   break;
	}
	case IMAGE_FILE_MACHINE_ALPHA64:
	{
									   wsprintf(szArchitecture, L"Alpha x64");
									   break;
	}
	case IMAGE_FILE_MACHINE_ALPHA:
	{
									 wsprintf(szArchitecture, L"Alpha AXP");
									 break;
	}
	default:
	{
			   wsprintf(szArchitecture, L"Unknown");
			   break;
	}
	}

	//OK, this is not all that is supported by Microsoft.
	//I only took part of it as others are not so commonly seen.(for me.
	return szArchitecture;
}



//BOOL CSLI_PE::SLI_acquire_ExportTable(ULONGLONG uAddress)
//Acquire Export table and put it in m_Export.
//|-[uAddress] Loaded address.
//Returns whether the function succeeds.
BOOL CSLI_PE::SLI_acquire_ExportTable(ULONGLONG uAddress)
{
	PIMAGE_DOS_HEADER pDOS_Header = (PIMAGE_DOS_HEADER)uAddress;

	PIMAGE_DATA_DIRECTORY pExport_Dir = nullptr;

	if (m_x64)
	{
		PIMAGE_NT_HEADERS64 pNT_Header = (PIMAGE_NT_HEADERS64)(uAddress + pDOS_Header->e_lfanew);
		PIMAGE_DATA_DIRECTORY pDATA_Dir = (pNT_Header->OptionalHeader.DataDirectory);

		//Some applications do not have an export table. So, let's set a test here.
		if (pDATA_Dir[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress == 0 || pDATA_Dir[IMAGE_DIRECTORY_ENTRY_EXPORT].Size == 0)
		{
			pDATA_Dir = nullptr;
			pNT_Header = nullptr;
			pDOS_Header = nullptr;

			//If either of the two attributes is zero, this application does not have an export table.
			return FALSE;
		}

		pExport_Dir = &pDATA_Dir[IMAGE_DIRECTORY_ENTRY_EXPORT];
	}
	else
	{
		PIMAGE_NT_HEADERS32 pNT_Header = (PIMAGE_NT_HEADERS32)(uAddress + pDOS_Header->e_lfanew);
		PIMAGE_DATA_DIRECTORY pDATA_Dir = (pNT_Header->OptionalHeader.DataDirectory);

		if (pDATA_Dir[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress == 0 || pDATA_Dir[IMAGE_DIRECTORY_ENTRY_EXPORT].Size == 0)
		{
			pDATA_Dir = nullptr;
			pNT_Header = nullptr;
			pDOS_Header = nullptr;
			return FALSE;
		}

		pExport_Dir = &pDATA_Dir[IMAGE_DIRECTORY_ENTRY_EXPORT];
	}

	//The virtual address stored in data directory may not be used directly.
	ULONGLONG uExport_Offset = SLI_rVA_2_Offset(pExport_Dir->VirtualAddress, uAddress);
	PIMAGE_EXPORT_DIRECTORY pExport = (PIMAGE_EXPORT_DIRECTORY)(uExport_Offset + uAddress);

	if (pExport->Characteristics != 0)
	{
		pDOS_Header = nullptr;
		pExport_Dir = nullptr;
		pExport = nullptr;

		//It should be zero. If it's not, something is wrong.
		return FALSE;
	}

	//Here, I am referring to my old code.
	//The address stored in the structure is rVA, and to really read it out, 
	//it needs to be converted into offset.
	m_Export.uName_Table_Address = SLI_rVA_2_Offset(pExport->AddressOfNames, uAddress);
	m_Export.uFunction_Address_Table = SLI_rVA_2_Offset(pExport->AddressOfFunctions, uAddress);
	m_Export.uIndex_Table_Address = SLI_rVA_2_Offset(pExport->AddressOfNameOrdinals, uAddress);
	m_Export.uIndex_Base = pExport->Base;
	m_Export.uName_Count = pExport->NumberOfNames;
	m_Export.uFunction_Count = pExport->NumberOfFunctions;

	wsprintf(m_Export.szFileName, L"%S", (PCHAR)(SLI_rVA_2_Offset(pExport->Name, uAddress) + uAddress));

	//Now, loop to extract functions in the export table.
	//Before that, there is a need to declare pointers towards those address we acquired earlier.
	PDWORD pAddress_Table = (PDWORD)(m_Export.uFunction_Address_Table + uAddress);
	PDWORD pName_Table = (PDWORD)(m_Export.uName_Table_Address + uAddress);
	PWORD pIndex_Table = (PWORD)(m_Export.uIndex_Table_Address + uAddress);

	for (DWORD dwOrdinal = 0; dwOrdinal < pExport->NumberOfFunctions; dwOrdinal++)
	{
		//There are sometime, some member of the address_table do not posses valid value.
		//So, we ignore them.
		if (pAddress_Table[dwOrdinal] == 0)
		{
			continue;
		}

		//Here, we prepare a SLI_EXPORT_FUNCTIONS
		SLI_EXPORT_FUNCTIONS sli_Function = { 0 };

		for (DWORD dwIndex = 0; dwIndex < pExport->NumberOfNames; dwIndex++)
		{
			//If the ordinal is found in index_table.
			//Then this function got a name.
			if (pIndex_Table[dwIndex] == dwOrdinal)
			{
				wsprintf(sli_Function.szFunctionName, L"%S", (PCHAR)(SLI_rVA_2_Offset(pName_Table[dwIndex], uAddress) + uAddress));
				sli_Function.dwIndex = dwOrdinal + pExport->Base;
				sli_Function.uFunction_Address = pAddress_Table[dwOrdinal];
				m_Export.m_vecExportFunc.push_back(sli_Function);
				break;
			}
			//If it is the last index. 
			//This function is exported as ordinal.
			if (dwIndex == pExport->NumberOfNames - 1)
			{
				wsprintf(sli_Function.szFunctionName, L"-");
				sli_Function.dwIndex = dwOrdinal + pExport->Base;
				sli_Function.uFunction_Address = pAddress_Table[dwOrdinal];
				m_Export.m_vecExportFunc.push_back(sli_Function);
			}
		}
	}
	//That's about it for export table.
	return TRUE;
}



//BOOL CSLI_PE::SLI_acquire_ImportTable(ULONGLONG uAddress)
//This one acquires import_table data and stores into m_vecImportTable
//|-[uAddress] is the loaded address
//Returns if the function succeeds.
BOOL CSLI_PE::SLI_acquire_ImportTable(ULONGLONG uAddress)
{
	//As always, the DOS_Header comes first.
	PIMAGE_DOS_HEADER pDOS_Header = (PIMAGE_DOS_HEADER)uAddress;

	PIMAGE_DATA_DIRECTORY pImport_Dir = nullptr;
	if (m_x64)
	{
		PIMAGE_NT_HEADERS64 pNT_Header = (PIMAGE_NT_HEADERS64)(uAddress + pDOS_Header->e_lfanew);
		PIMAGE_DATA_DIRECTORY pDATA_Dir = (pNT_Header->OptionalHeader.DataDirectory);

		if (pDATA_Dir[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress == 0 || pDATA_Dir[IMAGE_DIRECTORY_ENTRY_IMPORT].Size == 0)
		{
			pDOS_Header = nullptr;
			pNT_Header = nullptr;
			pDATA_Dir = nullptr;
			return FALSE;
		}

		pImport_Dir = &pDATA_Dir[IMAGE_DIRECTORY_ENTRY_IMPORT];
	}
	else
	{
		PIMAGE_NT_HEADERS32 pNT_Header = (PIMAGE_NT_HEADERS32)(uAddress + pDOS_Header->e_lfanew);
		PIMAGE_DATA_DIRECTORY pDATA_Dir = (pNT_Header->OptionalHeader.DataDirectory);

		if (pDATA_Dir[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress == 0 || pDATA_Dir[IMAGE_DIRECTORY_ENTRY_IMPORT].Size == 0)
		{
			pDOS_Header = nullptr;
			pNT_Header = nullptr;
			pDATA_Dir = nullptr;
			return FALSE;
		}

		pImport_Dir = &pDATA_Dir[IMAGE_DIRECTORY_ENTRY_IMPORT];
	}

	ULONGLONG uImport_Offset = SLI_rVA_2_Offset(pImport_Dir->VirtualAddress, uAddress);

	PIMAGE_IMPORT_DESCRIPTOR pImport = (PIMAGE_IMPORT_DESCRIPTOR)(uAddress + uImport_Offset);

	//The tricky point here is that the import table is an array of such structure, IMAGE_IMPORT_DESCRIPTOR.
	//It does not have a count, so we have no idea of how many IMAGE_IMPORT_DESCRIPTOR there are in this file.
	//So, we use loop.
	//A full-zero structure indicates a termination of the array. If it's not zero, it's valid.
	while (pImport->Name)
	{
		SLI_IMPORT_MODULES import_Module = { 0 };
		wsprintf(import_Module.szModuleName, L"%S", (PCHAR)(SLI_rVA_2_Offset(pImport->Name, uAddress) + uAddress));
		import_Module.uAddress_INT = SLI_rVA_2_Offset(pImport->OriginalFirstThunk, uAddress);
		import_Module.uAddress_IAT = SLI_rVA_2_Offset(pImport->FirstThunk, uAddress);

		//Now, here is the thing. We know which DLL it is, but getting out those functions imported is a trick part.
		//Because those thing are platform specified.
		//Originally, I want to use a different function to retrieve these information, but for the base of understanding, I will right it here.
		if (m_x64)
		{
			PIMAGE_THUNK_DATA64 pThunk_Data = nullptr;

			//In some rare cases, INT could be invalid. So, we are going to perform a test after the platform test.
			if (import_Module.uAddress_INT == -1)
			{
				pThunk_Data = (PIMAGE_THUNK_DATA64)(import_Module.uAddress_IAT + uAddress);
			}
			else
			{
				pThunk_Data = (PIMAGE_THUNK_DATA64)(import_Module.uAddress_INT + uAddress);
			}

			//This thing has no count number, so we loop it through.
			while (pThunk_Data->u1.AddressOfData)
			{
				SLI_IMPORT_DETAILS import_Details = { 0 };
				if (IMAGE_SNAP_BY_ORDINAL64(pThunk_Data->u1.AddressOfData))
				{
					//If it is imported by ordinal.
					wsprintf(import_Details.szFunctionName, L"-");
					import_Details.dwOrdinal = IMAGE_ORDINAL64(pThunk_Data->u1.Ordinal);
					import_Details.uAddress_rVA = pThunk_Data->u1.AddressOfData;
					import_Details.uAddress_Offset = SLI_rVA_2_Offset(pThunk_Data->u1.AddressOfData, uAddress);
					import_Module.m_vecDetais.push_back(import_Details);
				}
				else
				{
					//This indicates that the function is imported by name.
					PIMAGE_IMPORT_BY_NAME pBy_Name = (PIMAGE_IMPORT_BY_NAME)(SLI_rVA_2_Offset(pThunk_Data->u1.AddressOfData, uAddress) + uAddress);

					wsprintf(import_Details.szFunctionName, L"%S", pBy_Name->Name);
					import_Details.uAddress_rVA = pThunk_Data->u1.AddressOfData;
					import_Details.uAddress_Offset = SLI_rVA_2_Offset(pThunk_Data->u1.AddressOfData, uAddress);
					import_Details.dwOrdinal = pBy_Name->Hint;
					import_Module.m_vecDetais.push_back(import_Details);
				}
				//To the next structure.
				pThunk_Data++;
			}
		}
		else
		{
			PIMAGE_THUNK_DATA32 pThunk_Data = nullptr;

			if (import_Module.uAddress_INT == -1)
			{
				pThunk_Data = (PIMAGE_THUNK_DATA32)(import_Module.uAddress_IAT + uAddress);
			}
			else
			{
				pThunk_Data = (PIMAGE_THUNK_DATA32)(import_Module.uAddress_INT + uAddress);
			}

			while (pThunk_Data->u1.AddressOfData)
			{
				SLI_IMPORT_DETAILS import_Details = { 0 };
				if (IMAGE_SNAP_BY_ORDINAL32(pThunk_Data->u1.AddressOfData))
				{
					wsprintf(import_Details.szFunctionName, L"-");
					import_Details.dwOrdinal = IMAGE_ORDINAL32(pThunk_Data->u1.Ordinal);
					import_Details.uAddress_rVA = pThunk_Data->u1.AddressOfData;
					import_Details.uAddress_Offset = SLI_rVA_2_Offset(pThunk_Data->u1.AddressOfData, uAddress);
					import_Module.m_vecDetais.push_back(import_Details);
				}
				else
				{
					PIMAGE_IMPORT_BY_NAME pBy_Name = (PIMAGE_IMPORT_BY_NAME)(SLI_rVA_2_Offset(pThunk_Data->u1.AddressOfData, uAddress) + uAddress);

					wsprintf(import_Details.szFunctionName, L"%S", pBy_Name->Name);
					import_Details.uAddress_rVA = pThunk_Data->u1.AddressOfData;
					import_Details.uAddress_Offset = SLI_rVA_2_Offset(pThunk_Data->u1.AddressOfData, uAddress);
					import_Details.dwOrdinal = pBy_Name->Hint;
					import_Module.m_vecDetais.push_back(import_Details);
				}
				pThunk_Data++;
			}
		}
		m_vecImportTable.push_back(import_Module);

		//To the next imported DLL.
		pImport++;
	}
	return TRUE;
}



//BOOL CSLI_PE::SLI_Load_File(wchar_t* szFilePath)
//Load PE file into memory
//|-[szFilePath] is the absolute path to the file.
//Returns whether the file had been successfully loaded.
BOOL CSLI_PE::SLI_Load_File(wchar_t* szFilePath)
{
	//Prepare a handle.
	HANDLE hFile = NULL;
	DWORD dwSize = 0;
	DWORD dwRet = 0;
	PVOID lpFileImage = nullptr;

	//Open the file, and get its handle.
	hFile = CreateFile(szFilePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	//Get the target file's size.
	dwSize = GetFileSize(hFile, NULL);
	if (dwSize == INVALID_FILE_SIZE)
	{
		CloseHandle(hFile);
		return FALSE;
	}

	//Allocate a piece of memory to load the target file.
	lpFileImage = VirtualAlloc(NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);
	if (lpFileImage == NULL)
	{
		CloseHandle(hFile);
		return FALSE;
	}

	//Loading...
	if (!ReadFile(hFile, lpFileImage, dwSize, &dwRet, NULL))
	{
		CloseHandle(hFile);
		VirtualFree(lpFileImage, dwSize, MEM_RELEASE);
		return FALSE;
	}

	//Finally.
	m_LoadedAddress = (ULONGLONG)lpFileImage;
	m_dwSize = dwSize;
	CloseHandle(hFile);
	return TRUE;
}



//BOOL CSLI_PE::SLI_acquire_PE(ULONGLONG uAddress, DWORD dwSize)
//Outer function to perform all those above.
//|-[uAddress] is the loaded address.
//|-[dwSize] is the total size of the file.
//Returns if something is wrong during the process.
BOOL CSLI_PE::SLI_acquire_PE(wchar_t* szFilePath)
{
	//Load the file first.
	if (!SLI_Load_File(szFilePath))
	{
		MessageBox(NULL, L"Error when loading...", L"CSLI_PE_Error", MB_OK);
		return FALSE;
	}

	//Check its validation.
	if (!SLI_is_PE(m_LoadedAddress, m_dwSize))
	{
		return FALSE;
	}

	//Check Platform.
	if (SLI_is_x64(m_LoadedAddress))
	{
		m_x64 = TRUE;
	}
	else
	{
		m_x64 = FALSE;
	}

	//Get basic info.
	if (!SLI_acquire_Header_Info(m_LoadedAddress))
	{
		MessageBox(NULL, L"Error acquiring header info...", L"CSLI_PE_Error", MB_OK);
		return FALSE;
	}

	//Get data directory.
	if (!SLI_acquire_Data_Dir(m_LoadedAddress))
	{
		MessageBox(NULL, L"Error acquiring data directory...", L"CSLI_PE_Error", MB_OK);
		return FALSE;
	}

	//Get sections.
	if (!SLI_acquire_Sections(m_LoadedAddress))
	{
		MessageBox(NULL, L"Error acquiring sections...", L"CSLI_PE_Error", MB_OK);
		return FALSE;
	}

	//Get export table.
	if (!SLI_acquire_ExportTable(m_LoadedAddress))
	{
		MessageBox(NULL, L"Error acquiring export table...", L"CSLI_PE_Error", MB_OK);
		return FALSE;
	}

	//Get import table.
	if (!SLI_acquire_ImportTable(m_LoadedAddress))
	{
		MessageBox(NULL, L"Error acquiring import table...", L"CSLI_PE_Error", MB_OK);
		return FALSE;
	}

	return TRUE;

}