// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <Spore\Effects.h>
#include <Spore/Swarm/cEffectsManager.h>
#include "EffectOverrider.h"
#include <stdexcept>

void Initialize()
{
	//Initialize the effect overrider.
	EffectOverrider* overrider = new EffectOverrider();
}

member_detour(EffectOverrideDetour, Swarm::cEffectsManager, int(uint32_t, uint32_t))
{
	int detoured(uint32_t instanceId, uint32_t groupId) //Detouring the function for obtaining effect indexes...
	{
		uint32_t effID = EffectOverrider::GetOverrideEffect(instanceId); //Get the new instance ID for the effect.
		return original_function(this, effID, groupId); //And call the original function with the new instance ID.
	}
};

void Dispose()
{
	// This method is called when the game is closing
}

void AttachDetours()
{
	//Attach the detour.
	EffectOverrideDetour::attach(GetAddress(Swarm::cEffectsManager, GetDirectoryAndEffectIndex));
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

