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
    add_headerfiles("../(lanox2d/base/**.h)")
    add_headerfiles("../(lanox2d/prefix/**.h)")
    add_headerfiles("../(lanox2d/core/**.h)|pixmap/*.h|devices/**.h")
    add_headerfiles("../(lanox2d/platform/**.h)|windows/*.h")
    add_headerfiles("$(buildir)/$(plat)/$(arch)/$(mode)/lanox2d.config.h", {prefixdir = "lanox2d"})

    -- add source files
    add_files("*.c")
    add_files("base/**.c")
    add_files("core/**.c|pixmap/*.c|devices/**.c")
    add_files("platform/**.c|windows/*.c")

    -- add options
    add_options("small", "wchar", "window")

    -- add sdl package
    if has_package("libsdl") then
        add_packages("libsdl")
        set_configvar("LX_CONFIG_WINDOW_HAVE_SDL", 1)
    end

    -- add bitmap device
    if has_package("libsdl") then
        set_configvar("LX_CONFIG_DEVICE_HAVE_BITMAP", 1)
        add_files("core/device/bitmap/**.c")
    end

