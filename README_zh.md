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

## 简介

Lanox2D 是一个轻量快速的 2D 矢量图形引擎。

## 备注

此项目还处于内部开发过程中，我们不会提供任何技术支持。

## 构建

### 准备

我们需要先安装 [Xmake](https://github.com/xmake-io/xmake) 跨平台构建工具。

### 构建平台

#### 主机平台

默认情况下，Windows/Linux/macOS 平台都只需要一键编译，Xmake 会选择最优的渲染设备和窗口。

```console
$ xmake
```

#### Android

编译库并且生成 Example 工程的 apk 包。

```console
$ xmake apk_build
```

当然，我们也可以直接用 Android Studio 打开内部的工程来直接编译运行。

#### iOS

编译库并且生成 Example 工程的 ipa 包。

```console
$ xmake ipa_build
```

当然，我们也可以直接用 Xcode 打开内部的工程来直接编译运行。

### 渲染设备

当前，我们支持以下渲染设备：

- opengl
- vulkan
- metal
- bitmap
- skia

#### OpenGL

在 Linux 上，我们默认使用 OpenGL 作为默认的渲染设备，但是在其他平台上，我们如果也想使用 OpenGL，可以手动配置切换。

```console
$ xmake f --window=glfw [--device=opengl]
$ xmake
```

通常，我们只需要设置切换到 glfw 窗口，xmake 就会默认使用 OpenGL。

我们也可以切换使用 Glut 作为窗口。

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

在 macOS/iOS 上，我们会默认使用 Metal 作为渲染设备。

### 渲染窗口

我们可以通过 `xmake f --window=xxx` 配置切换以下窗口类型。

- glfw
- glut
- sdl
- fbdev
- android
- mach

## 联系

* 邮箱：[waruqi@gmail.com](mailto:waruqi@gmail.com)
* 主页：[lanox2d.com](https://lanox2d.com)
* 源码：[Github](https://github.com/lanox2d/lanox2d), [Gitee](https://gitee.com/lanox2d/lanox2d)
* 微信公众号: tboox-os

