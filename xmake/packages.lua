-- libsdl package
if is_config("window", "sdl") then
    add_requires("libsdl")
end

-- glut package
if is_config("window", "glut") and is_plat("linux", "windows") then
    add_requires("freeglut")
end
