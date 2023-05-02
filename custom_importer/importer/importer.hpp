/*
   C++17 Lightweight import resolver for Windows
*/

#ifndef _IMPORTER_HPP_
#define _IMPORTER_HPP_
#include <Windows.h>
#include <winternl.h>

#define IMPORT(imp) reinterpret_cast<decltype(&imp)>(importer::helper::lookup_entry<#imp>())

namespace importer
{
    namespace helper
    {
        namespace hash
        {
            inline constexpr auto offset = 0xcbf29ce484222325ull;
            inline constexpr auto prime = 1099511628211ull;

            constexpr __forceinline std::uint64_t hash_str( std::string_view str )
            {
                const std::size_t sz = str.length( );

                auto hash = offset;
                for ( std::size_t i = 0; i < sz; ++i )
                {
                    const std::uint8_t c = static_cast< std::uint8_t >( str[ i ] );

                    hash ^= ( c >= 'A' && c <= 'Z' ) ? ( c | ( 1 << 5 ) ) : c;
                    hash *= prime;
                }

                return hash;
            }
        }

        template <auto v>
        struct wrapper_const
        {
            static constexpr decltype( v ) value = v;
        };

        template <typename t, typename s, typename v>
        __forceinline constexpr v copy_range( t dst, s size, v result )
        {
            if ( !dst || !size )
                return result;

            *result = *dst;
            ++result;

            for ( s i = 1; i != size; ++i )
            {
                *result = *( ++dst );
                ++result;
            }

            return result;
        }

        template <size_t sz>
        struct str_const
        {
            constexpr str_const( const char( &str )[ sz ] )
            {
                copy_range( str, sz, value );
            }

            char value[ sz ];
        };

        template <auto v>
        constexpr decltype( v ) wrapped_constant = wrapper_const<v>::value;

        template <str_const function_name>
        __forceinline std::uintptr_t* lookup_entry( )
        {
            PPEB peb { };
#ifdef _WIN64
            peb = reinterpret_cast< PPEB >( __readgsqword( 0x60 ) );
#else
            peb = reinterpret_cast< PPEB >( __readfsdword( 0x30 ) );
#endif

            if ( !peb )
                return nullptr;

            const auto function_hash = wrapped_constant<hash::hash_str( function_name.value )>; // hash function name
            
            for ( auto& current = peb->Ldr->InMemoryOrderModuleList.Flink; current != &peb->Ldr->InMemoryOrderModuleList; current = current->Flink )
            {
                const auto entry = CONTAINING_RECORD( current, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks );

                if ( !entry )
                    continue;

                if ( !entry->FullDllName.Buffer )
                    continue;

                const auto base = reinterpret_cast< std::uintptr_t >( entry->DllBase );

                const auto dos_header = reinterpret_cast< IMAGE_DOS_HEADER* >( base );
                if ( !dos_header || dos_header->e_magic != IMAGE_DOS_SIGNATURE )
                    continue;

                const auto nt_headers = reinterpret_cast< PIMAGE_NT_HEADERS >( base + dos_header->e_lfanew );
                if ( !nt_headers || nt_headers->Signature != IMAGE_NT_SIGNATURE )
                    continue;

                const auto export_directory = reinterpret_cast< PIMAGE_EXPORT_DIRECTORY >( base + nt_headers->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_EXPORT ].VirtualAddress );
                if ( !export_directory->AddressOfFunctions )
                    continue;

                const auto export_address_table = reinterpret_cast< std::uint32_t* >( base + export_directory->AddressOfFunctions );
                const auto export_name_table = reinterpret_cast< std::uint32_t* >( base + export_directory->AddressOfNames );
                const auto export_ordinal_table = reinterpret_cast< std::uint16_t* >( base + export_directory->AddressOfNameOrdinals );

                if ( !export_directory || !export_address_table || !export_name_table || !export_ordinal_table )
                    continue;

                for ( auto i = 0u; i < export_directory->NumberOfNames; ++i )
                {
                    const auto export_name = ( reinterpret_cast< const char* >( base + export_name_table[ i ] ) );
                    const auto export_hash = hash::hash_str( export_name );

                    if ( function_hash == export_hash )
                        return reinterpret_cast< std::uintptr_t* >( base + export_address_table[ export_ordinal_table[ i ] ] );
                }
            }

            return nullptr;
        }
    }
}
#endif