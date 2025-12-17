#ifndef PARSER_H
#define PARSER_H

#include "types.h"
#include <string>

class XmlParser {
public:
    XmlParser(const std::string& s);
    Node parse();

private:
    std::string src;
    size_t pos;

    Node parse_node();
    char peek();
    void skip_ws();
    void expect(char c);
    std::string read_word();
    std::string read_until(char c);
};

#endif
