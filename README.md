# gm-ent-util
A library that gets you those CBaseEntity pointers you deserve.
Bind new entity functions or avoid calling back into lua for simple entity operations.
Requires [garrysmod_common](https://github.com/danielga/garrysmod_common).

# Usage
Add the `include` folder in this repo to your build includes, then `#include <entityutils.h>`.
Call `Utils::InitializeEnityList()` in your module initialization.
After the library has been initialized, you can use `Utils::GetEntity(LUA, stackPos)` to get the `CBaseEntity` pointer for an entity at the given stack position.
See the example folder for a full working implementation.

# Supported Configurations
* Windows server x64
* Windows server x86
* Linux server x86
