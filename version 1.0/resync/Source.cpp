#include <stdio.h>
#include <Windows.h>
#include <fstream>
#include <tchar.h>
using namespace std;
#define IDR_TASK2 102
bool Filecheck(const char* filenamepath);
bool Filecheck(const char* filenamepath)
{
	fstream file;
	file.open(filenamepath, fstream::out);
	if(file.is_open() == true)
	{
		file.close();
		return true;
	}
	else
	{
		file.close();
		return false;
	}
}
int main(int argc, char** argv)
{
	if(argc != 1)
	{ 
		for (int i = 0; i <= argc; i++)
		{
			printf("argc:%d\nargv[%d]=%s", argc, i, argv[i]);
			if (!strcmp(argv[i], "/setup") && strcmp(argv[i], "/task") && strcmp(argv[i], "/startup"))
			{
				char windir[500];
				char buff2[500],file[500],cdrive[32];
				TCHAR filex[200];
				HKEY keyhandle;
				GetEnvironmentVariable("systemroot", windir, sizeof(windir));
				GetEnvironmentVariable("SystemDrive", cdrive, sizeof(cdrive));
				sprintf_s(buff2, "%s\\System32\\schtasks.exe /create /F /sc ONCE /ST 12:00 /SD 01/01/2003 /tn \"Resync On Startup\" /tr \"%s\\resync.exe /task\" /ru System\0",windir,cdrive);
				sprintf_s(file, "%s\\resync.exe", cdrive);
				sprintf_s(filex, "%s\\resync.exe /startup\0", cdrive);
				PROCESS_INFORMATION pi;
				STARTUPINFO si = { sizeof(STARTUPINFO) };
				si.cb = sizeof(STARTUPINFO);
				if (!CreateProcess(NULL, buff2, NULL, NULL, FALSE, CREATE_NO_WINDOW|NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi))
				{
					exit(-1);
				}
				WaitForSingleObject(pi.hProcess, INFINITE);
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
				CopyFile(argv[0], file, FALSE);
				if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE| KEY_WOW64_64KEY, &keyhandle) != ERROR_SUCCESS)
				{
					exit(-1);
				}
				if (RegSetValueEx(keyhandle, "Resync", 0, REG_SZ, (PBYTE)&filex, sizeof(filex)) != ERROR_SUCCESS)
				{
					exit(-1);
				}
				CloseHandle(keyhandle);
				MessageBox(NULL, "Installation Was Completed.", "Completed Install", MB_OK | MB_ICONINFORMATION | MB_DEFBUTTON1);
				exit(0);
				
			}
			else if (!strcmp(argv[i], "/task") && strcmp(argv[i], "/setup") && strcmp(argv[i], "/startup"))
			{
				char buff[500];
				PROCESS_INFORMATION pi;
				STARTUPINFO si = { sizeof(STARTUPINFO) };
				sprintf_s(buff, "net start w32time\0");
				if (!CreateProcess(NULL, buff, NULL, NULL, FALSE, CREATE_NO_WINDOW | NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi))
				{
					exit(-1);
				}
				WaitForSingleObject(pi.hProcess,INFINITE);
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
				memcpy(buff, 0, sizeof(buff));
				sprintf_s(buff, "w32tm /resync /nowait\0");
				if (!CreateProcess(NULL, buff, NULL, NULL, FALSE, CREATE_NO_WINDOW | NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi))
				{
					exit(-1);
				}
				WaitForSingleObject(pi.hProcess, INFINITE);
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
				memcpy(buff, 0, sizeof(buff));
				sprintf_s(buff,"net stop w32time\0");
				exit(0);
			}
			else if (!strcmp(argv[i], "/startup") && strcmp(argv[i], "/setup") && strcmp(argv[i], "/task"))
			{
				PROCESS_INFORMATION pi;
				STARTUPINFO si = { sizeof(STARTUPINFO) };
				char buff[500];
				sprintf_s(buff, "schtasks /run /I /tn \"Resync On Startup\"\0");
				if (!CreateProcess(NULL, buff, NULL, NULL, FALSE, CREATE_NO_WINDOW | NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi))
				{
					exit(-1);
				}
				WaitForSingleObject(pi.hProcess, INFINITE);
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
				exit(0);
			}
			else if (!strcmp(argv[i], "/uninstall") && strcmp(argv[i], "/task") && strcmp(argv[i], "/startup") && strcmp(argv[i], "/setup"))
			{
				char windir[500];
				char buff2[500], file[500], cdrive[32];
				HKEY keyhandle;
				GetEnvironmentVariable("systemroot", windir, sizeof(windir));
				GetEnvironmentVariable("SystemDrive", cdrive, sizeof(cdrive));
				sprintf_s(buff2, "%s\\System32\\schtasks.exe /delete /F /tn \"Resync On Startup\"\0", windir);
				sprintf_s(file, "%s\\resync.exe", cdrive);
				PROCESS_INFORMATION pi;
				STARTUPINFO si = { sizeof(STARTUPINFO) };
				si.cb = sizeof(STARTUPINFO);
				if (!CreateProcess(NULL, buff2, NULL, NULL, FALSE, CREATE_NO_WINDOW | NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi))
				{
					exit(-1);
				}
				WaitForSingleObject(pi.hProcess, INFINITE);
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
				DeleteFile(file);
				RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE| KEY_WOW64_64KEY, &keyhandle);
				RegDeleteValue(keyhandle, "Resync");
				CloseHandle(keyhandle);
				remove(argv[0]);
				MessageBox(NULL, "Uninstall was completed.", "Completed uninstall", MB_OK | MB_ICONINFORMATION | MB_DEFBUTTON1);
				exit(0);
			}
			else {
				if ((i+1) > argc)
				{
					goto main;
				}
			}
		}
	}
	main:
	if (MessageBox(NULL, "This is program for resync time on startup.\nThis action will create task for resync(to not request UAC) and program will copy themselv into C:\\.\nIf you accept the program will ask for UAC permission to take necessary actions.\nAre you sure? If you hit cancel, program will stop and you can run this program again.", "WARNING!!! Initial Setup", MB_OKCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2) == IDOK)
	{
		SHELLEXECUTEINFO shExInfo = { 0 };
		shExInfo.cbSize = sizeof(shExInfo);
		shExInfo.fMask = SEE_MASK_DEFAULT;
		shExInfo.hwnd = 0;
		shExInfo.lpVerb = _T("runas");
		shExInfo.lpFile = _T(argv[0]);
		shExInfo.lpParameters = "/setup";
		shExInfo.lpDirectory = 0;
		shExInfo.nShow = SW_SHOW;
		shExInfo.hInstApp = 0;
		if (ShellExecuteEx(&shExInfo) == TRUE)
		{
			exit(0);
		}
		else
		{
			MessageBox(NULL, "Something went wrong!!\nOpen Issiue on github https://github.com/rhscz/resynconstartup", "ERROR!!", MB_OK);
			exit(-1);
		}
	}
	else
	{
		exit(0);
	}
}