#pragma once

#include <vector>
#include <string>

// forward def to avoid include
struct YGNode;

#include "Math.hpp"
#include "Style/StyleDefinitions.hpp"

namespace flexui {

	class StyleSheet;
	struct StyleComputed;
	class Surface;
	class Painter;

	enum class MeasureMode {
		UNDEFINED,
		EXACTLY,
		AT_MOST
	};

	class Element {
	public:
		Element();
		virtual ~Element();

		void addElement(Element* child, int index = -1);
		void removeElement(Element* child);

		void setID(const std::string& id);
		void addClass(const std::string& klass);
		void addStyleSheet(StyleSheet* stylesheet);
		void setStyleProperty(const StyleProperty& property);
		void setPseudoStates(const StylePseudoStates states);
		void removePseudoStates(const StylePseudoStates states);

		Element* getParent() const;
		const std::vector<Element*>& getChildrens() const;
		const std::vector<StyleSheet*>& getStyleSheets() const;
		Rect getBoundingRect() const;
		Rect getContentRect() const;
		bool isVisible() const;
		int getDepth() const;
		Surface* getSurface() const;
		std::string getQualifiedName() const;

		virtual void paintContent(Painter* painter);
		virtual Vec2 measureContent(float width, MeasureMode widthMode, float height, MeasureMode heightMode);

	protected:
		StyleComputed* m_ComputedStyle;

		void setTag(const std::string& tag);

		void setAsTextType();
		void enableMeasurement();

	private:
		friend class Surface;
		friend class LayoutTreeUpdater;
		friend class StyleTreeUpdater;

		void updateHierarchy();

		Element* m_Parent;
		std::vector<Element*> m_Childrens;
		int m_Depth;

		friend class StyleSelectorMatcher;
		StyleIdentifier m_ID, m_Tag;
		std::vector<StyleIdentifier> m_Classes;
		StylePseudoStates m_PseudoStates;
		std::vector<StyleSheet*> m_StyleSheets;
		StyleRule m_InlineRules;

		YGNode* m_YogaNode;
		Rect m_LayoutRect;
		Rect m_BoundingRect;

		Surface* m_Surface;
	};

}
