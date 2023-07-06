#pragma once

#include <Spore\BasicIncludes.h>

#define EffectOverriderPtr intrusive_ptr<EffectOverrider>

class EffectOverrider 
	: public Object
	, public DefaultRefCounted
{
public:
	static const uint32_t TYPE = id("EffectOverrider");
	
	EffectOverrider();
	~EffectOverrider();

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;
	static uint32_t GetOverrideEffect(uint32_t instanceID);

private:
	void SetupOverrides();
	static map<uint32_t, uint32_t> effectOverrides;
};
