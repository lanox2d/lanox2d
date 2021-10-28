task("apk_build")
    set_menu {usage = "xmake apk_build [options]", description = "build android apk.", options = {
        {'r', "release", "k", nil, "Build the release package."}
    }}
    on_run(function ()
        import("core.base.option")
        import("detect.sdks.find_android_sdk")
        os.cd(os.scriptdir())
        if option.get("release") then
            os.exec("./gradlew app:assembleRelease")
            print("resigning apk ..")
            local apkfile_signed = "./app/build/outputs/apk/release/app-release-signed.apk"
            local apkfile_unsigned = "./app/build/outputs/apk/release/app-release-unsigned.apk"
            local alias = "test"
            local storepass = "1234567890"
            local keystore = path.join(os.scriptdir(), "res", "sign.keystore")
            local android_sdk = assert(find_android_sdk(), "Android SDK not found!")
            local apksigner = path.join(android_sdk.sdkdir, "build-tools", android_sdk.build_toolver, "apksigner")
            assert(os.isfile(apksigner), "apksigner not found!")
            local argv = {"sign", "--ks", keystore, "--ks-key-alias", alias, "--ks-pass", "pass:" .. storepass, "--out", apkfile_signed, apkfile_unsigned}
            os.runv(apksigner, argv)
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
task_end()

if is_plat("android") then
    includes("lib/jni", "app/jni")
end
