# custom-importer

Hides any API calls from appearing within the Imports section of any image.

## Example
```cpp
  auto get_module_handle = IMPORT( GetModuleHandleA ); //can be assigned to a variable
  IMPORT( GetModuleHandleA )( "kernel32.dll" ); //can be called directly
```
