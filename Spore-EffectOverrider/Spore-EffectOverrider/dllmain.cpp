// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <Spore\Effects.h>
#include <Spore/Swarm/cEffectsManager.h>
#include "EffectOverrider.h"
#include <stdexcept>

void Initialize()
{
	// This method is executed when the game starts, before the user interface is shown
	// Here you can do things such as:
	//  - Add new cheats
	//  - Add new simulator classes
	//  - Add new game modes
	//  - Add new space tools
	//  - Change materials
	//Swarm::cEffectsManager::GetDirectoryAndEffectIndex
	EffectOverrider* overrider = new EffectOverrider();
}

member_detour(EffectOverrideDetour, Swarm::cEffectsManager, int(uint32_t, uint32_t))
{
	int detoured(uint32_t instanceId, uint32_t groupId)
	{
		uint32_t effID = instanceId;
		uint32_t prevEffID = instanceId;
		int cap = 0;
		do
		{
			prevEffID = effID;
			effID = EffectOverrider::GetOverrideEffect(effID);
			cap++;
			if (cap > 1000000)
			{
				MessageBox(NULL, LPCWSTR(u"Endless loop achieved; make sure your effect isn't overriden by what it's ovewriting!"), LPCWSTR(u"Error overriding effect"), 0x00000010L);
				throw std::exception("Endless loop achieved; make sure your effect isn't overriden by what it's ovewriting!");
			}
		} while (effID != prevEffID);


		return original_function(this, effID, groupId);
	}
};

void Dispose()
{
	// This method is called when the game is closing
}

void AttachDetours()
{
	EffectOverrideDetour::attach(GetAddress(Swarm::cEffectsManager, GetDirectoryAndEffectIndex));
	// Call the attach() method on any detours you want to add
	// For example: cViewer_SetRenderType_detour::attach(GetAddress(cViewer, SetRenderType));
}




// Generally, you don't need to touch any code here
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		ModAPI::AddPostInitFunction(Initialize);
		ModAPI::AddDisposeFunction(Dispose);

		PrepareDetours(hModule);
		AttachDetours();
		CommitDetours();
		break;

	case DLL_PROCESS_DETACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

