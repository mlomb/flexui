#pragma once

#include <algorithm> // for min, max
#include <cstdint>

namespace flexui {

	struct Vec2 {
		float x, y;

		inline bool operator==(const Vec2& b) const { return x == b.x && y == b.y; }
		inline bool operator!=(const Vec2& b) const { return x != b.x || y != b.y; }
		inline Vec2 operator+(const Vec2& b) const { return { x + b.x, y + b.y }; };
		inline Vec2 operator-(const Vec2& b) const { return { x - b.x, y - b.y }; };
		inline Vec2 operator*(const Vec2& b) const { return { x * b.x, y * b.y }; };
		inline Vec2 operator*(const float& k) const { return { x * k, y * k }; };

		inline Vec2& operator+=(const Vec2& b) {
			this->x += b.x;
			this->y += b.y;
			return *this;
		}

		static inline Vec2 Min(const Vec2& a, const Vec2& b) { return { std::min(a.x, b.x), std::min(a.y, b.y) }; }
	};

	struct Rect {
		Rect(float x, float y, float w, float h) : x(x), y(y), width(w), height(h) { };
		Rect() : x(0), y(0), width(0), height(0) { };

		union {
			struct {
				Vec2 position;
				Vec2 size;
			};
			struct {
				float x, y, width, height;
			};
		};

		inline bool operator==(const Rect& b) const { return x == b.x && y == b.y && width == b.width && height == b.height; }
		inline bool operator!=(const Rect& b) const { return x != b.x || y != b.y || width != b.width || height != b.height; }

		inline bool contains(const Vec2& point) const {
            return point.x >= x && point.y >= y && point.x <= x + width && point.y <= y + height;
		}
	};

	typedef uint32_t Color;

    typedef uint32_t StrHash;

    constexpr const StrHash HashStr(const char* str, const int i = 0)
    {
        // TODO: maybe find a better hash
        return str[i] ? (HashStr(str, i + 1) * 33) ^ str[i] : 5381;
    }
}
