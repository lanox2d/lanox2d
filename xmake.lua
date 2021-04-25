set_project("lanox2d")
set_xmakever("2.5.3")
set_version("1.0.1", {build = "%Y%m%d%H%M"})
set_warnings("all", "error")
add_rules("mode.release", "mode.debug", "mode.asan", "mode.minsizerel", "mode.releasedbg")
if is_plat("debug") then
    add_defines("LX_DEBUG")
end
includes("src")
