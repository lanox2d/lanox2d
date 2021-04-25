option("tests",    {showmenu = true, default = true, description = "Enable tests"})
option("examples", {showmenu = true, default = true, description = "Enable exmaples"})
option("small",    {showmenu = true, default = true, configvar = {"LX_CONFIG_SMALL", 1},
                    description = "Enable smallest mode and disable all optional modules"})
option("wchar",    {ctypes = "wchar_t", configvar = {"LX_CONFIG_TYPE_HAVE_WCHAR", 1}})

includes("lanox2d")
if has_config("tests") then
    includes("tests")
end
if has_config("examples") then
    includes("examples")
end
