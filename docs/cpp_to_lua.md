# Exposing C++ functions to Lua Scripts
C++ functions can be exposed to Lua code using wrapper functions and 
the interface defined in scripting.h.

## Lua Wrappers
To be available in Lua, a function will need a wrapper with the signature of lua_CFunction.
Arguments passed from Lua can be accessed from lua_State using the lua_is... and lua_to...
library functions. For more information, refer to the
[Lua Reference Manual](http://www.lua.org/manual/5.4/manual.html#lua_CFunction)

## Registering Lua Wrappers
The scripting interface allows for two types of function registration:
modular and global. Modular registration minimizes potential naming conflicts
and improves interface organization. It is therefore preferred.

Modules are defined using scripting::beginModule() and scripting::finalizeModule().
Fuctions can be added to an active module by calling scripting::registerModuleFunction() between the
above functions. Registered functions can then be accessed in Lua using <module-name>.<function-name>.

Global functions can be registered using scripting::registerGlobalFunction(). They can be accessed in
Lua by simply using the function name given.
