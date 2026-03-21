#pragma once

#include <QList>

#include <optional>
#include <string>

#include "QtAwesome.h"

// These types of node are available
enum NodeType
{
    NTYPE_RECTANGLE,
    NTYPE_ELLIPSE,
    NTYPE_DIAMOND,
    NTYPE_TEXT,
};

// These are the values for the toolbar's ComboBox, where user can change type of the selected node
const QStringList NODE_TYPE_NAMES = {"Rectangle", "Ellipse", "Diamond", "Text"};
const auto N_NTYPES = NODE_TYPE_NAMES.length();
// (used for buttons as well)
constexpr int NODE_TYPE_ICONS[] = {
    fa::fa_square, fa::fa_circle, fa::fa_diamond, fa::fa_a
};

// When parsing TOML, the value of "type=" parameter is converted to this enum
constexpr std::optional<NodeType> GetNodeTypeFromString(const std::string& type_str)
{
    if (type_str.empty() || type_str == "rectangle") return NTYPE_RECTANGLE;
    if (type_str == "ellipse") return NTYPE_ELLIPSE;
    if (type_str == "diamond") return NTYPE_DIAMOND;
    if (type_str == "text") return NTYPE_TEXT;
    return std::nullopt;
}

// Show this error if user provides an unknown type
const std::string NODETYPE_ERROR_MESSAGE =
    "Allowed NodeType values are: 'text', 'rectangle', 'ellipse', 'diamond'";

// Used when adding new node or editing node type via ComboBox (to know how to set "type=" value).
// Also used in tooltips for buttons to add new node.
constexpr const char* GetQuotedStringFromNodeType(const NodeType type)
{
    switch (type) {
    case NTYPE_RECTANGLE:
        return "\"rectangle\"";
    case NTYPE_ELLIPSE:
        return "\"ellipse\"";
    case NTYPE_DIAMOND:
        return "\"diamond\"";
    case NTYPE_TEXT:
        return "\"text\"";
    default:
        return "\"\"";
    }
}
