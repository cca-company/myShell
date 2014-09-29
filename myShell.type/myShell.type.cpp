// myShell.type.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
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
		_fputts(_T("파일을 찾을 수 없습니다"),stdout);
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

