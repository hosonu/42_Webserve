#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <cctype>
#include <stdexcept>

// ヘッダーを保存するためのmap型
typedef std::map<std::string, std::string> HeaderMap;

// トリム関数：前後の空白を削除
void trim(std::string& str) {
    size_t start = str.find_first_not_of(" \t");
    size_t end = str.find_last_not_of(" \t");

    if (start == std::string::npos || end == std::string::npos) {
        str.clear();
    } else {
        str = str.substr(start, end - start + 1);
    }
}

// ヘッダーフィールド名が有効かをチェック
bool isValidHeaderFieldName(const std::string& fieldName) {
    // ヘッダ名はアルファベット、数字、-、_のみ
    for (size_t i = 0; i < fieldName.size(); ++i) {
        char c = fieldName[i];
        if (!(std::isalnum(c) || c == '-' || c == '_')) {
            return false;
        }
    }
    return true;
}

// ヘッダーフィールドの値が有効かをチェック
bool isValidHeaderFieldValue(const std::string& fieldValue) {
    // 制御文字とLF以外の不可視文字を許容しない
    for (size_t i = 0; i < fieldValue.size(); ++i) {
        char c = fieldValue[i];
        if (std::iscntrl(c) && c != '\r' && c != '\n') {
            return false;
        }
    }
    return true;
}

// ヘッダーフィールドのパース
void parseHeaderField(const std::string& line, HeaderMap& headers) {
    size_t colonPos = line.find(':');
    if (colonPos == std::string::npos) {
        throw std::invalid_argument("Invalid header format, missing colon");
    }

    std::string fieldName = line.substr(0, colonPos);
    std::string fieldValue = line.substr(colonPos + 1);

    // トリム
    trim(fieldName);
    trim(fieldValue);

    // ヘッダ名と値の検証
    if (!isValidHeaderFieldName(fieldName)) {
        throw std::invalid_argument("Invalid header field name: " + fieldName);
    }
    if (!isValidHeaderFieldValue(fieldValue)) {
        throw std::invalid_argument("Invalid header field value: " + fieldValue);
    }

    // 小文字に変換
    for (size_t i = 0; i < fieldName.size(); ++i) {
        fieldName[i] = std::tolower(fieldName[i]);
    }

    // ヘッダーフィールドを格納
    headers[fieldName] = fieldValue;
}

// ヘッダーをパースする
void parseHeaders(const std::string& httpMessage, HeaderMap& headers) {
    std::istringstream stream(httpMessage);
    std::string line;

    while (std::getline(stream, line)) {
        trim(line);

        // 空行に遭遇したらヘッダの終了
        if (line.empty()) {
            break;
        }

        // ヘッダーフィールドをパース
        parseHeaderField(line, headers);
    }
}

// ヘッダの出力
void printHeaders(const HeaderMap& headers) {
    for (HeaderMap::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        std::cout << it->first << ": " << it->second << std::endl;
    }
}

int main() {
    // 不正なHTTPメッセージの例
    std::string httpMessage =
        "Host: example.com\r\n"
        "User-Agent: curl/7.68.0\r\n"
        "Accept: */*\r\n"
        "Invalid-Header: value;\r\n"  // 不正なヘッダフィールド名（';'含む）
        "Content-Type: text/html; charset=UTF-8\r\n"
        "\r\n";

    HeaderMap headers;

    try {
        parseHeaders(httpMessage, headers);
        printHeaders(headers);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
