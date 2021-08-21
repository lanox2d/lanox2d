target("lanox2d")
    add_mflags("-Wno-error=deprecated-declarations")
    add_headerfiles("*.h")
    add_includedirs(".", {public = true})
    if is_plat("macosx") then
        add_frameworks("AppKit")
    else
        add_frameworks("UIKit")
    end
    if is_plat("iphoneos") and is_config("device", "opengl") then
        add_files("opengl/Lanox2dGLView.m")
        add_frameworks("OpenGLES", "QuartzCore")
    elseif is_config("device", "metal") then
        add_files("metal/Lanox2dMetalView.m")
        add_frameworks("MetalKit", "Metal")
    end

