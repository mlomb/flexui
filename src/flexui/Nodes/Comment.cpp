#include "flexui/Nodes/Comment.hpp"

namespace flexui {

    Comment::Comment(const std::string& text)
        : m_Text(text)
    {
    }

    Comment::~Comment() {
    }

    std::string Comment::getDebugInfo() const
    {
        return Node::getDebugInfo() + " \"" + m_Text + "\"";
    }

}