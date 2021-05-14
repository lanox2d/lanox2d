for _, filepath in ipairs(os.files(path.join(os.scriptdir(), "*.c"))) do
    local name = path.basename(filepath)
    target("example_" .. name)
        if not has_config("examples") then
            set_default(false)
        end
        set_kind("binary")
        add_deps("lanox2d")
        add_files(name .. ".c")
end
