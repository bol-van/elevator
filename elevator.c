#include <windows.h>
#include <stdio.h>
#include <shellapi.h>
#include <winternl.h>

#include <winnt.h>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	LPWSTR *szArglist, szParam, p;
	int nArgs;
	int i;
	UINT size;
	SHELLEXECUTEINFOW si;
	BOOL b;

	szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
	if (!szArglist)
	{
		MessageBoxW(NULL, L"CommandLineToArgvW", L"Error", MB_ICONERROR);
		return 1;
	}
	if (nArgs < 2)
	{
		MessageBoxW(NULL, L"elevator <exe_file> <parameters>", L"Error", MB_ICONERROR);
		return 1;
	}

	for (i = 2, size = nArgs-2; i < nArgs; i++) size += wcslen(szArglist[i]);
	if (size)
	{
		szParam = LocalAlloc(LMEM_FIXED, size * sizeof(WCHAR));
		if (!szParam)
		{
			LocalFree(szArglist);
			return 2;
		}
		for (i = 2, p = szParam, *szParam = 0; i < nArgs; i++)
		{
			size = wcslen(szArglist[i]);
			memmove(p, szArglist[i], size * sizeof(WCHAR));
			p += size;
			*p++ = (i == (nArgs - 1)) ? 0 : L' ';
		}
	}
	else
		szParam = NULL;

	ZeroMemory(&si, sizeof(si));
	si.cbSize = sizeof(si);
	si.lpVerb = L"runas";
	si.lpFile = szArglist[1];
	si.lpParameters = szParam;
	si.nShow = SW_SHOW;

	//MessageBoxW(NULL, szParam, L"", 0);

	b=ShellExecuteExW(&si);

	if (szParam) LocalFree(szParam);
	LocalFree(szArglist);

	return b ? 0 : 3;
}

void APIENTRY wWinMainCRTStartup(LPVOID pArgPeb)
{
	ExitProcess(wWinMain(NULL, NULL, GetCommandLineW(), SW_SHOW));
}

