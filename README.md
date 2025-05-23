# Chester The Chat

**Chester The Chat** is a lightweight IP/UDP-based local network chat application built with the Qt Framework (C++). It's designed to support multiple instances on the same machine, each with isolated configuration, making it ideal for local testing or LAN-based messaging scenarios.

![image](https://github.com/cyberlord8/ChesterTheChat/blob/main/resources/screenshots/Alice.png)
![image](https://github.com/cyberlord8/ChesterTheChat/blob/main/resources/screenshots/Caterpillar.png)
![image](https://github.com/cyberlord8/ChesterTheChat/blob/main/resources/screenshots/Jabberwocky.png)
---

## 🚀 Features

- 🗨️ Send and receive chat messages over UDP multicast or unicast.
- 🔄 Supports multiple instances with auto-assigned configuration files.
- 🎨 Optional custom stylesheets (`.qss`) for a personalized look.
- 🖼️ Optional background image toggle.
- 🔧 Configurable TTL, ports, network interfaces.
- 💾 Persistent settings per instance (`instance_X_settings.ini`).
- 🛠️ Built with Qt (C++), cross-platform compatible.

## 📘 User Manual
[User Manual](https://github.com/cyberlord8/ChesterTheChat/blob/main/resources/UserManual.pdf)

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
| ![image](https://github.com/user-attachments/assets/609015a8-676e-4667-b261-88d220bbc8a6) | ![image](https://github.com/user-attachments/assets/9456f8d0-5244-4ea0-9128-69f61fa085ce) | ![image](https://github.com/user-attachments/assets/c5d58f64-9c4b-436d-852f-d860ad255839) |

---

## 📘 License
Licensed under the [GNU LGPL v3](https://www.gnu.org/licenses/lgpl-3.0.html).

---

## 🙌 Acknowledgements

- Developed using the **Qt Framework**
- Based on C++ best practices and Qt design patterns

---

## 🧠 About the Author

Developed by cyberlord8 — if you use this project or find it helpful, give it a ⭐ on GitHub or drop a message!

