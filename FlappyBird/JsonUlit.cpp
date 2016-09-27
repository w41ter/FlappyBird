#include "main.h"
#include "JsonUlit.h"

#include <fstream>

using std::wstring;
using std::string;
using std::fstream;

// Check that Json has the properties we want.
#include <type_traits>
#define CHECK_TRAIT(x) static_assert(std::x::value, #x)
CHECK_TRAIT(is_nothrow_constructible<Json>);
CHECK_TRAIT(is_nothrow_default_constructible<Json>);
CHECK_TRAIT(is_copy_constructible<Json>);
CHECK_TRAIT(is_nothrow_move_constructible<Json>);
CHECK_TRAIT(is_copy_assignable<Json>);
CHECK_TRAIT(is_nothrow_move_assignable<Json>);
CHECK_TRAIT(is_nothrow_destructible<Json>);

JsonUlit::JsonUlit()
{
}

JsonUlit::~JsonUlit()
{
}

JsonUlit & JsonUlit::instance()
{
    static JsonUlit ulit;
    return ulit;
}

Json JsonUlit::Parse(const wstring & filename)
{
    fstream file(filename, std::ios::in);
    if (!file)
    {
        throw std::runtime_error("open file faliure");
    }

    string buf, line;
    while (std::getline(file, line))
    {
        buf += line + '\n';
    }

    line.clear();
    auto json = Json::parse(buf, line);
    if (!line.empty())
    {
        throw std::runtime_error("parse json is failure");
    }

    return json;
}
//
//const JsonUlit::JsonObject & JsonUlit::object_items(const Json & json)
//{
//    assert(json.is_object());
//    return json.object_items();
//}
//
//const Json & JsonUlit::get_object(const JsonObject & object, const char * name)
//{
//    assert(object.count(name) != 0);
//    return object.find(name)->second;
//}
//
//int JsonUlit::get_int_value(const Json & json)
//{
//    assert(json.is_number());
//    return json.int_value();
//}
//
//const JsonUlit::JsonArray & JsonUlit::get_array(const Json & json)
//{
//    assert()
//}

int JsonUlit::get_int_value(const Json & parent, const char * name, int default_)
{
    return parent[name].is_number() ? parent[name].int_value() : default_;
}

bool JsonUlit::get_boolean_value(const Json & parent, const char * name, bool default_)
{
    return parent[name].is_bool() ? parent[name].bool_value() : default_;
}

double JsonUlit::get_number_value(const Json & parent, const char * name, double default_)
{
    return parent[name].is_number() ? parent[name].number_value() : default_;
}

const std::string & JsonUlit::get_string_value(const Json & parent, const char * name)
{
    return parent[name].string_value();
}

const Json & JsonUlit::get_object_value(const Json & parent, const char * name)
{
    assert(parent[name].is_object());
    return parent[name];
}

const JsonUlit::JsonArray & JsonUlit::get_array_value(const Json & parent, const char * name)
{
    assert(parent[name].is_array());
    return parent[name].array_items();
}
