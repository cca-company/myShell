// myShell.type.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include <Windows.h>
#include <locale.h>

#pragma warning(disable:4996)

void TYPE(TCHAR* fileName)
{
	_tsetlocale(LC_ALL, _T("Korean"));
	TCHAR StringBuff[1024];

	FILE* filePtr = _tfopen(fileName, _T("rt"));
	if (filePtr == nullptr)
	{
		_fputts(_T("������ ã�� �� �����ϴ�"),stdout);
		return;
	}

	while (_fgetts(StringBuff, 1024, filePtr))
		_fputts(StringBuff, stdout);
}

int _tmain(int argc, _TCHAR* argv[])
{
	_tsetlocale(LC_ALL, _T("Korean"));

	if (argc < 2)
		return -1;
	TYPE(argv[1]);

	return 0;
}

