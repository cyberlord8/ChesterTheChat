# Chester The Chat

**Chester The Chat** is a lightweight IP/UDP-based local network chat application built with the Qt Framework (C++). It's designed to support multiple instances on the same machine, each with isolated configuration, making it ideal for local testing or LAN-based messaging scenarios.

![Screenshot Placeholder](docs/screenshots/main_window.png)

---

## 🚀 Features

- 🗨️ Send and receive chat messages over UDP multicast or unicast.
- 🔄 Supports multiple instances with auto-assigned configuration files.
- 🎨 Optional custom stylesheets (`.qss`) for a personalized look.
- 🖼️ Optional background image toggle.
- 🔧 Configurable TTL, ports, network interfaces.
- 💾 Persistent settings per instance (`instance_X_settings.ini`).
- 🛠️ Built with Qt (C++), cross-platform compatible.

---

## 🛠️ Build Instructions

### Requirements

- Qt 5.15+ or Qt 6.x
- C++17 (or higher)
- Qt Creator or any CMake/qmake-based setup
- MinGW 64 (or equivalent compiler)

### Build using Qt Creator

1. Open `ChesterTheChat.pro` in Qt Creator.
2. Configure your desired kit (MinGW or MSVC).
3. Build and run.

### Run multiple instances

Each instance creates its own `.ini` file (`instance_1_settings.ini`, `instance_2_settings.ini`, etc.) in the executable directory. This allows running multiple sessions concurrently from the same folder.

---

## 📸 Screenshots

| Chat Window | Network Configuration | Style Sheet Example |
|-------------|------------------------|---------------------|
| ![](docs/screenshots/chat.png) | ![](docs/screenshots/network.png) | ![](docs/screenshots/style.png) |

> Add your screenshots in `docs/screenshots/`.

---

## 📘 License

This project is licensed under the LGPL v3 license. See [LGPLv3.pdf](LGPLv3.pdf) for details.

---

## 🙌 Acknowledgements

- Developed using the **Qt Framework**
- Licensed fonts, icons, and backgrounds (if applicable)
- Based on C++ best practices and Qt design patterns

---

## 🧠 About the Author

Developed by [Your Name or Handle] — if you use this project or find it helpful, give it a ⭐ on GitHub or drop a message!

