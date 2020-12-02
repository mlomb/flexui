#pragma once

#include <stdint.h>

namespace flexui {

	struct UIVec2 {
		float x, y;

		inline bool operator==(const UIVec2& b) const { return x == b.x && y == b.y; }
		inline bool operator!=(const UIVec2& b) const { return x != b.x || y != b.y; }
		inline UIVec2& operator+=(const UIVec2& b) {
			this->x += b.x;
			this->y += b.y;
			return *this;
		}
	};

	struct UIRect {
		union {
			struct {
				UIVec2 position;
				UIVec2 size;
			};
			struct {
				float x, y, width, height;
			};
		};

		inline bool operator==(const UIRect& b) const { return x == b.x && y == b.y && width == b.width && height == b.height; }
		inline bool operator!=(const UIRect& b) const { return x != b.x || y != b.y || width != b.width || height != b.height; }
	};

	typedef uint16_t UIIndex;
	struct UIVertex {
		UIVec2 pos;
		UIVec2 uv;
		uint32_t color; // AABBGGRR
	};


}