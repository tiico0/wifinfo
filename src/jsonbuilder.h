// very simple JSON "flat" builder
// rene-d 2020

#pragma once

#include <Arduino.h>

class JSONBuilder
{
    String &s;

public:
    explicit JSONBuilder(String &r) : s(r)
    {
        s.concat("{\"");
    }

    explicit JSONBuilder(String &r, size_t reserve_size) : s(r)
    {
        if (reserve_size != 0)
            s.reserve(reserve_size);
        s.clear();
        s.concat("{\"");
    }

    template <typename T>
    void append(const T *name, const char *value, bool last = false)
    {
        s.concat(name);
        s.concat("\":\"");
        s.concat(value);

        if (last)
            s.concat("\"}");
        else
            s.concat("\",\"");
    }

    template <typename T>
    void append(const T *name, uint32_t value, bool last = false)
    {
        s.concat(name);
        s.concat("\":");
        s.concat(value);

        if (last)
            s.concat("}");
        else
            s.concat(",\"");
    }

    template <typename T>
    void append_without_quote(const T *name, const char *value, bool last = false)
    {
        s.concat(name);
        s.concat("\":");
        s.concat(value);

        if (last)
            s.concat("}");
        else
            s.concat(",\"");
    }

    void finalize()
    {
        // nota:
        //  s a toujours au moins 2 caractères:
        //  {"  ou se termine par ",

        s.remove(s.length() - 1, 1); // supprime " ou , de fin

        char &last = s[s.length() - 1];
        if (last == '{')
        {
            // dict vide
            s.concat('}');
        }
        else
        {
            // remplace la , de fin par }
            last = '}';
        }
    }
};

class JSONTableBuilder
{
    String &s;

public:
    explicit JSONTableBuilder(String &r) : s(r)
    {
        s.concat("[");
    }

    explicit JSONTableBuilder(String &r, size_t reserve_size) : s(r)
    {
        if (reserve_size != 0)
            s.reserve(reserve_size);
        s.clear();
        s.concat("[");
    }

    template <typename T>
    void append(const T *name, const char *value)
    {
        s.concat("{\"na\":\"");
        s.concat(name);
        s.concat("\",\"va\":\"");
        s.concat(value);
        s.concat("\"},");
    }

    template <typename T>
    void append(const T *name, const String &value)
    {
        append(name, value.c_str());
    }

    template <typename T>
    void append(const T *name, uint32_t value)
    {
        s.concat("{\"na\":\"");
        s.concat(name);
        s.concat("\",\"va\":");
        s.concat(value);
        s.concat("},");
    }

    void finalize()
    {
        // s a toujours au moins 1 caractère
        char &last = s[s.length() - 1];
        if (last == '[')
            s.concat(']');
        else
            last = ']';
    }
};
