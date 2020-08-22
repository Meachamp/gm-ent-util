#include <GarrysMod/Lua/Interface.h>
#include <entityutils.h>

#include <cbase.h>
#include <baseentity.h>

/*
	Test lua function
*/

LUA_FUNCTION_STATIC(test) {
	/*Get the entity from Lua
	  You either get a valid pointer or the function throws a lua error.
	  Default behaviour is to pull off the top of the stack, but an optional second
	  arg can control the stack position we pull from. 
	*/
	CBaseEntity* ent = Utils::GetEntity(LUA);

	/*
	We can now use any function on the CBaseEntity class. We'll just push the entity userdata
	right back to lua. No messing around Entity calls into lua anymore, we can just do it the 
	same way gmod does internally. 
	*/
	ent->PushEntity();
	return 1;
}

GMOD_MODULE_OPEN()
{
	//Initialize the library and ensure it initialized properly. 
	bool success = Utils::InitializeEnityList();
	if (!success)
		LUA->ThrowError("Could not initialize entity utils!");

	//Here we push a test cfunction, just the global "test"
	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
		LUA->PushString("test");
		LUA->PushCFunction(test);
		LUA->SetTable(-3);
	LUA->Pop();

	return 0;
}

GMOD_MODULE_CLOSE()
{
	return 0;
}