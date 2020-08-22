#pragma once

#include <GarrysMod/Lua/Interface.h>
#include <GarrysMod/Lua/LuaBase.h>
#include <GarrysMod/FactoryLoader.hpp>
#include <scanning/symbolfinder.hpp>

#include <cbase.h>
#include <baseentity.h>

#if defined SYSTEM_LINUX
	static const char* GMOD_gEntList_sym_sig = "g_pEntityList";
#elif defined SYSTEM_WINDOWS && defined ARCHITECTURE_X86_64
	static const uint8_t GMOD_gEntList_sym_sig[] = "\x48\x89\x5C\x24\x08\x48\x89\x74\x24\x10\x57\x48\x83\xEC\x40\x48\x8B\x81\x50\x01\x00\x00";
	static const size_t GMOD_gEntList_sym_siglen = sizeof(GMOD_gEntList_sym_sig)-1;
	#define FUNC_TO_LEA 0x23
	#define LEA_TO_FIELD 0x3
	#define LEA_FIELD_SZ 0x4
#elif defined SYSTEM_WINDOWS && defined ARCHITECTURE_X86
	//REF: 'Warning: Spawning AI script conditions'
	static const uint8_t GMOD_gEntList_sym_sig[] = "\x56\x57\x8B\xF1\xBA";
	static const size_t GMOD_gEntList_sym_siglen = sizeof(GMOD_gEntList_sym_sig) - 1;
	#define FUNC_TO_MOV 0x15
	#define MOV_TO_FIELD 0x1
#else
	#error Signatures unavilable for this system!
#endif

CBaseEntityList* g_pEntityList = nullptr;

namespace Utils {
	/*
		Setup pointers to source's global entity list. Returns true on success.
		If it returns false, no library functions should be used.
		It should be called in GMOD_MODULE_OPEN.
	*/

	bool InitializeEnityList() {
		SourceSDK::ModuleLoader server_loader("server");
		SymbolFinder symfinder;

		if(server_loader.GetModule() == nullptr)
			return false;

		#if defined SYSTEM_WINDOWS && defined ARCHITECTURE_X86_64
			uint8* func = (uint8*)symfinder.FindPattern(server_loader.GetModule(), GMOD_gEntList_sym_sig, GMOD_gEntList_sym_siglen, server_loader.GetModule());

			if(func == nullptr)
				return false;

			uint8* lea_base = func + FUNC_TO_LEA;
			uint8* off_base = lea_base + LEA_TO_FIELD + LEA_FIELD_SZ;
			uint32* off_field = (uint32*)(lea_base + LEA_TO_FIELD);

			g_pEntityList = (CBaseEntityList*)(*off_field + off_base);
		#elif defined SYSTEM_WINDOWS && defined ARCHITECTURE_X86
			uint8* func = (uint8*)symfinder.FindPattern(server_loader.GetModule(), GMOD_gEntList_sym_sig, GMOD_gEntList_sym_siglen, server_loader.GetModule());

			if (func == nullptr)
				return false;

			g_pEntityList = *(CBaseEntityList**)(func + FUNC_TO_MOV + MOV_TO_FIELD);
		#elif defined SYSTEM_LINUX
			CBaseEntityList** ptr = (CBaseEntityList**)symfinder.FindSymbol(server_loader.GetModule(), GMOD_gEntList_sym_sig);

			if(ptr == nullptr)
				return false;

			g_pEntityList = *ptr;
		#endif

		return g_pEntityList != nullptr;
	}

	CBaseEntity* GetEntity(GarrysMod::Lua::ILuaBase* LUA, int stackPos = -1) {
		if (!g_pEntityList)
			LUA->ThrowError("EntityList is not initialized!");

		EHANDLE* hdl = LUA->GetUserType<EHANDLE>(stackPos, GarrysMod::Lua::Type::ENTITY);

		if (!hdl)
			LUA->ThrowError("Tried to use a NULL entity! (1)");

		CBaseEntity* pEntity = hdl->Get();

		if (!pEntity)
			LUA->ThrowError("Tried to use a NULL entity! (2)");

		return pEntity;
	}
}