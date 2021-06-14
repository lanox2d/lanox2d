target("lanox2d")

    -- set basic configuration
    set_kind("$(kind)")

    -- export all symbols for windows/dll
    if is_plat("windows") and is_kind("shared") then
        add_rules("utils.symbols.export_all")
    end

    -- set the auto-generated config.h
    set_configdir("$(buildir)/$(plat)/$(arch)/$(mode)")
    add_configfiles("lanox2d.config.h.in")

    -- add include directories
    add_includedirs("..", {public = true})
    add_includedirs("$(buildir)/$(plat)/$(arch)/$(mode)", {public = true})

    -- add the header files for installing
    add_headerfiles("../(lanox2d/*.h)")
    add_headerfiles("../(lanox2d/base/**.h)|libc/arch/**.h")
    add_headerfiles("../(lanox2d/prefix/**.h)")
    add_headerfiles("../(lanox2d/core/**.h)|private/**.h|pixmap/*.h|device/**.h")
    add_headerfiles("../(lanox2d/platform/**.h)|windows/*.h")
    add_headerfiles("$(buildir)/$(plat)/$(arch)/$(mode)/lanox2d.config.h", {prefixdir = "lanox2d"})

    -- add source files
    add_files("*.c")
    add_files("base/**.c|libc/arch/**.c|platform/**.c")
    add_files("base/platform/*.c")
    add_files("core/**.c|pixmap/*.c|device/**.c")
    add_files("platform/**.c|windows/*.c")

    -- add options
    add_options("small", "wchar", "window")

    -- check interfaces
    check_interfaces()

    -- add sdl package
    if is_config("window", "sdl") then
        add_packages("libsdl")
        set_configvar("LX_CONFIG_WINDOW_HAVE_SDL", 1)
    end

    -- add glut package
    if is_config("window", "glut") then
        if is_plat("macosx") then
            add_frameworks("GLUT")
            add_cxflags("-Wno-error=deprecated-declarations")
        else
            add_packages("freeglut")
        end
        set_configvar("LX_CONFIG_WINDOW_HAVE_GLUT", 1)
    end

    -- add bitmap device
    if is_config("window", "sdl") then
        set_configvar("LX_CONFIG_DEVICE_HAVE_BITMAP", 1)
        add_files("core/device/bitmap/**.c")
    end

    -- add opengl device
    if is_config("window", "glut") then
        set_configvar("LX_CONFIG_DEVICE_HAVE_OPENGL", 1)
        add_files("core/device/opengl/**.c")
    end
