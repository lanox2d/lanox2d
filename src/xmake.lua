option("tests",    {showmenu = true, default = true, description = "Enable tests"})
option("examples", {showmenu = true, default = true, description = "Enable exmaples"})

option("small")
    set_default(true)
    set_showmenu(true)
    set_configvar("LX_CONFIG_SMALL", 1)
    set_description("Enable smallest mode and disable all optional modules")
option_end()

option("wchar")
    add_ctypes("wchar_t")
    set_configvar("LX_CONFIG_TYPE_HAVE_WCHAR", 1)
option_end()

includes("lanox2d")
if has_config("tests") then
    includes("tests")
end
if has_config("examples") then
    includes("examples")
end
