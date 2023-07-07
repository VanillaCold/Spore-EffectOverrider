# Spore-EffectOverrider
A Spore mod that makes it possible easy to override effects!    
It requires the Spore ModAPI Launcher Kit, both to install and to use.  

# Usage (For end-users)  
If you're just looking to use a mod that requires this, the only required step is to install this mod.  
To install the mod, use the Spore ModAPI Launcher Kit's bundled mod installer (the Spore ModAPI Easy Installer).  
You can get the Spore ModAPI Launcher Kit from here:
http://davoonline.com/sporemodder/rob55rod/ModAPI/Public/

# Developer Info
To make a mod that overrides effects using this system, your mod's Sporemodder FX project must include the EffectOverrides folder.  
In this folder, it needs a property list containing two unsigned integer arrays; a list of effect IDs to override, and the effect IDs to replace them with.  

For further information, look at the sample project included in the mod's source code:    
https://github.com/VanillaCold/Spore-EffectOverrider/tree/main/EffectSampleSMFXProject
