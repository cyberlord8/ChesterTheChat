# 📘 DevStyleGuide.md
## *Chester The Chat*

> Coding conventions and design rules to keep the codebase consistent, readable, and scalable.

---
---
### 📦 Project Structure


- Logic is separated from UI.
- Files are named using **snake_case**.
- Classes are named using **PascalCase**.
- Each class has a `.h` and `.cpp` file.

---
---
### 🧠 Class Naming Conventions

| Element        | Format       | Example               |
|----------------|--------------|-----------------------|
| Classes        | `PascalCase` | `MessageManager`      |
| Members        | `camelCase`  | `configSettings`      |
| UI pointers    | Qt-style     | `ui->lineEditMessage` |
| Files          | `snake_case` | `message_manager.cpp` |

> ❌ Avoid prefixes like `ClassMessageManager`.

---
---

### 🔁 Signal / Slot Conventions
---
#### ✅ Signals

- Use **event-style naming** (past tense or notification-style).
- No `signal` prefix needed.
- Should describe what **just happened**.

```cpp
signals:
    void messageReceived(const QString &msg);
    void connectionLost();
    void settingsChanged();
```
---
#### ✅ Slots
 - Use imperative names (like commands or reactions).
 - Begin with handle, on, or update for clarity.
 - Match closely to signal where applicable.
```cpp
private slots:
    void handleMessageReceived(const QString &msg);
    void onSendButtonClicked();
    void updateUiFromSettings();
```
🚫 Avoid:
```cpp
    void signalSomething();   // ❌ No need for prefix
    void slotDoThing();       // ❌ Not descriptive
```
---
---
### 🧼 Settings & UI Decoupling
 - Logic classes (e.g., SettingsManager, UdpManager) should not reference ui->.
 - UI pulls data from logic:
```cpp
ui->checkBox->setChecked(settingsManager->isMulticastEnabled());
```
 - UI pushes updates into logic:
```cpp
settingsManager->setMulticastEnabled(ui->checkBox->isChecked());
```
---
---
### 📄 Comments
Use Doxygen-style documentation:
```cpp
/**
 * @brief Loads all user-defined .qss files from the QStyleSheets folder.
 */
void loadQStyleSheetFolder();
```
Avoid unnecessary comments for self-explanatory code.

---
---
### 🛠️ Code Style
 - Indentation: 4 spaces, no tabs
 - Braces: K&R style

```cpp

if (condition) {
    doThing();
}
```
Use const wherever possible, especially for method args and return types.

---
---

### 🔧 Optional Best Practices
Use QScopedPointer or std::unique_ptr for owned objects.

 - Use QStringLiteral for static strings.
 - Use QDateTime::currentDateTimeUtc() for all logging timestamps.
 
---
---
### 📜 Git & Documentation
 - Follow commit message format:

```makefile
type(scope): summary
Example:
fix(network): handle socket rebinding properly
feat(settings): add persistent stylesheet support
```
 - Update README.md and UserManual.md as needed.
 - Prefer Markdown for documentation (docs/ folder).
---
---
### ✅ Summary
 - Keep things readable, intentional, and consistent.
 - Separate logic from UI
 - Use meaningful names
 - Follow Qt idioms for signals/slots
 - Make every new file easy for others (and future-you) to understand
