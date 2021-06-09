// FirstService.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Windows.h"

#define FIRST_SERVICE_NAME  _T("FirstService")
#define FIRST_SERVICE_PATH  _T("C:\\FirstDriver\\FirstDriver.sys")


PCWSTR ServiceStateToString(_In_ const UINT32 state)
{
	switch (state)
	{
	case SERVICE_STOPPED:
		return L"SERVICE_STOPPED";
	case SERVICE_START_PENDING:
		return L"SERVICE_START_PENDING";
	case SERVICE_STOP_PENDING:
		return L"SERVICE_STOP_PENDING";
	case SERVICE_RUNNING:
		return L"SERVICE_RUNNING";
	case SERVICE_CONTINUE_PENDING:
		return L"SERVICE_CONTINUE_PENDING";
	case SERVICE_PAUSE_PENDING:
		return L"SERVICE_PAUSE_PENDING";
	case SERVICE_PAUSED:
		return L"SERVICE_PAUSED";
	}

	return L"Unknown";
}

int main()
{
	UINT32 status = NO_ERROR;
	SERVICE_STATUS srvStatus = {0};
	UINT32 serviceState = SERVICE_STOPPED;
	SC_HANDLE schSCManager = nullptr;
	SC_HANDLE svcHandle = nullptr;
	// Open the local default service control manager database
	schSCManager = OpenSCManager(nullptr, nullptr, SC_MANAGER_CONNECT |
	                             SC_MANAGER_CREATE_SERVICE);
	if (schSCManager == nullptr)
	{
		wprintf(L"OpenSCManager failed w/err 0x%08lx\n", GetLastError());
		goto Cleanup;
	}

	svcHandle = CreateService(
		schSCManager, // SCManager database
		FIRST_SERVICE_NAME, // Name of service
		FIRST_SERVICE_NAME, // Name to display
		SERVICE_ALL_ACCESS, // Desired access
		SERVICE_KERNEL_DRIVER, // Service type
		SERVICE_DEMAND_START, // Service start type
		SERVICE_ERROR_IGNORE, // Error control type
		FIRST_SERVICE_PATH, // Service's binary
		nullptr, // No load ordering group
		nullptr, // No tag identifier
		nullptr, // Dependencies
		nullptr, // Service running account
		nullptr // Password of the account
	);
	if (svcHandle == nullptr)
	{
		wprintf(L"CreateService failed w/err 0x%08lx\n", GetLastError());
		goto Cleanup;
	}

	//准备启动服务
	if (!StartService(svcHandle, 0, nullptr))
	{
		status = GetLastError();

		if (status != ERROR_SERVICE_ALREADY_RUNNING)
			wprintf(L"HlprServiceStopStart: StartService() [status: %#x][Starting %s]",
			        status,
			        FIRST_SERVICE_NAME);
	}
	wprintf(L"Start service success.\n");
	getchar();
	if (!ControlService(svcHandle,
	                    SERVICE_CONTROL_STOP,
	                    &srvStatus))
	{
		status = GetLastError();

		if (status == NO_ERROR ||
			status == ERROR_INVALID_SERVICE_CONTROL ||
			status == ERROR_SERVICE_CANNOT_ACCEPT_CTRL ||
			status == ERROR_SERVICE_NOT_ACTIVE)
			wprintf(
				L"PrvHlprServiceControl : ControlService() [status: %#x][Stopping %s][ServiceType: %#x][CurrentState: %s][ControlsAccepted: %#x][Win32ExitCode: %#x][ServiceSpecificExitCode: %#x][CheckPoint: %#x][WaitHint: %#x]",
				status,
				FIRST_SERVICE_NAME,
				srvStatus.dwServiceType,
				ServiceStateToString(srvStatus.dwCurrentState),
				srvStatus.dwControlsAccepted,
				srvStatus.dwWin32ExitCode,
				srvStatus.dwServiceSpecificExitCode,
				srvStatus.dwCheckPoint,
				srvStatus.dwWaitHint);
	}
	wprintf(L"Stop service success.\n");
	DeleteService(svcHandle);
	return 0;

Cleanup:
	// Centralized cleanup for all allocated resources.
	if (schSCManager)
	{
		CloseServiceHandle(schSCManager);
		schSCManager = nullptr;
	}

	if (svcHandle)
	{
		CloseServiceHandle(svcHandle);
		svcHandle = nullptr;
	}
	return -1;
}
