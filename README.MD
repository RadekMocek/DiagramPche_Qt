# DiagramPche :: Qt

## TODO

* [ ] Error highlight
* [ ] Resizable textedit
* [x] SVG export ability
* [ ] SVG export dialog
  * [x] _Browse..._ button
  * [ ] _Overwrite guard_ – label, auto checkbox/export_btn enable/disable
  * [x] _Action after export_

## Problems

* Paths "aliasing"

### Differences to IMGUI

* Canvas: no endless scrolling, automatic boundaries and scrollbars

## Dependencies

* [Qt](https://www.qt.io/development/qt-framework) ([LGPL](https://doc.qt.io/qt-6/lgpl.html))
* [toml++](https://github.com/marzer/tomlplusplus) ([MIT](https://github.com/marzer/tomlplusplus/blob/master/LICENSE))
* [Inconsolata](https://levien.com/type/myfonts/inconsolata.html) ([SIL Open Font](https://github.com/google/fonts/blob/main/ofl/inconsolata/OFL.txt))

## Building locally

### Debian based distros

* Download Qt from <https://www.qt.io/> and install it, then:

```bash
sudo apt install cmake ninja-build libxcb-cursor0 libxcb-cursor-dev

export QTDIR=<path_to_qt_installation>/6.10.0/gcc_64/  # Add this line to ~/.bashrc for auto export

# Navigate to the root directory of this repository

cmake -S . -B build

cmake --build build
```

* To make it work in CLion on Linux:
  1. Open the project in CLion
  2. Click on the CMake icon at the bottom left corner
  3. Click on the cogwheel icon → _CMake settings_
  4. In the _Cache variables_ section of the settings, change _Value_ of _Name_ `Qt6_DIR` to `<path_to_qt_installation>/6.10.0/gcc_64/lib/cmake/Qt6`

### Windows with Visual Studio

* Download Qt from <https://www.qt.io/> and install it, the only component needed is _Qt_ → _Qt 6.10.0_ → _MSVC 2022 64-bit_
* Create a new system environment variable called `QTDIR` with value: `<path_to_qt_installation>\6.10.0\msvc2022_64`
* Add two new values to system environment variable PATH:
  * `%QTDIR%\bin`
  * `%QTDIR%\dir`
* In Visual Studio's launcher, click on the _Open a local folder_ button and select root directory of this repository

### Windows with CLion

To run working Visual Studio project from the previous section in CLion, a _Visual Studio toolchain_ must be set up. See [Tutorial: Configure CLion on Windows](https://www.jetbrains.com/help/clion/quick-tutorial-on-configuring-clion-on-windows.html).

An alternative approach would be to choose the _MinGW_ component instead of _MSVC_ during the Qt installation, set environment variables accordingly, and then run the project directly in CLion. I haven't tested that, though.
