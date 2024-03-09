//-----------------------------------------------------------------------------
// Prepare
//-----------------------------------------------------------------------------
#include <GarrysMod/Lua/Interface.h>
#include <Platform.hpp>

#include <iostream>
#include <string>
#include <cstdio>

#pragma region

	#ifdef SYSTEM_WINDOWS
		const bool is_windows = true;
	#else
		const bool is_windows = false;
	#endif

	#ifdef SYSTEM_LINUX
		const bool is_linux = true;
	#else
		const bool is_linux = false;
	#endif

	#ifdef SYSTEM_MACOSX
		const bool is_osx = true;
	#else
		const bool is_osx = false;
	#endif

	#ifdef ARCHITECTURE_X86_64
		const bool is_x64 = true;
	#else
		const bool is_x64 = false;
	#endif

	const char* dll_prefix = "gmsv";

	bool static IsClient( GarrysMod::Lua::ILuaBase* LUA ) {

		LUA->GetField( GarrysMod::Lua::INDEX_GLOBAL, "CLIENT" );
		const bool CLIENT = LUA->GetBool( -1 ); LUA->Pop();

		return CLIENT;

	}

	void static SetDLLPrefixToClient() {

		dll_prefix = "gmcl";

	}

	const char* dll_suffix = (

		/*if*/ is_windows ?
			/*return*/ ( is_x64 ? "win64" : "win32" )
		/*else*/ : (

			/*if*/ is_linux ?
				/*return*/ ( is_x64 ? "linux64" : "linux" )
			/*else*/ : (

				/*if*/ is_osx ?
					/*return*/ ( is_x64 ? "osx64" : "osx" )
				/*else*/ :
					/*return*/ ""

			)

		)

	);

	const char* separator = is_windows ? "\\" : "/";

	char PathFormat[64] = "";

	#define Format sprintf

	#define isstring( LUA, stackpos ) LUA->IsType( stackpos, GarrysMod::Lua::Type::String )
	#define isuserdata( LUA, stackpos ) LUA->IsType( stackpos, GarrysMod::Lua::Type::UserData )

	#define IsValidPath( loadlib, relpath ) ( ( LOADLIB.find( "LOADLIB: " ) != std::string::npos ) && ( LOADLIB.find( RelativePath ) != std::string::npos ) )

#pragma endregion

//-----------------------------------------------------------------------------
// unrequire
//-----------------------------------------------------------------------------
LUA_FUNCTION_STATIC( unrequire ) {

	const char* name = LUA->CheckString( 1 ); LUA->Pop();

	//
	// Purge from _MODULES
	//
	LUA->GetField( GarrysMod::Lua::INDEX_GLOBAL, "_MODULES" );
		LUA->PushNil();
		LUA->SetField( -2, name );

	LUA->Pop( 1 );

	//
	// Purge from package.loaded
	//
	LUA->GetField( GarrysMod::Lua::INDEX_GLOBAL, "package" );
		LUA->GetField( -1, "loaded" );
			LUA->PushNil();
			LUA->SetField( -2, name );

	LUA->Pop( 2 );

	//
	// Format
	//
	char RelativePath[64] = "";
	Format( RelativePath, PathFormat, name );

	//
	// Iterate over registry
	//
	LUA->PushSpecial( GarrysMod::Lua::SPECIAL_REG );
	LUA->PushNil();

	std::string LOADLIB;

	while ( LUA->Next( -2 ) != 0 ) {

		if ( !( isstring( LUA, -2 ) && isuserdata( LUA, -1 ) ) ) {
			goto next;
		}

		LOADLIB = LUA->GetString( -2 );

		if ( !IsValidPath( LOADLIB, RelativePath ) ) {
			goto next;
		}

		//
		// Close module
		//
		LUA->GetMetaTable( -1 );
			LUA->GetField( -1, "__gc" );
				LUA->Push( -3 );
				LUA->Call( 1, 0 );

		LUA->Pop( 3 );

		//
		// Purge from registry
		//
		LUA->PushNil();
		LUA->SetField( 1, LOADLIB.c_str() );

		break;

	next:
		LUA->Pop();

	}

	LUA->Pop();

	return 0;

}

//-----------------------------------------------------------------------------
// Initialize
//-----------------------------------------------------------------------------
GMOD_MODULE_OPEN() {

	if ( dll_suffix == "" ) {
		LUA->ThrowError( "unknown platform?" );
	}

	if ( IsClient( LUA ) ) {
		SetDLLPrefixToClient();
	}

	Format( PathFormat, "%sgarrysmod%slua%sbin%s%s_%%s_%s.dll", separator, separator, separator, separator, dll_prefix, dll_suffix );

	LUA->PushCFunction( unrequire );
	LUA->SetField( GarrysMod::Lua::INDEX_GLOBAL, "unrequire" );

	return 0;

}

//-----------------------------------------------------------------------------
// Deinitialize
//-----------------------------------------------------------------------------
GMOD_MODULE_CLOSE() {

	return 0;

}
