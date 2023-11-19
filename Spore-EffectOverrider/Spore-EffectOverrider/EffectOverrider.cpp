#include "stdafx.h"
#include "EffectOverrider.h"
#include <stdexcept>

EffectOverrider::EffectOverrider()
{
	try //Try the following.
	{
		SetupOverrides(); //Setup the effects.
	}
	catch (std::exception except) //If there's an error,
	{
		const char* text = except.what(); //Get the text.
		wstring report; //then, make a WString 
		report.assign_convert(text); //and assign it from the text.
		MessageBox(NULL, report.c_str(), LPCWSTR(u"Error initializing effect replacer"), 0x00000010L); //Then, use this wstring in a message box.
	}
}

EffectOverrider::~EffectOverrider()
{
}


uint32_t EffectOverrider::GetOverrideEffect(uint32_t instanceID)
{
	auto effectFind = effectOverrides.find(instanceID); //First, find the effect to override it with.
	
	if (effectFind != effectOverrides.end()) //If the effect is in the override list,
	{
		uint32_t newEffect = effectFind->second; //Get the ID of the new effect.

		numIterations++; //Increase the number of iterations, used to prevent endless loops.
		
		if (numIterations > 1000000) //If an endless loop is detected:
		{
			MessageBox(NULL, LPCWSTR(u"Endless loop achieved; make sure your effect isn't overriden by what it's ovewriting!"), LPCWSTR(u"Error overriding effect"), 0x00000010L); //Inform the user.
			throw std::exception("Endless loop achieved; make sure your effect isn't overriden by what it's ovewriting!"); //And then, throw an exception.
		}
		//If no loop was achieved yet,
		return GetOverrideEffect(newEffect); //Recursively iterate through the effect map for until an effect which is not overwritten is found.
	}
	numIterations = 0; //Reset the iteration count.
	return instanceID; //Return the effect if it has no override.
}

void EffectOverrider::SetupOverrides()
{
	vector<uint32_t> props; //Create a vector for the properties.
	if (PropManager.GetPropertyListIDs(id("EffectOverrides"), props)) //and load all effect override prop files into it.
	{
		for each (uint32_t propID in props) //For each effect override file,
		{
			PropertyListPtr propList; //Make a property list pointer
			PropManager.GetPropertyList(propID, id("EffectOverrides"), propList); //and load the file into the list.
			
			//Initialize some pointers
			uint32_t* effectsToOverride; 
			uint32_t* overrideEffects;
			size_t numEffectsToOverride;
			size_t numOverrideEffects;

			//Get the arrays of effects to override and effects to replace them with.
			App::Property::GetArrayUInt32(propList.get(), id("ReplaceEffects"), numEffectsToOverride, effectsToOverride);
			App::Property::GetArrayUInt32(propList.get(), id("EffectsToReplaceWith"), numOverrideEffects, overrideEffects);
			
			if (numEffectsToOverride == numOverrideEffects) //If the number of effects and overrides is the same,
			{
				for (int i = 0; i < numEffectsToOverride; i++) //loop through all effects.
				{
					if (EffectsManager.HasVisualEffect(overrideEffects[i], 0)) //If the effect to override it with exists in the first place,
					{
						effectOverrides.emplace(effectsToOverride[i], overrideEffects[i]); //add both of them to the override map.
					}
					else //otherwise,
					{
						throw std::length_error("Replacement effect does not exist!"); //throw an exception.
					}
				}
			}
			else //If the numbers are different,
			{
				throw std::length_error("Number of replaced and replacement effects do not match."); //Throw an exception.
			}

		}
		
	}
}

hash_map<uint32_t, uint32_t> EffectOverrider::effectOverrides; //Add static variables at end of CPP file.
int EffectOverrider::numIterations;

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

