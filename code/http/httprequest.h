#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <regex>
#include <errno.h>
#include <mysql/mysql.h>

#include "../buffer/buffer.h"
#include "../log/log.h"
#include "../sqlconnpool/sql_conn_pool.h"
#include "../threadpool/threadpool.h"

using namespace std;

enum HTTP_CODE
{
    NO_REQUEST = 0,
    GET_REQUEST,
    BAD_REQUEST,
    NO_RESOURSE,
    FORBIDDENT_REQUEST,
    FILE_REQUEST,
    INTERNAL_ERROR,
    CLOSED_CONNECTION
};

enum PARSE_STATE
{
    REQUEST_LINE = 0,
    HEADERS,
    BODY,
    FINISH
};

class HttpRequest
{
public:
    HttpRequest() {init();}
    ~HttpRequest() = default;

    void init();
    HTTP_CODE parse(Buffer& buffer);

    string getPathConst() const {return path;}
    string& getPath() {return path;}
    string getMethod() const {return method;}
    string getVersion() const {return version;}
    bool isKeepAlive() const;
    // isDownload:是否下载，isRange:是否断点续传
    bool isDownload, isRange, isEtag;
    // int:支持最大2GB的range请求
    int rangeStart, rangeEnd;
    // 这里Etag的规则是文件名+最后修改时间的hash
    int Etag;

private:
    HTTP_CODE parseRequestLine(const string& line);
    HTTP_CODE parseHeader(const string& line);
    HTTP_CODE parseBody();

    void parsePath();
    void parseFromUrlEncoded();
    void parseFormData();

    static bool userVerify(const string& name, const string& pwd, bool isLogin);

    PARSE_STATE state;
    string method, path, version, body;
    bool linger;
    size_t contentLen;
    unordered_map<string, string> header;
    unordered_map<string, string> post;
    unordered_map<string, string> fileInfo;

    static const unordered_set<string> DEFAULT_HTML;
    static const unordered_map<string, int> DEFAULT_HTML_TAG;
    static int convertHex(char ch);
};

#endif