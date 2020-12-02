#pragma once

namespace flexui {

	class Element;
	class Surface;

	enum class VersionChangeType {
		STYLE,
		LAYOUT,
		PAINT
	};

	class TreeProcessor {
	public:
		TreeProcessor(Surface* surface);
		virtual ~TreeProcessor();

		virtual void process() = 0;

	protected:
		Surface* m_Surface;

		virtual void onVersionChange(Element* element, VersionChangeType changes);
	};

}