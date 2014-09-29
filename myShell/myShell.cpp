// OS.EX3.1.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#pragma warning(disable:4996)

TCHAR		ERROR_CMD[] = _T("'%s'��(��) ������ �� �ִ� ���α׷��� �ƴմϴ�. \n");
TCHAR		cmdString[STR_LEN];
TCHAR		cmdTokenList[CMD_TOKEN_NUM][STR_LEN];
TCHAR		seps[] = _T(" ,\t\n");
TCHAR*		cmdWordList[] = { _T("exit"), _T("pwd"), _T("echo"), _T("start"), _T("list"),
						_T("kill"), _T("dir"), _T("mkdir"), _T("rmdir") };
std::vector<TCHAR*> cmdLastWordList;

int			CmdStringToken(void);
int			CmdProcessing(int tokenNum);
TCHAR*		StrLower(TCHAR* pStr);
bool		isCmdWord(TCHAR* word);

int _tmain(int argc, _TCHAR* argv[])
{
	// �ѱ� �Է� ����
	_tsetlocale(LC_ALL, _T("Korean"));

	DWORD isExit;

	if (argc > 1)
	{
		for (int i = 1; i < argc; ++i)
		{
			wcscpy(cmdTokenList[i-1], argv[i]);
		}
	}

	while (1)
	{
		if (argc > 1)
		{
			isExit = CmdProcessing(0);
			argc = 0;
		}
		else
		{
			isExit = CmdProcessing(CmdStringToken());
		}
		if (isExit == TRUE)
		{
			_fputts(_T("��ɾ� ó���� �����մϴ�. \n"), stdout);
			break;
		}
	}
	return 0;
}

int CmdStringToken(void)
{
	TCHAR*	token;
	int		tokenNum;

	_fputts(_T("Best command prompt>> "), stdout);
	_getts_s(cmdString);

	cmdLastWordList.push_back(cmdString);
	_tcscpy(cmdTokenList[0], cmdString);
	token = _tcstok(cmdString, seps);
	tokenNum = 1;

	while (token != NULL)
	{
		_tcscpy(cmdTokenList[tokenNum++], StrLower(token));
		token = _tcstok(NULL, seps);
	}

	return tokenNum;

}

int CmdProcessing(int tokenNum)
{
	STARTUPINFO				si;
	PROCESS_INFORMATION		pi;
	TCHAR					cmdTempString[STR_LEN] = _T("\0");

	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));


	if (!_tcscmp(cmdTokenList[1], _T("exit")))
	{
		return TRUE;
	}
	else if (!_tcscmp(cmdTokenList[1], _T("pwd")))
	{
		// pwd :: ���� ���丮 ȣ��
		TCHAR dirPath[DIR_LEN];

		GetCurrentDirectory(DIR_LEN, dirPath);
		_fputts(dirPath, stdout);
		_fputts(_T("\n"), stdout);
	}
	else if (!_tcscmp(cmdTokenList[1], _T("echo")))
	{
		// echo :: ���ڿ� ���
		for (int i = 2; i < tokenNum; ++i)
		{
			wcscat(cmdTempString, cmdTokenList[i]);
			if (i<tokenNum - 1) wcscat(cmdTempString, _T(" "));
		}
		_fputts(cmdTempString, stdout);
		_fputts(_T("\n"), stdout);
	}
	else if (!_tcscmp(cmdTokenList[1], _T("start")))
	{
		// echo :: �� â���� ���μ��� ����

		// ��ū ��ģ ���ڿ� ����, ��ɾ� ��ū�� ��� myshell ����
		if (isCmdWord(cmdTokenList[2])) wcscat(cmdTempString, _T("myshell.exe "));
		
		for (int i = 2; i < tokenNum; ++i)
		{
			wcscat(cmdTempString, cmdTokenList[i]);
			if (i<tokenNum - 1) wcscat(cmdTempString, _T(" "));
		}

		if (!CreateProcess(NULL, cmdTempString, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
		{
			_tprintf(ERROR_CMD, cmdTokenList[2]);
			return 0;
		}

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	else if (!_tcscmp(cmdTokenList[1], _T("list")))
	{
		// list :: �������� ���μ��� ����Ʈ ���
		HANDLE hProcess = INVALID_HANDLE_VALUE;
		PROCESSENTRY32 pe32 = { 0 };

		hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		pe32.dwSize = sizeof(PROCESSENTRY32);

		if (Process32First(hProcess, &pe32))
		{
			do{
				_tprintf(_T("%d\t%s\n"), pe32.th32ProcessID, pe32.szExeFile);
			} while (Process32Next(hProcess, &pe32));
		}

		CloseHandle(hProcess);

	}
	else if (!_tcscmp(cmdTokenList[1], _T("kill")))
	{
		// kill :: ���� ���� ���μ��� ����
		HANDLE hProcess = INVALID_HANDLE_VALUE;
		DWORD PId;
		PId = (DWORD)wcstod(cmdTokenList[2], _T('\0'));
		// �־��� ���μ��� ���̵�� ���μ��� �ڵ��� ����

		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PId);
		if (hProcess == INVALID_HANDLE_VALUE)
		{
			_tprintf(_T("���μ����� ã�� �� �����ϴ�"));
			return 0;
		}

		TerminateProcess(hProcess, -1);
		CloseHandle(hProcess);

	}
	else if (!_tcscmp(cmdTokenList[1], _T("dir")))
	{
		// dir :: ���͸� ��ȸ
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind = INVALID_HANDLE_VALUE;
		TCHAR DirSpec[DIR_LEN];

		GetCurrentDirectory(DIR_LEN, DirSpec);
		wcscat(DirSpec, _T("\\*"));
		hFind = FindFirstFile(DirSpec, &FindFileData);

		if (hFind == INVALID_HANDLE_VALUE)
		{
			_tprintf(_T("������ ã�� �� �����ϴ�"));
			return 0;
		}

		do{
			_tprintf(_T("%s\n"),FindFileData.cFileName);
		} while (FindNextFile(hFind, &FindFileData) != 0);
		FindClose(hFind);
	}
	else if (!_tcscmp(cmdTokenList[1], _T("mkdir")))
	{
		// mkdir :: ���͸� ����
		TCHAR DirSpec[DIR_LEN];

		GetCurrentDirectory(DIR_LEN, DirSpec);

		_stprintf(DirSpec, _T("%s\\%s"), DirSpec, cmdTokenList[2]);
		CreateDirectory(DirSpec,NULL);
	}
	else if (!_tcscmp(cmdTokenList[1], _T("del")))
	{
		// mkdir :: ���͸� ����
		TCHAR DirSpec[DIR_LEN];

		GetCurrentDirectory(DIR_LEN, DirSpec);

		_stprintf(DirSpec, _T("%s\\%s"), DirSpec, cmdTokenList[1]);
		RemoveDirectory(DirSpec);
	}
	else if (!_tcscmp(cmdTokenList[1], _T("ren")))
	{
		// ren :: ���͸� �̸� ����
		TCHAR DirSpec[DIR_LEN];
		TCHAR TempName[DIR_LEN];
		TCHAR ModName[DIR_LEN];

		GetCurrentDirectory(DIR_LEN, DirSpec);

		_stprintf(TempName, _T("%s\\%s"), DirSpec, cmdTokenList[1]);
		_stprintf(ModName, _T("%s\\%s"), DirSpec, cmdTokenList[2]);

		_trename(TempName,ModName);
	}
	else if (!_tcscmp(cmdTokenList[1], _T("type")))
	{
		// type :: ���� ���� �����Ͽ� ���

		if (!_tcscmp(cmdTokenList[3], _T("|")))
		{
			// sort ������

			HANDLE hReadPipe, hWritePipe;

			SECURITY_ATTRIBUTES pipeSA = {
				sizeof(SECURITY_ATTRIBUTES), NULL, TRUE
			};

			CreatePipe(&hReadPipe, &hWritePipe, &pipeSA, 0);

			/* process type�� ���� ���� */
			STARTUPINFO siType = { 0, };
			PROCESS_INFORMATION piType;
			siType.cb = sizeof(siType);

			siType.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
			siType.hStdError = GetStdHandle(STD_ERROR_HANDLE);
			siType.hStdOutput = hWritePipe; // ��� �����̷���
			siType.dwFlags |= STARTF_USESTDHANDLES;

			_stprintf(cmdTempString, _T("%s %s"), _T("myShell.type.exe"), cmdTokenList[2]);
			if (!CreateProcess(NULL, cmdTempString, NULL, NULL, TRUE, 0, NULL, NULL, &siType, &piType))
			{
				_tprintf(ERROR_CMD, cmdTokenList[0]);
				return 0;
			}

			CloseHandle(piType.hThread);
			CloseHandle(hWritePipe);

			/* process sort�� ���� ���� */
			STARTUPINFO siSort = { 0, };
			PROCESS_INFORMATION piSort;
			siSort.cb = sizeof(siSort);

			siSort.hStdInput = hReadPipe; // �Է� �����̷���
			siSort.hStdError = GetStdHandle(STD_ERROR_HANDLE);
			siSort.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
			siSort.dwFlags |= STARTF_USESTDHANDLES;

			_tcscpy(cmdTempString, _T("myShell.sort.exe"));
			if (!CreateProcess(NULL, cmdTempString, NULL, NULL, TRUE, 0, NULL, NULL, &siSort, &piSort))
			{
				_tprintf(ERROR_CMD, cmdTokenList[0]);
				return 0;
			}

			CloseHandle(piSort.hThread);
			CloseHandle(hReadPipe);

			WaitForSingleObject(piType.hProcess, INFINITE);
			WaitForSingleObject(piSort.hProcess, INFINITE);

			CloseHandle(piType.hProcess);
			CloseHandle(piSort.hProcess);

		}
		else
		{
			_stprintf(cmdTempString, _T("%s %s"), _T("myShell.type.exe"), cmdTokenList[2]);
			
			STARTUPINFO si = { 0, };
			PROCESS_INFORMATION pi;
			si.cb = sizeof(si);

			if (!CreateProcess(NULL, cmdTempString, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
			{
				_tprintf(ERROR_CMD, cmdTokenList[0]);
				return 0;
			}
			WaitForSingleObject(pi.hProcess, INFINITE);

			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);

		}
		_tprintf(_T("\n"));
		
	}
	else
	{
		// ���μ��� ����

		if (!CreateProcess(NULL, cmdTokenList[0], NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi))
		{
			_tprintf(ERROR_CMD, cmdTokenList[0]);
			return 0;
		}

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		
	}
	return 0;

}

TCHAR* StrLower(TCHAR* pStr)
{
	TCHAR *ret = pStr;

	while (*pStr)
	{
		if (_istupper(*pStr))
			*pStr = _totlower(*pStr);
		pStr++;
	}

	return ret;
}

bool isCmdWord(TCHAR* word)
{

	for (int i = 0; i < _countof(cmdWordList); ++i)
	{
		if (!_tcscmp(word, cmdWordList[i]))	return true;
	}

	return false;
}