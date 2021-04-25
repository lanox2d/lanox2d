for _, filepath in ipairs(os.files(path.join(os.scriptdir(), "*.c"))) do
    local name = path.basename(filepath)
    target(name)
        set_kind("binary")
        add_deps("lanox2d")
        add_files(name .. ".c")
end
