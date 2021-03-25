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

	enum class MeasureMode {
		UNDEFINED,
		EXACTLY,
		AT_MOST
	};

	// A Element is the base class for all elements in the UI tree
	//
	// An Elements object can be attached to a tree
	class Element : public ContainerNode {
	public:
		Element();
		virtual ~Element();

		void setID(const std::string& id);
		void addClass(const std::string& klass);
		void addStyleSheet(StyleSheet* stylesheet);
		void setStyleProperty(const StyleProperty& property);
		void setPseudoStates(const StylePseudoStates states);
		void removePseudoStates(const StylePseudoStates states);

		const std::vector<StyleSheet*>& getStyleSheets() const;
		Rect getBoundingRect() const;
		Rect getContentRect() const;
		bool isVisible() const;
		Surface* getSurface() const;
		std::string getDebugLine() const;

		NodeType getNodeType() const override { return NodeType::ELEMENT; }

		virtual void paintContent(Painter* painter);
        virtual Vec2 measureContent(float width, MeasureMode widthMode, float height, MeasureMode heightMode);
        virtual void executeDefault(EventBase* evt);
		virtual std::string getName() const { return "Element"; };

		int getDepth() const { return 5; } // TODO

	protected:
		StyleComputed* m_ComputedStyle;

		void setTag(const std::string& tag);

		void setAsTextType();
		void enableMeasurement();

	private:
		friend class Surface;
		friend class LayoutTreeUpdater;
		friend class StyleTreeUpdater;

		friend class StyleSelectorMatcher;
		StyleIdentifier m_ID, m_Tag;
		std::vector<StyleIdentifier> m_Classes;
		StylePseudoStates m_PseudoStates;
		std::vector<StyleSheet*> m_StyleSheets;
		StyleRule m_InlineRules;

		Rect m_LayoutRect;
		Rect m_BoundingRect;

		Surface* m_Surface;
	};

}
