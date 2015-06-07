#include <numeric>
#include <cctype>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>

#include <Windows.h>
#include <ShlObj.h>

#include <MinHook.h>

#include <d3d9.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxerr.lib")

template <typename T>
inline MH_STATUS MH_CreateHookEx(LPVOID pTarget, LPVOID pDetour, T** ppOriginal) {
	return MH_CreateHook(pTarget, pDetour, reinterpret_cast<LPVOID*>(ppOriginal));
}

template <typename T>
inline MH_STATUS MH_CreateHookApiEx(LPCWSTR pszModule, LPCSTR pszProcName, LPVOID pDetour, T** ppOriginal) {
	return MH_CreateHookApi(pszModule, pszProcName, pDetour, reinterpret_cast<LPVOID*>(ppOriginal));
}

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

auto TrueSleep = &Sleep;
void __stdcall DetouredSleep(DWORD dwMilliseconds) {
	std::cout << "Wanted to sleep " << dwMilliseconds << ", instead sleepan " << 2 * dwMilliseconds << std::endl;
	TrueSleep(2 * dwMilliseconds);
	std::cout << "done" << std::endl;
}

int main() {

	Direct3DCreate9(0);

	MH_Initialize();

	if (MH_CreateHookEx(&Sleep, &DetouredSleep, &TrueSleep) != MH_OK) {
		std::cerr << "Couldn't hook Sleep" << std::endl;
		return 1;
	}

	if (MH_EnableHook(&Sleep) != MH_OK) {
		std::cerr << "Couldn't enable Sleep hook" << std::endl;
		return 1;
	}

	Sleep(2000);

	wchar_t * x = L"Test";
	std::cout << std::hex << sub_401140(reinterpret_cast<unsigned __int16 *>(x)) << std::endl;
	std::cout << std::hex << saveFolderHash(x) << std::endl;

	std::wcout << findSaveFolder() << std::endl;
	std::wcout << findSaveFolder(0x234b33f) << std::endl;
	std::wcout << findSaveFolder(0) << std::endl;

	MH_Uninitialize();

	return 0;
}
