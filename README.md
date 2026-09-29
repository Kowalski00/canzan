# canzan

## dependencies

```bash
sudo apt install libgtk-4-dev build-essential pkg-config

sudo apt install libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev

sudo apt install meson ninja-build
```

## building


```bash
meson setup build

ninja -C build
```

to clear:


```bash
ninja -C build clean

```

