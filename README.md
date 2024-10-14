# clipboard: lua module for clipboard on Windows

# how to build

## msvc

```bat
git clone -v --recursive https://github.com/fxliang/clipboard.git
cd clipboard
xmake f -p windows
xmake 
```

## mingw

```bat
git clone -v --recursive https://github.com/fxliang/clipboard.git
cd clipboard
xmake f -p mingw
xmake 
```

# API in lua

```lua
-- make sure clip.dll in your package.cpath
local clip = require("clip")
-- get current clipboard data (txt), it might be nil or empty
local txt = clip.get()
print(txt)
-- clip.set("data")
```

# how to use with librime

- copy `clip.dll` and `clipboard.lua` to `<path-to-user-data-dir>\lua\`
- patch your schema, add  `- lua_translator@*clipboard` to `engine/translators`.
- patch your schema, add `recognizer/patterns` to recognize the predefined pattern `/zt` if you don't have a compatible one.
- deploy and enjoy.


# Credits
[lua](https://github.com/lua/lua.git) (MIT License)