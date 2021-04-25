target("lanox2d")

    -- set basic configuration
    set_kind("$(kind)")
    add_defines("__lx_prefix__=\"lanox2d\"")

    -- set the auto-generated config.h
    set_configdir("$(buildir)/$(plat)/$(arch)/$(mode)")
    add_configfiles("lanox2d.config.h.in")

    -- add include directories
    add_includedirs("..", {public = true})
    add_includedirs("$(buildir)/$(plat)/$(arch)/$(mode)", {public = true})

    -- add the header files for installing
    add_headerfiles("../(lanox2d/**.h)|**/impl/**.h")
    add_headerfiles("$(buildir)/$(plat)/$(arch)/$(mode)/lanox2d.config.h", {prefixdir = "lanox2d"})

    -- add source files
    add_files("*.c")

    -- add options
    add_options("small", "wchar")
