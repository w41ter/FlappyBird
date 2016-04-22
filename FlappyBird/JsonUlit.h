#pragma once

#include "json11.hpp"

using namespace json11;

class JsonUlit
{
    using Json = json11::Json;

    JsonUlit();
    ~JsonUlit();
public:
    using JsonObject = std::map<std::string, Json>;
    using JsonArray = std::vector<Json>;

    static JsonUlit & instance();

    Json Parse(const std::wstring &filename);

    int get_int_value(const Json &parent, const char *name, int default_ = 0);
    bool get_boolean_value(const Json &parent, const char *name, bool default_ = false);
    double get_number_value(const Json &parent, const char *name, double default_ = 0.f);
    const std::string &get_string_value(const Json &parent, const char *name);
    const Json & get_object_value(const Json &parent, const char *name);
    const JsonArray & get_array_value(const Json &parent, const char *name);
};