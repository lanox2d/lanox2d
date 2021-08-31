target("Example")
    if has_config("examples") and is_config("window", "mach") then
        set_default(true)
    else
        set_default(false)
    end
    add_deps("lanox2d")
    add_rules("xcode.application")
    add_files("*.m", "*.xcassets")
    add_defines("LX_NOMAIN_ENTRY")
    add_installfiles("$(projectdir)/res/*.png")
    if is_plat("macosx") then
        add_files("macOS/**.storyboard", "macOS/Info.plist")
    elseif is_plat("iphoneos") then
        add_files("iOS/*.m", "iOS/**.storyboard", "iOS/Info.plist")
        add_includedirs("iOS")
    end
