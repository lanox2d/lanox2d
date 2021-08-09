task("apk_build")
    set_menu {usage = "xmake apk_build [options]", description = "build android apk.", options = {
        {'r', "release", "k", nil, "Build the release package."}
    }}
    on_run(function ()
        import("core.base.option")
        os.cd(os.scriptdir())
        if option.get("release") then
            os.exec("./gradlew app:assembleRelease")
            print("resigning apk ..")
            local apkfile_signed = "./app/build/outputs/apk/release/app-release-signed.apk"
            local apkfile_unsigned = "./app/build/outputs/apk/release/app-release-unsigned.apk"
            local java_home = assert(os.getenv("JAVA_HOME"), "$JAVA_HOME not found!")
            local jarsigner = path.join(java_home, "bin", "jarsigner" .. (is_host("windows") and ".exe" or ""))
            assert(os.isfile(jarsigner), "%s not found!", jarsigner)
            local alias = "test"
            local storepass = "1234567890"
            local argv = {"-keystore", path.join(os.scriptdir(), "res", "sign.keystore"), "-signedjar", apkfile_signed, "-digestalg", "SHA1", "-sigalg", "MD5withRSA", apkfile_unsigned, alias, "--storepass", storepass}
            os.runv(jarsigner, argv)
            print("resigning apk ok!")
        else
            os.exec("./gradlew app:assembleDebug")
        end
    end)

task("apk_install")
    set_menu {usage = "xmake apk_install [options]", description = "install android apk.", options = {
        {'r', "release", "k", nil, "Install the release package."}
    }}
    on_run(function ()
        import("core.base.option")
        os.cd(os.scriptdir())
        if option.get("release") then
            os.exec("adb install -r ./app/build/outputs/apk/release/app-release-signed.apk")
        else
            os.exec("adb install -r ./app/build/outputs/apk/debug/app-debug.apk")
        end
    end)

task("apk_test")
    set_menu {usage = "xmake apk_test [options]", description = "install and test android apk.", options = {
        {'r', "release", "k", nil, "Build and test the release package."}
    }}
    on_run(function ()
        import("core.base.task")
        import("core.base.option")
        task.run("apk_build", {release = option.get("release")})
        task.run("apk_install", {release = option.get("release")})
        os.exec("adb logcat -s lanox2d")
    end)

task("aar_upload")
    set_menu {usage = "xmake aar_upload [options]", description = "upload the aar archives.", options = {}}
    on_run(function ()
        os.cd(os.scriptdir())
        os.exec("./gradlew lib:uploadArchives")
    end)

includes("lib/jni", "app/jni")
