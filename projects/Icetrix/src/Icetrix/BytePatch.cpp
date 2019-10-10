#include "BytePatch.h"

void Icetrix::BytePatch::Patch(Icetrix::Patch patch, uint64_t baseAddress)
{
	blackbone::ProcessMemory* memory = &(Icetrix::Process::GetInstance())->memory();

	if (NT_SUCCESS(memory->Protect(baseAddress + patch.address, patch.size, PAGE_EXECUTE_READWRITE)))
	{
		for (int i = 0; i < patch.size; i++)
		{
			auto newAddress = baseAddress + patch.address + i;
			memory->Write(newAddress, patch.patch[i]);
		}
	}
}

void Icetrix::BytePatch::Unpatch(Icetrix::Patch patch, uint64_t baseAddress)
{
	blackbone::ProcessMemory* memory = &(Icetrix::Process::GetInstance())->memory();

	if (NT_SUCCESS(memory->Protect(baseAddress + patch.address, patch.size, PAGE_EXECUTE_READWRITE)))
	{
		for (int i = 0; i < patch.size; i++)
		{
			auto newAddress = baseAddress + patch.address + i;
			memory->Write(newAddress, patch.original[i]);
		}
	}
}