#include <numeric>
#include <cctype>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>

#include <Windows.h>
#include <ShlObj.h>

unsigned int __cdecl sub_401140(unsigned __int16 *a1)
{
	unsigned int result; // eax@2
	unsigned __int16 v2; // ax@3
	int v3; // esi@3
	int i; // edi@3
	int v5; // ecx@4

	if (a1)
	{
		v2 = *a1;
		v3 = 0;
		for (i = 0; v2; v3 = 31 * v3 + v5)
		{
			++i;
			v5 = (unsigned __int16)toupper(v2);
			v2 = a1[i];
		}
		result = v3 + v3 / 0xFFFFFFFFu;
	} else
	{
		result = 0;
	}
	return result;
}

std::uint32_t saveFolderHash(const std::wstring & str) {
	std::uint32_t result = 0;
	result = std::accumulate(std::begin(str), std::end(str), 0, [](std::uint32_t l, std::uint32_t r) {
		return 31 * l + toupper(r);
	});
	return result + result / 0xFFFFFFFFu;
}

std::wstring findSaveFolder(std::uint32_t targetHash = 0x4e8a110) {

	wchar_t documentsFolder[MAX_PATH];
	if (SHGetFolderPathW(NULL, CSIDL_MYDOCUMENTS, NULL, 0, documentsFolder) != S_OK) {
		return L"";
	}
	auto saveFolder = std::wstring{ documentsFolder } + L"\\NBGI\\DarkSouls\\";

	if (targetHash == 0) {
		return saveFolder;
	}

	WIN32_FIND_DATAW findData;
	HANDLE findHandle = FindFirstFileW((saveFolder + L"*").c_str(), &findData);
	if (findHandle != INVALID_HANDLE_VALUE) {
		do {
			if (saveFolderHash(findData.cFileName) == targetHash) {
				return saveFolder + findData.cFileName + L"\\";
			}
		} while (FindNextFileW(findHandle, &findData));
		std::wstringstream ss;
		ss << std::setfill(L'0') << std::setw(8) << std::hex << targetHash;
		std::wstring targetHashString;
		ss >> targetHashString;
		return saveFolder + targetHashString + L"\\";
	}

	return saveFolder;

}

int main() {

	wchar_t * x = L"Test";
	std::cout << std::hex << sub_401140(reinterpret_cast<unsigned __int16 *>(x)) << std::endl;
	std::cout << std::hex << saveFolderHash(x) << std::endl;

	std::wcout << findSaveFolder() << std::endl;
	std::wcout << findSaveFolder(0x234b33f) << std::endl;
	std::wcout << findSaveFolder(0) << std::endl;

	return 0;
}
