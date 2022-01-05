-- libsdl package
if is_config("window", "sdl") then
    add_requires("libsdl 2.x")
end

-- glut package
if is_config("window", "glut") then
    add_requires("glut")
end

-- glfw package
if is_config("window", "glfw") then
    add_requires("glfw")
end

-- vulkan package
-- we should use libvulkan instead of MoltenVK on macOS for glfw, @see https://github.com/achirkin/vulkan/issues/24
if is_config("device", "vulkan") then
    if is_plat("windows", "linux", "macosx") then
        add_requires("vulkan-loader")
        add_requires("glslang", {configs = {binaryonly = true}})
    elseif is_plat("android") then
        add_requires("vulkan-validationlayers")
    end
    add_requires("vulkan-memory-allocator")
end

-- skia package
if is_config("device", "skia") then
    add_requires("skia")
end

-- libpng package
local bitmap = get_config("bitmap")
if bitmap and bitmap:find("png") and not is_plat("android", "macosx", "iphoneos") then
    add_requires("libpng")
end

-- libjpeg-turbo package
local bitmap = get_config("bitmap")
if bitmap and bitmap:find("jpg") and not is_plat("macosx", "iphoneos") then
    if is_plat("android") then
        add_requires("libjpeg", {configs = {headeronly = true}})
    else
        add_requires("libjpeg-turbo", {alias = "libjpeg"})
    end
end
