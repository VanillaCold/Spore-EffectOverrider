#include "stdafx.h"
#include "EffectOverrider.h"
#include <stdexcept>

EffectOverrider::EffectOverrider()
{
	try
	{
		SetupOverrides();
	}
	catch (std::exception except)
	{
		const char* text = except.what();
		wstring report;
		report.assign_convert(text);
		MessageBox(NULL, report.c_str(), LPCWSTR(u"Error initializing effect replacer"), 0x00000010L);
	}
}


EffectOverrider::~EffectOverrider()
{
}

// For internal use, do not modify.
int EffectOverrider::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int EffectOverrider::Release()
{
	return DefaultRefCounted::Release();
}

// You can extend this function to return any other types your class implements.
void* EffectOverrider::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(EffectOverrider);
	return nullptr;
}

uint32_t EffectOverrider::GetOverrideEffect(uint32_t instanceID)
{
	auto effectFind = effectOverrides.find(instanceID);
	
	if (effectFind != effectOverrides.end())
	{
		uint32_t newEffect = effectFind->second;
		return newEffect;
	}
	return instanceID;
}

void EffectOverrider::SetupOverrides()
{
	vector<uint32_t> props;
	if (PropManager.GetPropertyListIDs(id("EffectOverrides"), props))
	{
		for each (uint32_t propID in props)
		{
			PropertyListPtr propList;
			PropManager.GetPropertyList(propID, id("EffectOverrides"), propList);
			uint32_t* effectsToOverride;
			uint32_t* overrideEffects;
			size_t numEffectsToOverride;
			size_t numOverrideEffects;

			App::Property::GetArrayUInt32(propList.get(), id("ReplaceEffects"), numEffectsToOverride, effectsToOverride);
			App::Property::GetArrayUInt32(propList.get(), id("EffectsToReplaceWith"), numOverrideEffects, overrideEffects);
			if (numEffectsToOverride == numOverrideEffects)
			{
				for (int i = 0; i < numEffectsToOverride; i++)
				{
					if (EffectsManager.HasVisualEffect(overrideEffects[i], 0))
					{
						effectOverrides.emplace(effectsToOverride[i], overrideEffects[i]);
					}
					else
					{
						throw std::length_error("Replacement effect does not exist!");
					}
				}
			}
			else
			{
				throw std::length_error("Number of replaced and replacement effects do not match.");
			}

		}
		
	}
}

map<uint32_t, uint32_t> EffectOverrider::effectOverrides;