#pragma once

#include "TreeProcessor.hpp"

namespace flexui {

	class Painter;

	class TreePainter : public TreeProcessor {
	public:
		TreePainter(Surface* surface);
		virtual ~TreePainter();

		void process() override;

		Painter* getPainter() const;

	private:
		Painter* m_Painter;

		void rec(Element* el);
	};

}