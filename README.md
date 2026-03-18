# lumina

**lumina** is a digital image processing application designed to do my assignments for my DIP course.

## Quick Start

### Step 1: Install the Dependencies
You need all the libraries below. Use your own package manager. Here I show pacman for Arch.
#### For compiling the app:
```bash
sudo pacman -S base-devel 
```
#### For graphics:
```bash
sudo pacman -S raylib alsa-lib mesa
```
#### For window management
```bash
sudo pacman -S libx11 libxrandr libxi libxcursor libxinerama wayland-protocols libxkbcommon
```

### Step 2: Clone the repo
```bash
git clone --depth=1 https://github.com/mnaetce/lumina.git
```

### Step 3: Bootstrap the Build System
lumina uses `tsoding/nob.h` to build the app. Bootstrap nob by doing this **only once**:
```bash
cc -DLUMINA_NO_REPORT -o nob nob.c
```

### Step 4: Build the App
```bash
./nob
```

Then you can
```bash
./lumina help
```

to know how to use the app.

## Acknowledgements and Third-Party Libraries
Lumina is built using the following incredible open-source tools:

* **[raylib](https://github.com/raysan5/raylib)** - A simple and easy-to-use library to enjoy videogames programming. Licensed under the zlib/libpng license.
* **[stb_image.h](https://github.com/nothings/stb)** by Sean Barrett - Public domain image loader.
* **[nob.h](https://github.com/tsoding/nob.h)** by Alexey Kutepov (Tsoding) - A header-only library for writing build recipes in C. Licensed under the MIT License.

