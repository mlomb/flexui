#pragma once

#include <vector>
#include <string>

#include "flexui/Nodes/ContainerNode.hpp"
#include "flexui/Math.hpp"
#include "flexui/Style/StyleDefinitions.hpp"
#include "flexui/Style/StyleProperty.hpp"
#include "flexui/Selectors/Selector.hpp"
#include "flexui/Misc/String.hpp"

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

		void setID(const HashedString& id);
		void addClass(const HashedString& klass);
		void setPseudoStates(const PseudoStates states);
		void removePseudoStates(const PseudoStates states);

		Rect getBoundingRect() const { return Rect(); };
		Rect getContentRect() const;
		bool isVisible() const;
		const StyleComputed* getComputedStyle() const { return m_ComputedStyle; };

		std::string getNodeName() const override { return m_Tag; }
		std::string getDebugInfo() const override;
		NodeType getNodeType() const override { return NodeType::ELEMENT; }

		const HashedString& getID() const { return m_ID; }
		const HashedString& getTag() const { return m_Tag; }
		const std::vector<HashedString>& getClasses() const { return m_Classes; }
		PseudoStates getPseudoStates() const { return m_PseudoStates; }

		//virtual Vec2 measureContent(float width, MeasureMode widthMode, float height, MeasureMode heightMode) { return Vec2(); };

		virtual void drawContent(Painter& painter) override;
        virtual void executeDefault(EventBase* evt);

	private:
		friend class Surface;
		friend class LayoutTreeUpdater;
		friend class StyleTreeUpdater;

		friend class StyleSelectorMatcher;
		HashedString m_ID, m_Tag;
		std::vector<HashedString> m_Classes;
		PseudoStates m_PseudoStates;
		std::vector<StyleProperty> m_InlineStyleProperties;

		friend class StyleEngine;

		StyleComputed* m_ComputedStyle;
	};

}
