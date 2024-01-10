#include "stdafx.h"
#include "EffectOverrider.h"
#include <stdexcept>

EffectOverrider::EffectOverrider()
{
	try //Try the following.
	{
		SetupOverrides(); //Setup the effects.
	}
	catch (std::exception except)
		//If an exception has been found, then we
	{
		const char* text = except.what(); //Get the text from the exception.
		wstring report; //then, make an empty WString as part of casting the initial text - this can't be done in one step
		report.assign_convert(text); //So, we assign the text to the report, converting it to the right format.
		MessageBox(NULL, report.c_str(), LPCWSTR(u"Error initializing effect replacer"), 0x00000010L); //Lastly, use this string in a message box to inform the user of an error.
		//And, finally, we just exit the program by calling exit().
		exit(EXIT_FAILURE);
	}
}

EffectOverrider::~EffectOverrider()
{
}


uint32_t EffectOverrider::GetOverrideEffect(uint32_t instanceID)
{
	auto effectFind = effectOverrides.find(instanceID); //First, find the effect to override it with.
	//This is done using the hash-map's find() function.
	//A hash-map is more efficient with a large data set than a regular map, so it is used here - it has an O(1) time complexity in most cases.
	
	if (effectFind != effectOverrides.end()) //If the effect is in the override list, then we will override it.
		//This is required as an exit-condition for the recursive nature of this function. 
		//Otherwise, it would cause a stack overflow and quickly crash Spore without informing the developer.
	{
		uint32_t newEffect = effectFind->second; //Get the ID of the new effect.

		numIterations++; //Increase the number of iterations, used to prevent endless loops.
		
		if (numIterations > 1000000) //If an endless loop is detected, then something very wrong has happened.
		{
			//It likely is caused by an effect overriding another, which overrides the first effect - so, we use a message box to tell the mod developer that something is up.
			//The message box is from the Win32 API, so there's no need to ship an extra DLL or anything for it.
			MessageBox(NULL, LPCWSTR(u"Endless loop achieved; make sure your effect isn't overriden by what it's ovewriting!"), LPCWSTR(u"Error overriding effect"), 0x00000010L);
			throw std::exception("Endless loop achieved; make sure your effect isn't overriden by what it's ovewriting!"); //And then, throw an exception so that Spore crashes.
		}
		//If no loop was achieved yet, that's good.
		//So, we recursively iterate through the effect map for until an effect which is not overwritten is found.
		return GetOverrideEffect(newEffect); 
	}
	numIterations = 0; //Reset the iteration count.
	return instanceID; //Return the effect if it has no override.
}

void EffectOverrider::SetupOverrides()
{
	vector<uint32_t> props; //Create a vector for the properties.
	if (PropManager.GetPropertyListIDs(id("EffectOverrides"), props)) //and load all effect override prop files into it.
		//If it's successful, then it returns true and this statement is continued.
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
			//The App::Property::GetArrayUInt32 method inputs a property ID, and outputs a boolean (as the return value) and two return arguments.
			//The return arguments are what we're interested in - they're the output array and the number of elements in the array.
			App::Property::GetArrayUInt32(propList.get(), id("ReplaceEffects"), numEffectsToOverride, effectsToOverride);
			App::Property::GetArrayUInt32(propList.get(), id("EffectsToReplaceWith"), numOverrideEffects, overrideEffects);
			
			//Using these two return arguments...
			if (numEffectsToOverride == numOverrideEffects) //If the number of effects and overrides is the same,
			{
				for (int i = 0; i < numEffectsToOverride; i++) //loop through all effects.
					//This is done with a basic FOR loop, since a foreach loop is not supported by arrays in C++.
				{
					if (EffectsManager.HasVisualEffect(overrideEffects[i], 0)) //If the effect to override it with exists in the first place,
					{
						effectOverrides.emplace(effectsToOverride[i], overrideEffects[i]); //add both of them to the override map.
					}
					else //otherwise, an invalid effect has been specified, and we must notify the user.
					{
						//So, an error is thrown for the sake of this. It'll be caught by the try... catch statement in the constructor, where this function is called.
						throw std::invalid_argument("Replacement effect does not exist!"); //So, the exception is thrown.
					}
				}
			}
			else //If the numbers are different, then another error must've occurred.
			{
				//The mod author would've accidentally messed up the size of the arrays, and this too must be notified.
				throw std::length_error("Number of replaced and replacement effects do not match."); //So, we throw an exception.
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

