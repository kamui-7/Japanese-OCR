# Gazou OCR 

Gazou is a Japanese OCR for Linux written in C++. It utilizes image processing, and other parameters to improve the accuracy. All contributions are welcome.

## Installation

This program is on the AUR so Arch based systems can install it with their AUR helper like this:
```
yay -S gazou-git
```

For other distros, you will have to build it from source.

## Building from source
### Dependencies

These dependencies will need to be installed by your system's package manager. 

- Qt5 >= 5.10
- Qt5X11Extras >= 5.10
- Tesseract >= 4.0.0
- Leptonica >= 1.70

### Install

```sh
git clone --recursive https://github.com/kamui-7/Gazou-OCR.git
cd Gazou-OCR
mkdir build
cd build
cmake .. 
sudo make install
```

## Usage

You can run the program by typing `gazou` in your terminal. It runs in the background and should appear in your system tray. If you click on the app in the tray, a settings dialog should appear. In here, you can customize the keybinds to your liking. The default hotkeys are

- `Alt+A`: Vertical OCR
- `Alt+D`: Horizontal OCR
- `Alt+S`: Repeat the previous OCR

When you perform an OCR, the result text gets copied to the clipboard.

Gazou also has a command line mode, and this can be useful for integrating it with bash scripts. It takes two different parameters, the image path and the orientation, which is either `vertical (-v)` or `horizontal (-h)`. Here's an example:

```bash
gazou -h page103.png
```
This command scans all the text in the image and prints the resulting text.

For more help Use: `gazou --help`
