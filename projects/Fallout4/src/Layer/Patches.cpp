#include "pch.h"
#include "Patches.h"

void Patches::OnAttach(const Icetrix::LayerEvent::Attach &attach)
{
	blackbone::Process* process = Icetrix::Process::GetInstance();
	Icetrix::Features* features = Icetrix::Features::GetInstance();

	if (process->valid())
	{
		auto mainModule = process->modules().GetMainModule();

		for (Icetrix::Memory::Patch patch : patches)
		{
			features->Push(new Icetrix::Feature{ patch.name, false });
		}
	}
	else
	{
		std::cout << "[!] Patches: No valid process found";
	}
}

void Patches::OnUpdate(const Icetrix::LayerEvent::Update &update)
{
	blackbone::Process* process = Icetrix::Process::GetInstance();
	Icetrix::Features* features = Icetrix::Features::GetInstance();

	if (process->valid())
	{
		auto mainModule = process->modules().GetMainModule();

		for (Icetrix::Memory::Patch patch : patches)
		{
			if (features->Get(patch.name)->enabled)
			{
				Icetrix::Memory::BytePatch::Patch(patch, mainModule->baseAddress);
			}
			else
			{
				Icetrix::Memory::BytePatch::Unpatch(patch, mainModule->baseAddress);
			}
		}
	}
	else
	{
		std::cout << "[!] Update Patches: No valid process found";
	}
}

void Patches::OnDetach(const Icetrix::LayerEvent::Detach &detach)
{
	blackbone::Process* process = Icetrix::Process::GetInstance();

	if (process->valid())
	{
		auto mainModule = process->modules().GetMainModule();

		for (Icetrix::Memory::Patch patch : patches)
		{
			Icetrix::Memory::BytePatch::Unpatch(patch, mainModule->baseAddress);
		}
	}
	else
	{
		std::cout << "[!] Patches: No valid process found";
	}
}
