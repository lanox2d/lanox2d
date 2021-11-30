<div align="center">
  <a href="https://lanox2d.com">
    <img width="30%" height="30%" src="https://tboox.org/static/img/lanox2d/lanox2d.png">
  </a>

  <div>
    <a href="https://github.com/lanox2d/lanox2d/blob/main/LICENSE.md">
      <img src="https://img.shields.io/github/license/lanox2d/lanox2d.svg?colorB=f48041&style=flat-square" alt="license" />
    </a>
    <a href="https://t.me/tbooxorg">
      <img src="https://img.shields.io/badge/chat-on%20telegram-blue.svg?style=flat-square" alt="Telegram" />
    </a>
    <a href="https://jq.qq.com/?_wv=1027&k=5hpwWFv">
      <img src="https://img.shields.io/badge/chat-on%20QQ-ff69b4.svg?style=flat-square" alt="QQ" />
    </a>
    <a href="https://github.com/sponsors/waruqi">
      <img src="https://img.shields.io/badge/donate-us-orange.svg?style=flat-square" alt="Donate" />
    </a>
  </div>

  <p>A lightweight and fast 2D vector graphics engine</p>
</div>

## Introduction ([中文](/README_zh.md))

Lanox2D is a lightweight and fast 2D vector graphics engine.

## Note

This project is still in the internal development process, we will not provide any technical support.

## Build

### Prepare

We need to install the [Xmake](https://github.com/xmake-io/xmake) cross-platform build tool first.

### Platform

#### Host Platform

By default, Windows/Linux/macOS platforms only require one-click compilation, and Xmake will select the optimal rendering device and window.

```console
$ xmake
```

#### Android

Compile the library and generate the apk package of the Example project.

```console
$ xmake apk_build
```

Of course, we can also directly use Android Studio to open the internal project to directly compile and run.

#### iOS

Compile the library and generate the ipa package of the Example project.

```console
$ xmake ipa_build
```

Of course, we can also directly use Xcode to open the internal project to directly compile and run.

### Rendering device

Currently, we support the following rendering devices:

- opengl
- vulkan
- metal
- bitmap
- skia

#### OpenGL

On Linux, we use OpenGL as the default rendering device by default, but on other platforms, if we also want to use OpenGL, we can manually configure the switch.

```console
$ xmake f --window=glfw [--device=opengl]
$ xmake
```

Usually, we only need to switch to the glfw window, and xmake will use OpenGL by default.

We can also switch to use Glut as a window.

```console
$ xmake f --window=glut
$ xmake
```

#### Vulkan

```console
$ xmake f --device=vulkan --window=glfw
$ xmake
```

#### Metal

On macOS/iOS, we will use Metal as the rendering device by default.

### Render window

We can switch the following window types through the `xmake f --window=xxx` configuration.

- glfw
- glut
- sdl
- fbdev
- android
- mach

## Contacts

* Email：[waruqi@gmail.com](mailto:waruqi@gmail.com)
* Homepage：[lanox2d.com](https://lanox2d.com)
* Source Code：[Github](https://github.com/lanox2d/lanox2d), [Gitee](https://gitee.com/lanox2d/lanox2d)
* Wechat Public: tboox-os
