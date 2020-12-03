#pragma once

#include <algorithm> // for min, max

namespace flexui {

	struct UIVec2 {
		UIVec2(float x, float y) : x(x), y(y) { };
		UIVec2() : x(0), y(0) { };

		float x, y;

		inline bool operator==(const UIVec2& b) const { return x == b.x && y == b.y; }
		inline bool operator!=(const UIVec2& b) const { return x != b.x || y != b.y; }
		inline UIVec2 operator+(const UIVec2& b) const { return { x + b.x, y + b.y }; };
		inline UIVec2 operator-(const UIVec2& b) const { return { x - b.x, y - b.y }; };
		inline UIVec2 operator*(const UIVec2& b) const { return { x * b.x, y * b.y }; };
		inline UIVec2 operator*(const float& k) const { return { x * k, y * k }; };

		inline UIVec2& operator+=(const UIVec2& b) {
			this->x += b.x;
			this->y += b.y;
			return *this;
		}

		static inline UIVec2 Min(const UIVec2& a, const UIVec2& b) { return { std::min(a.x, b.x), std::min(a.y, b.y) }; }
	};

	struct UIRect {
		UIRect(float x, float y, float w, float h) : x(x), y(y), width(w), height(h) { };
		UIRect() : x(0), y(0), width(0), height(0) { };

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

	typedef uint32_t UIColor;
}