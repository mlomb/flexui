#include "flexui/Render/DynamicAtlasAllocator.hpp"

#include <cmath>
#include <cassert>

namespace flexui {

	inline bool isPowerOfTwo(unsigned int x) {
		return x == 0 || (x & (x - 1)) == 0;
	}

	inline unsigned int nextPowerOfTwo(unsigned int x) {
		x--;
		x |= x >> 1;
		x |= x >> 2;
		x |= x >> 4;
		x |= x >> 8;
		x |= x >> 16;
		x++;
		return x;
	}

	inline unsigned int getLog2OfNextPower(int n) {
		float next = (float)nextPowerOfTwo(n);
		float pow = std::log2(next);
		return (unsigned int)floor(pow);
	}

	DynamicAtlasAllocator::DynamicAtlasAllocator(unsigned int initialSize, unsigned int maxSize)
		: m_FirstUnpartitionedArea(nullptr)
	{
		assert(initialSize > 0 && initialSize <= maxSize);
		assert(isPowerOfTwo(initialSize) && isPowerOfTwo(maxSize));

		m_MaxSize = maxSize;
		m_PhysicalSize = initialSize;

		int maxOpenRows = getLog2OfNextPower(maxSize) + 1;
		m_OpenRows.resize(maxOpenRows, nullptr);

		m_FirstUnpartitionedArea = new Area();
		m_FirstUnpartitionedArea->x = 0;
		m_FirstUnpartitionedArea->y = 0;
		m_FirstUnpartitionedArea->width = initialSize;
		m_FirstUnpartitionedArea->height = initialSize;

		buildAreas(initialSize);
	}

	DynamicAtlasAllocator::~DynamicAtlasAllocator()
	{
		for (Row* row : m_OpenRows)
			delete row;
		if (m_FirstUnpartitionedArea) {
			Area* current = m_FirstUnpartitionedArea;
			assert(current->prev == nullptr);
			while (current) {
				Area* next = current->next;
				delete current;
				current = next;
			}
		}
	}

	bool DynamicAtlasAllocator::tryAllocate(unsigned int width, unsigned int height, unsigned int& locationX, unsigned int& locationY)
	{
		if (width < 1 || height < 1)
			return false;
		if (width > m_MaxSize || height > m_MaxSize)
			return false;

		int rowIndex = getLog2OfNextPower(height);
		int rowHeight = 1 << rowIndex;
		assert(rowIndex >= 0 && rowIndex < m_OpenRows.size());
		Row* row = m_OpenRows[rowIndex];

		if (row && row->width - row->cursor < width)
			row = nullptr;

		if (!row) {
			// attempt a partition
			Area* current = m_FirstUnpartitionedArea;
			while (current) {
				if (tryPartitionArea(current, rowIndex, rowHeight, width)) {
					row = m_OpenRows[rowIndex];
					break;
				}

				current = current->next;
			}

			if (!row)
				return false;
		}

		locationX = row->offsetX + row->cursor;
		locationY = row->offsetY;
		row->cursor += width;
		assert(row->cursor <= row->width);

		m_PhysicalSize =
			std::max(
				m_PhysicalSize,
				std::max(
					nextPowerOfTwo(std::max(locationX, locationX + width)),
					nextPowerOfTwo(std::max(locationY, locationY + height))
				)
			);

		return true;
	}

	bool DynamicAtlasAllocator::tryPartitionArea(Area* area, unsigned int rowIndex, unsigned int rowHeight, unsigned int minWidth)
	{
		if (area->height < rowHeight || area->width < minWidth)
			return false;

		Row* row = m_OpenRows[rowIndex];
		if (!row)
			m_OpenRows[rowIndex] = row = new Row();
		row->cursor = 0;
		row->offsetX = area->x;
		row->offsetY = area->y;
		row->width = area->width;
		row->height = rowHeight;

		area->y += rowHeight;
		area->height -= rowHeight;

		assert(area->height >= 0);

		if (area->height == 0) {
			if (area == m_FirstUnpartitionedArea)
				m_FirstUnpartitionedArea = area->next;
			// remove from chain
			if (area->prev)
				area->prev->next = area->next;
			if (area->next)
				area->next->prev = area->prev;
			delete area;
		}

		return true;
	}

	void DynamicAtlasAllocator::buildAreas(unsigned int initialSize)
	{
		//  _______________________
		// |                       |
		// |                       |
		// |           5           |
		// |                       |
		// |                       |
		// |_______________________|
		// |           |           |
		// |     3     |           |
		// |___________|     4     |
		// |     |     |           |
		// |  1  |  2  |           |
		// |_____|_____|___________|
		// See https://github.com/Unity-Technologies/UnityCsReference/blob/89e9fc18cb70c969773bd90f338a991423da3bf3/Modules/UIElements/Renderer/UIRAtlasAllocator.cs#L293

		Area* current = m_FirstUnpartitionedArea;

		unsigned int virtualWidth = initialSize, virtualHeight = initialSize;

		while (virtualWidth < m_MaxSize || virtualHeight < m_MaxSize)
		{
			Area* newAreaNode = new Area();
			if (virtualWidth > virtualHeight)
			{
				// Double Vertically.
				newAreaNode->x = 0;
				newAreaNode->y = virtualHeight;
				newAreaNode->width = virtualWidth;
				newAreaNode->height = virtualHeight;
				virtualHeight *= 2;
			}
			else
			{
				// Double Horizontally.
				newAreaNode->x = virtualWidth;
				newAreaNode->y = 0;
				newAreaNode->width = virtualWidth;
				newAreaNode->height = virtualHeight;
				virtualWidth *= 2;
			}

			// add after current
			newAreaNode->prev = current;
			newAreaNode->next = current->next;
			current->next = newAreaNode;

			current = newAreaNode;
		}
	}

	unsigned int DynamicAtlasAllocator::getSize() const
	{
		return m_PhysicalSize;
	}
}