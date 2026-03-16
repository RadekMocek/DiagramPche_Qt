# DiagramPche :: Qt

This is part of a IMGUI paradigm research, [go back to main repo](https://github.com/RadekMocek/DP).

* Jump to:
  * [Building locally](#building-locally)
  * [Dependencies](#dependencies)
  * [Problems](#problems)

## TODO

* General:
  * [ ] View → Jump to canvas origin
  * [ ] View → Toolbar
    * [ ] Source: Allow changing font size of the text editor
    * [ ] Source: Show cursor position in the text editor
    * [ ] Canvas: Change color node with color picker (add the parameter if it's missing in source; be aware that color def can be multiple lines if array)
    * [ ] Canvas: Change node's type via Drop-down list (add the parameter if it's missing in source)
  * [ ] View → Secondary canvas toolbar
    * [ ] Drag'n'Drop new node, button for each node type (beware the canvas scroll and zoom)
    * [ ] Zoom slider, right aligned
  * Saving/loading:
    * [ ] New, Open, Save, Save as
    * [ ] Confirm unsaved exit (OS close button, alt+f4, mainmenubar option)
    * [ ] Confirm unsaved new
    * [ ] Confirm unsaved open, including loading a debug diagram or an example
    * [ ] Change window name according to opened document (incl. star for unsaved)
    * [ ] _Save → Cancel_ should behave the same as unsavedwarn's _Cancel_
  * [ ] Code editor syntax highlight
* Pipeline: TOML parse → Draw to canvas / Render to SVG:
  * [ ] Remove `(expecting [X, Y])` from error message (does not make sense in wider context)
  * [x] `[node].type = "text"`
  * [x] `[node].type = "rectangle"`
  * [x] `[node].type = "ellipse"`
  * [x] `[node].type = "diamond"`
  * [ ] `[node].color_border`, don't forget to generalize the error message
  * [ ] `[node].label_shift`, allow for variables, don't forget about zoom level
  * [ ] Allow setting individual start/end path shift, keep backwards compatible
  * [ ] `[[path]].label`
  * [ ] `[[path]].label_bg`
* Canvas features:
  * [ ] Canvas zooming: anchor under mouse
  * [ ] Change the cursor type while dragging the canvas
  * [ ] Ctrl+Click to jump to node definition in text editor
* Settings window:
  * [ ] Switch between vanilla and 3rd party text editor
  * [ ] Appearance: color theme
  * [ ] Option for forcing light canvas regardless of color theme
  * [ ] Duplicate settings from app: view→grid/toolbar/toolbar2, text editor font size (so all things that can be set and saved are at one place)
* Benchmark window:
  * [ ] Do?
* Icons:
  * [ ] Main menu bar → File items
  * [ ] Toolbar → Drop-down list with node types
  * [ ] Secondary canvas toolbar → Drag'n'Drop buttons
  * [ ] UnsavedWarn modal
* TOML Parser:
  * [ ] Refactor TOML parsing: do not std::move into priority queue, use pq just for determining draw order and nothing more

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

## Dependencies

* [Qt](https://www.qt.io/development/qt-framework) ([LGPL](https://doc.qt.io/qt-6/lgpl.html))
* [toml++](https://github.com/marzer/tomlplusplus) ([MIT](https://github.com/marzer/tomlplusplus/blob/master/LICENSE))

<br>

* [Inconsolata](https://levien.com/type/myfonts/inconsolata.html) ([SIL Open Font](https://github.com/google/fonts/blob/main/ofl/inconsolata/OFL.txt))

## Problems
