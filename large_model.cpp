#include "large_model.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

namespace {

const char* MODEL_NAME = "deepseek-v4-flash";
const char* REQUEST_PATH = "deepseek_request.json";
const char* RESPONSE_PATH = "deepseek_response.json";

string jsonEscape(const string& text) {
    string result;
    result.reserve(text.size() + 16);

    for (const unsigned char ch : text) {
        switch (ch) {
            case '"':
                result += "\\\"";
                break;
            case '\\':
                result += "\\\\";
                break;
            case '\b':
                result += "\\b";
                break;
            case '\f':
                result += "\\f";
                break;
            case '\n':
                result += "\\n";
                break;
            case '\r':
                result += "\\r";
                break;
            case '\t':
                result += "\\t";
                break;
            default:
                if (ch < 0x20) {
                    char buf[7] = {0};
                    snprintf(buf, sizeof(buf), "\\u%04x", ch);
                    result += buf;
                } else {
                    result += (char) ch;
                }
                break;
        }
    }

    return result;
}

void appendUtf8FromCodePoint(string& out, unsigned int codePoint) {
    if (codePoint <= 0x7F) {
        out += (char) codePoint;
    } else if (codePoint <= 0x7FF) {
        out += (char) (0xC0 | (codePoint >> 6));
        out += (char) (0x80 | (codePoint & 0x3F));
    } else if (codePoint <= 0xFFFF) {
        out += (char) (0xE0 | (codePoint >> 12));
        out += (char) (0x80 | ((codePoint >> 6) & 0x3F));
        out += (char) (0x80 | (codePoint & 0x3F));
    } else {
        out += (char) (0xF0 | (codePoint >> 18));
        out += (char) (0x80 | ((codePoint >> 12) & 0x3F));
        out += (char) (0x80 | ((codePoint >> 6) & 0x3F));
        out += (char) (0x80 | (codePoint & 0x3F));
    }
}

int hexValue(char ch) {
    if (ch >= '0' && ch <= '9') {
        return ch - '0';
    }
    if (ch >= 'a' && ch <= 'f') {
        return ch - 'a' + 10;
    }
    if (ch >= 'A' && ch <= 'F') {
        return ch - 'A' + 10;
    }
    return -1;
}

string jsonUnescape(const string& text) {
    string result;
    result.reserve(text.size());

    for (size_t i = 0; i < text.size(); i++) {
        if (text[i] != '\\' || i + 1 >= text.size()) {
            result += text[i];
            continue;
        }

        const char escaped = text[++i];
        switch (escaped) {
            case '"':
                result += '"';
                break;
            case '\\':
                result += '\\';
                break;
            case '/':
                result += '/';
                break;
            case 'b':
                result += '\b';
                break;
            case 'f':
                result += '\f';
                break;
            case 'n':
                result += '\n';
                break;
            case 'r':
                result += '\r';
                break;
            case 't':
                result += '\t';
                break;
            case 'u': {
                if (i + 4 >= text.size()) {
                    result += "\\u";
                    break;
                }

                unsigned int codePoint = 0;
                bool valid = true;
                for (int n = 0; n < 4; n++) {
                    const int value = hexValue(text[i + 1 + n]);
                    if (value < 0) {
                        valid = false;
                        break;
                    }
                    codePoint = (codePoint << 4) | (unsigned int) value;
                }

                if (!valid) {
                    result += "\\u";
                    break;
                }

                i += 4;
                appendUtf8FromCodePoint(result, codePoint);
                break;
            }
            default:
                result += escaped;
                break;
        }
    }

    return result;
}

bool extractJsonStringField(const string& json, const string& field, string& value) {
    const string marker = "\"" + field + "\":\"";
    size_t start = json.find(marker);
    if (start == string::npos) {
        return false;
    }

    start += marker.size();
    size_t end = start;
    bool escaped = false;
    while (end < json.size()) {
        if (json[end] == '\\' && !escaped) {
            escaped = true;
        } else if (json[end] == '"' && !escaped) {
            value = jsonUnescape(json.substr(start, end - start));
            return true;
        } else {
            escaped = false;
        }
        end++;
    }

    return false;
}

string readWholeFile(const char* path) {
    ifstream in(path, ios::binary);
    if (!in) {
        return "";
    }

    stringstream buffer;
    buffer << in.rdbuf();
    return buffer.str();
}

bool writeRequestFile(const string& question) {
    ofstream out(REQUEST_PATH, ios::binary);
    if (!out) {
        return false;
    }

    const string systemPrompt =
        "You are an AI assistant for a volunteer service points system for left-behind children. "
        "Answer in Chinese plain text. Help users understand registration, account query, "
        "service timing, point acquisition, point usage, account cancellation, and donations. "
        "Do not use Markdown.";

    out << "{";
    out << "\"model\":\"" << MODEL_NAME << "\",";
    out << "\"messages\":[";
    out << "{\"role\":\"system\",\"content\":\"" << jsonEscape(systemPrompt) << "\"},";
    out << "{\"role\":\"user\",\"content\":\"" << jsonEscape(question) << "\"}";
    out << "],";
    out << "\"thinking\":{\"type\":\"disabled\"},";
    out << "\"stream\":false";
    out << "}";

    return true;
}

bool callDeepSeekApi() {
    const char* apiKey = getenv("DEEPSEEK_API_KEY");
    if (apiKey == nullptr || apiKey[0] == '\0') {
        cout << "未配置 DEEPSEEK_API_KEY 环境变量，无法调用 DeepSeek API。" << endl;
        return false;
    }

    const string command =
        "curl -sS -X POST \"https://api.deepseek.com/chat/completions\" "
        "-H \"Content-Type: application/json\" "
        "-H \"Authorization: Bearer %DEEPSEEK_API_KEY%\" "
        "--data-binary @" + string(REQUEST_PATH) + " > " + RESPONSE_PATH;

    const int result = system(command.c_str());
    if (result != 0) {
        cout << "DeepSeek API 调用失败，curl 返回码: " << result << endl;
        return false;
    }

    return true;
}

void printDeepSeekResponse() {
    const string response = readWholeFile(RESPONSE_PATH);
    if (response.empty()) {
        cout << "未读取到 DeepSeek 响应。" << endl;
        return;
    }

    string errorMessage;
    if (response.find("\"error\"") != string::npos && extractJsonStringField(response, "message", errorMessage)) {
        cout << "DeepSeek API 返回错误: " << errorMessage << endl;
        return;
    }

    string content;
    if (!extractJsonStringField(response, "content", content)) {
        cout << "无法解析 DeepSeek 响应。" << endl;
        cout << response.substr(0, 500) << endl;
        return;
    }

    cout << endl << "AI 回答:" << endl;
    cout << content << endl;
}

void cleanupTempFiles() {
    remove(REQUEST_PATH);
    remove(RESPONSE_PATH);
}

}

void runDeepSeekAssistant() {
    cout << "----------AI 助手 (DeepSeek V4 Flash)----------" << endl;
    cout << "输入 0 返回主菜单。" << endl << endl;

    string question;
    while (true) {
        cout << "请输入你的问题: ";
        getline(cin, question);

        if (question.empty()) {
            continue;
        }

        if (question == "0") {
            break;
        }

        if (!writeRequestFile(question)) {
            cout << "无法创建请求文件。" << endl;
            continue;
        }

        cout << "正在请求 DeepSeek V4 Flash，请稍候..." << endl;
        if (callDeepSeekApi()) {
            printDeepSeekResponse();
        }

        cleanupTempFiles();
        cout << endl;
    }
}
