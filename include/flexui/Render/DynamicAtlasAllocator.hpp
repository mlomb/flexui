#pragma once

#include <vector>

namespace flexui {

    /**
        @brief A rect allocator for dynamic atlases
        Note that this class does not hold any kind of imagery.
        This class allocates rects filling POT areas, growing
        to the next POT.
        This implementation is based on Unity's UIR atlas allocator
        https://github.com/Unity-Technologies/UnityCsReference/blob/2019.3/Modules/UIElements/Renderer/UIRAtlasAllocator.cs
    */
    class DynamicAtlasAllocator {
    public:
        DynamicAtlasAllocator(unsigned int initialSize = 64, unsigned int maxSize = 4096);
        ~DynamicAtlasAllocator();

        bool tryAllocate(unsigned int width, unsigned int height, unsigned int& locationX, unsigned int& locationY);
        unsigned int getSize() const;

    private:
        struct Row {
            unsigned int width, height;
            unsigned int offsetX, offsetY;
            unsigned int cursor;
        };
        struct Area {
            unsigned int x, y, width, height;
            Area* prev = nullptr, * next = nullptr;
        };

        unsigned int m_MaxSize;
        unsigned int m_PhysicalSize;

        void buildAreas(unsigned int initialSize);
        bool tryPartitionArea(Area* area, unsigned int rowIndex, unsigned int rowHeight, unsigned int minWidth);

        std::vector<Row*> m_OpenRows;
        Area* m_FirstUnpartitionedArea;
    };

}