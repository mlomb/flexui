#pragma once

#include <vector>
#include <string>

#include "flexui/Nodes/ContainerNode.hpp"

#include "flexui/Math.hpp"
#include "flexui/Style/StyleDefinitions.hpp"

namespace flexui {

	class StyleSheet;
	struct StyleComputed;
	class Surface;
    class Painter;
    class EventBase;

	// A Element is the base class for all elements in the UI tree
	//
	// An Elements object can be attached to a tree
	class Element : public ContainerNode {
	public:
		Element(const std::string& tag);
		virtual ~Element();

		void setID(const std::string& id);
		void addClass(const std::string& klass);
		void setStyleProperty(const StyleProperty& property);
		void setPseudoStates(const StylePseudoStates states);
		void removePseudoStates(const StylePseudoStates states);

		Rect getBoundingRect() const { return Rect(); };
		Rect getContentRect() const;
		bool isVisible() const;
		const StyleComputed* getComputedStyle() const { return m_ComputedStyle; };

		std::string getNodeName() const override { return m_Tag.text; }
		std::string getDebugInfo() const override;
		NodeType getNodeType() const override { return NodeType::ELEMENT; }

		//virtual Vec2 measureContent(float width, MeasureMode widthMode, float height, MeasureMode heightMode) { return Vec2(); };

		virtual void drawContent(Painter& painter) override;
        virtual void executeDefault(EventBase* evt);

	private:
		friend class Surface;
		friend class LayoutTreeUpdater;
		friend class StyleTreeUpdater;

		friend class StyleSelectorMatcher;
		StyleIdentifier m_ID, m_Tag;
		std::vector<StyleIdentifier> m_Classes;
		StylePseudoStates m_PseudoStates;
		StyleRule m_InlineRules;

		friend class StyleEngine;

		StyleComputed* m_ComputedStyle;

		void setTag(const std::string& tag);
	};

}
