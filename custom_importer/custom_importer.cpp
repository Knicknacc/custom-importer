#include <iostream>
#include "importer/importer.hpp"

int main( )
{
	// Import the functions.
	static auto load_library_a = IMPORT( LoadLibraryW );
	static auto get_module_handle_a = IMPORT( GetModuleHandleA );
	static auto get_proc_address = IMPORT( GetProcAddress );
	static auto get_current_process = IMPORT( GetCurrentProcess );
	static auto get_current_thread = IMPORT( GetCurrentThread );
	static auto get_process_id = IMPORT( GetProcessId );
	static auto get_thread_id = IMPORT( GetThreadId );
	static auto get_system_info = IMPORT( GetSystemInfo );
	static auto get_system_time_as_file_time = IMPORT( GetSystemTimeAsFileTime );
	static auto get_system_time = IMPORT( GetSystemTime );
	static auto get_system_time_adjustment = IMPORT( GetSystemTimeAdjustment );
	static auto get_system_time_precise_as_file_time = IMPORT( GetSystemTimePreciseAsFileTime );
	static auto get_system_times = IMPORT( GetSystemTimes );
	static auto get_system_time_adjustment_precise = IMPORT( GetSystemTimeAdjustmentPrecise );
	static auto get_async_key_state_fn = IMPORT( GetAsyncKeyState );

	// Print out the addresses of the imported functions.
	printf( "LoadLibrary: 0x%p\n", load_library_a );
	printf( "GetModuleHandleA: 0x%p\n", get_module_handle_a );
	printf( "GetProcAddress: 0x%p\n", get_proc_address );
	printf( "GetCurrentProcess: 0x%p\n", get_current_process );

	return std::cin.get( );
}