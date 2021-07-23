-- libsdl package
if is_config("window", "sdl") then
    add_requires("libsdl")
end

-- glut package
if is_config("window", "glut") and is_plat("linux", "windows") then
    add_requires("freeglut")
end

-- skia package
if is_config("device", "skia") then
    add_requires("skia")
end

-- libpng package
local bitmap = get_config("bitmap")
if bitmap and bitmap:find("png") then
    add_requires("libpng")
end
