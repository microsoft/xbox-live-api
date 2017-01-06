//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#pragma once

#include <vector>
#include <utility>
#include <string>
#include <sstream>

#include "shared_macros.h"

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_BEGIN
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN
#endif

std::vector<string_t> GetSegments(string_t path);

template<typename T>
class trie_node
{
public:
    trie_node(_In_ string_t segment) :
        m_segment(std::move(segment)),
        m_hasValue(false)
    {
    }

    std::vector<trie_node>& children()
    {
        return m_children;
    }

    const std::vector<trie_node>& children() const
    {
        return m_children;
    }

    const T value() const
    {
        return m_value;
    }

    bool has_value() const
    {
        return m_hasValue;
    }

    void set_value(_In_ T value)
    {
        m_value = value;
        m_hasValue = true;
    }

    const string_t& segment() const
    {
        return m_segment;
    }

    bool is_leaf() const
    {
        return m_children.empty();
    }

private:
    T m_value;
    bool m_hasValue;
    string_t m_segment;
    std::vector<trie_node> m_children;
};

template<typename T>
class trie
{
public:
    trie();

    void add(
        _In_ string_t path,
        _In_ T value);

    bool get(
        _In_ string_t path, 
        _Out_ T& result) const;

    bool get_exact(
        _In_ string_t path,
        _Out_ T& result) const;

private:
    trie_node<T> m_root;

    bool get_internal(
        _In_ const trie_node<T>& node, 
        _In_ std::vector<string_t>::iterator begin,
        _In_ std::vector<string_t>::iterator end, 
        _Out_ T& result) const;
};

template<typename T>
trie<T>::trie() :
    m_root(string_t())
{
}

template<typename T>
void trie<T>::add(
    _In_ string_t path, 
    _In_ T value)
{
    std::vector<string_t> segments(GetSegments(path));
    trie_node<T>* node = &m_root;

    for (auto it = segments.begin(); it != segments.end(); it++)
    {
        // Search children to see if we have a match
        std::vector<trie_node<T>>& children(node->children());
        bool found = false;

        for (typename std::vector<trie_node<T>>::iterator chit = children.begin(); chit != children.end(); chit++)
        {
            if (chit->segment() == *it)
            {
                node = &*chit;
                found = true;
                break;
            }
        }

        if (!found)
        {
            // No match so add it
            children.emplace_back(*it);
            node = &children.back();
        }
    }

    node->set_value(value);
}

template<typename T>
bool trie<T>::get(_In_ string_t path, _In_ T& result) const
{
    std::vector<string_t> segments(GetSegments(path));
    return get_internal(m_root, segments.begin(), segments.end(), result);
}

// node always points to the longest prefix so far. If node.HasValue() == true, 
// then this is a prefix (but not necessarily the longest prefix).
// begin is the segment to be matched against
// end is marks the last segment
template<typename T>
bool trie<T>::get_internal(
    _In_ const trie_node<T>& node, 
    _In_ std::vector<string_t>::iterator begin, 
    _In_ std::vector<string_t>::iterator end, 
    _Out_ T& result) const
{
    // Only search children if there are more path segments
    if (begin != end)
    {
        const std::vector<trie_node<T>>& children(node.children());
        for (auto child = children.begin(); child != children.end(); ++child)
        {
            if (child->segment() == *begin)
            {
                // We got a match so recurse!
                if (get_internal(*child, begin + 1, end, result))
                {
                    return true;
                }

                break;
            }
        }
    }

    // None of the children matched or there are no more path segments.
    // If this node contains a value, then we found the longest prefix.
    // Otherwise, we return false and let the previous node match.
    if (node.has_value())
    {
        result = node.value();
        return true;
    }

    return false;
}

template<typename T>
bool trie<T>::get_exact(_In_ string_t path, _Out_ T& result) const
{
    std::vector<string_t> segments(GetSegments(path));
    const trie_node<T>* node = &m_root;

    for (auto segment = segments.begin(); segment != segments.end(); ++segment)
    {
        const std::vector<trie_node<T>>& children(node->children());
        bool found = false;
        for (auto child = children.begin(); child != children.end(); ++child)
        {
            if (child->segment() == *segment)
            {
                found = true;
                node = &*child;
                break;
            }
        }

        if (!found)
        {
            return false;
        }
    }

    if (node->has_value())
    {
        result = node->value();
        return true;
    }

    return false;
}

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_END
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
#endif

