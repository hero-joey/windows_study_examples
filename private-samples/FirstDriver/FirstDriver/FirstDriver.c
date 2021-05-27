#include "ntddk.h"

VOID DriverUnload(PDRIVER_OBJECT DriverObject)
{
	if (DriverObject != NULL)
	{
		DbgPrint("[%ws] Driver Unload, Driver Object Address:%p\n", __FUNCTION__, DriverObject);
	}
}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	DbgPrint("[%ws] Hello Kernel World\n", __FUNCTION__);
	if (RegistryPath != NULL)
	{
		DbgPrint("[%ws] Driver RegistryPath:%wZ\n", __FUNCTION__, RegistryPath);
	}

	if (DriverObject != NULL)
	{
		DbgPrint("[%ws] Driver Object Address:%p\n", __FUNCTION__, DriverObject);
		DriverObject->DriverUnload = DriverUnload;
	}

	return STATUS_SUCCESS;
}