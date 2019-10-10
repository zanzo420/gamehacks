#include "pch.h"
#include "Icetrix/Process.h"
#include "Game/Game.h"
#include "Icetrix/BytePatch.h"
#include "Patches.h"
#include "Icetrix/Feature.h"

bool Patches::OnAttach()
{
	blackbone::Process* process = Icetrix::Process::GetInstance();
	Icetrix::Features* features = Icetrix::Features::GetInstance();

	if (process->valid())
	{
		auto mainModule = process->modules().GetMainModule();

		for (Icetrix::Patch patch : patches)
		{
			features->Push(new Icetrix::Feature{ patch.name, false });
		}
	}
	else
	{
		std::cout << "[!] Patches: No valid process found";
	}

	features->Push(new Icetrix::Feature{ "Pseudo Unlimited Health", false });

	return true;
}

bool Patches::OnUpdate()
{
	blackbone::Process* process = Icetrix::Process::GetInstance();
	Icetrix::Features* features = Icetrix::Features::GetInstance();

	if (process->valid())
	{
		auto mainModule = process->modules().GetMainModule();

		for (Icetrix::Patch patch : patches)
		{
			if (features->Get(patch.name)->enabled)
			{
				Icetrix::BytePatch::Patch(patch, mainModule->baseAddress);
			}
			else
			{
				Icetrix::BytePatch::Unpatch(patch, mainModule->baseAddress);
			}
		}
	}
	else
	{
		std::cout << "[!] Update Patches: No valid process found";
	}

	if (features->Get("Pseudo Unlimited Health")->enabled)
	{
		g.GetLocalPlayer()->m_Health = 100000;
		g.GetLocalPlayer()->m_Vest = 100000;
	}

	return true;
}

void Patches::OnDetach()
{
	blackbone::Process* process = Icetrix::Process::GetInstance();

	if (process->valid())
	{
		auto mainModule = process->modules().GetMainModule();

		for (Icetrix::Patch patch : patches)
		{
			Icetrix::BytePatch::Unpatch(patch, mainModule->baseAddress);
		}
	}
	else
	{
		std::cout << "[!] Patches: No valid process found";
	}

	g.GetLocalPlayer()->m_Health = 100;
	g.GetLocalPlayer()->m_Vest = 100;
}
