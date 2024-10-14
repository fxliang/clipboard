target("lua54")
  set_kind("static")
  add_files("lua/*.c")
  remove_files("lua/lua.c", "lua/onelua.c")

target("clip")
  set_kind("shared")
  add_files("./src/clip.cpp")
  add_includedirs("./lua")
  add_deps("lua54")
  add_links('user32')
  set_filename("clip.dll")
  if is_plat("mingw") then
    add_shflags("-static-libgcc", "-static-libstdc++")
  end
  after_build(function(target)
    os.cp(path.join(target:targetdir(), "clip.dll"), "$(projectdir)")
  end)
