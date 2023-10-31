#pragma once
#include "Box.h"
#include <iostream>
namespace rtc { struct HitData; }
namespace Util
{
    inline float Layers = 1;
    inline float InvLayers = 1;
    template<typename T>
    class BVHNode
    {
    public:
        BVHNode() {};
        BVHNode(std::vector<T>& a_List, uint32_t a_Begin, uint32_t a_End, float layer)
        {
            uint size = a_End - a_Begin;
            Layers = layer; // It stores how many layers there are in the bvh tree
            InvLayers = 1.f / Layers;
            if (size == 1)
                m_Object = a_List[a_Begin];
            else
            {
                CalculateBounds(a_List, m_Box, a_Begin, a_End);

                // Calculate the split 
                uint32_t split = CalculateSplit(a_List, a_Begin, a_End);
                m_Left = new BVHNode(a_List, a_Begin, split, layer + 1);
                m_Right = new BVHNode(a_List, split, a_End, layer + 1);
            }
        };
        void CalculateBounds(std::vector<T>& a_List, Box& a_Box, uint32_t a_Begin, uint32_t a_End)
        {
            a_List[a_Begin]->BoundingBox(a_Box); // it Is making the the main box

            // Makes the size of the bounding box
            for (uint index = a_Begin + 1; index < a_End; index++)
            {
                Util::Box newBox;
                a_List[index]->BoundingBox(newBox);
                a_Box = Expand(a_Box, newBox);
            }
        }
        uint32_t CalculateSplit(std::vector<T>& a_List, uint32_t a_Begin, uint32_t a_End)
        {
            uint32_t size = a_End - a_Begin;
            uint32_t bestSplit = a_Begin;
            uint32_t bestAxis = 0;


            for (uint32_t axis = 0; axis < 3; axis++)
            {
                // Sort on all the axis
                SortAxis(a_List, a_Begin, a_End, axis);

                Box boxLeft;
                Box boxRight;

                std::vector<float> areaLeft;
                areaLeft.resize(a_End);

                std::vector<float> areaRight;
                areaRight.resize(a_End);

                // it is comparing all the posible outcomes and storing that left, right area
                for (uint32_t indexLeft = 0; indexLeft < size; ++indexLeft)
                {
                    uint32_t indexRight = size - indexLeft - 1;
                    Util::Box newBoxL;
                    Util::Box newBoxR;

                    a_List[(uint64_t)a_Begin + (uint64_t)indexLeft]->BoundingBox(newBoxL);
                    a_List[(uint64_t)a_Begin + (uint64_t)indexRight]->BoundingBox(newBoxR);

                    // Makes it bigger each time it loops through the for loop 
                    boxLeft = Expand(newBoxL, boxLeft);
                    boxRight = Expand(newBoxR, boxRight);

                    areaLeft[(uint64_t)a_Begin + (uint64_t)indexLeft] = Area(boxLeft);
                    areaRight[(uint64_t)a_Begin + (uint64_t)indexRight] = Area(boxRight);
                }

                float bestCost = FLT_MAX;
                for (uint32_t mid = a_Begin + 1; mid < a_End; ++mid)
                {
                    // cost = A(L) * N(L) + area
                    float cost = areaLeft[mid - 1] * (float)(mid - a_Begin) + areaRight[mid] * (float)(a_End - mid);
                    if (cost < bestCost + FLT_EPSILON)
                    {
                        bestSplit = mid;
                        bestCost = cost;
                        bestAxis = axis;
                    }
                }
            }

            SortAxis(a_List, a_Begin, a_End, bestAxis);

            return bestSplit;
        };
        void SortAxis(std::vector<T>& a_List, uint32_t a_Begin, uint32_t a_End, uint32_t axis)
        {
            std::sort(a_List.begin() + a_Begin, a_List.begin() + a_End,
                [&](const T& a, const T& b)
                {
                    Box aBox;
                    Box bBox;

                    a->BoundingBox(aBox);
                    b->BoundingBox(bBox);

                    point3 aC = Center(aBox);
                    point3 bC = Center(bBox);
                    return aC[axis] < bC[axis];
                });
        }
        inline bool BoxRayIntersection(const Util::Ray& a_R) const
        {
            const Util::vec3 rayOrgin = a_R.m_Orig;
            const Util::vec3 reverseDirection = a_R.m_RDir;
            const __m128 orgin = _mm_load_ps(rayOrgin.cell), rDir = _mm_load_ps(reverseDirection.cell);
            const __m128 t1 = _mm_mul_ps(_mm_sub_ps(m_Box.m_MinM, orgin), rDir);
            const __m128 t2 = _mm_mul_ps(_mm_sub_ps(m_Box.m_MaxM, orgin), rDir);
            const __m128 vmax4 = _mm_max_ps(t1, t2), vmin4 = _mm_min_ps(t1, t2);
            float* vmax = (float*)&vmax4, * vmin = (float*)&vmin4;
            float tmax = fminf(vmax[0], fminf(vmax[1], vmax[2]));
            float tmin = fmaxf(vmin[0], fmaxf(vmin[1], vmin[2]));
            return tmax >= tmin && tmax >= 0;
        };
        bool HitTree(const Util::Ray& a_R, float a_Min, float a_Max, rtc::HitData& a_HitData)
        {
            if (m_Object != nullptr)
                return m_Object->HitObject(a_R, a_Min, a_Max, a_HitData);
            else
            {
                if (!BoxRayIntersection(a_R)) // Does a intersection check
                    return false;

                //a_HitData.depth++;

                bool hitLeft = m_Left->HitTree(a_R, a_Min, a_Max, a_HitData); // here we check if we hit the left object
                // Same goes for the right side
                // if left has been hit the we continue down the line on the left side of the tree
                bool hitRight = m_Right->HitTree(a_R, a_Min, hitLeft ? a_HitData.t : a_Max, a_HitData);

                return (hitLeft || hitRight);
            }
        }
    private:
        BVHNode* m_Left = nullptr;
        BVHNode* m_Right = nullptr;

        T m_Object = nullptr;
        Box m_Box;
    };

    template<typename T>
    class BVHTree
    {
    public:
        BVHTree() {};
        BVHTree(std::vector<T>& a_List)
        {
            uint32_t n = (uint32_t)a_List.size();

            m_List = a_List;
            m_Pool = new Box[(uint64_t)n * 2 - 1];

            CalculateBounds(m_Pool[0], 0, n);
            m_Pool[0].min.cell[3] = (float)0;
            m_Pool[0].max.cell[3] = (float)n;

            Recursion((uint32_t)0, n, 1);
        };
        void CalculateBounds(Box& a_Box, uint32_t a_Begin, uint32_t a_End)
        {
            m_List[a_Begin]->BoundingBox(a_Box); // it Is making the the main box
            // Makes the size of the bounding box
            for (uint index = a_Begin; index < a_End; index++)
            {
                Util::Box newBox;
                m_List[index]->BoundingBox(newBox);
                a_Box = Expand(a_Box, newBox);
            }
        }
        void Recursion(uint32_t a_Begin, uint32_t a_End, float a_Layer )
        {
            uint size = a_End - a_Begin;
            Layers = a_Layer; // It stores how many layers there are in the bvh tree
            if (size == 1)
            {
                m_Pool[m_PoolPtr].min.dummy= (float)a_Begin; // min.dummy = first 
                m_Pool[m_PoolPtr].max.dummy = (float)a_End; // max.dummy = last
            }
            else
            {
                // Calculate the split 
                uint32_t split = CalculateSplit(a_Begin, a_End);

                m_PoolPtr++;
                CalculateBounds(m_Pool[m_PoolPtr], a_Begin, split);
                m_Pool[m_PoolPtr].min.dummy = m_PoolPtr + 1;// min.dummy = first   
                m_Pool[m_PoolPtr].max.dummy = (float)split;// max.dummy = last

                m_PoolPtr++;
                CalculateBounds(m_Pool[m_PoolPtr], split, a_End);
                int temp = m_PoolPtr; // 2 
                m_Pool[temp].max.dummy = (float)a_End;// max.dummy = last

                Recursion(a_Begin, split, Layers + 1);

                m_Pool[temp].min.dummy = m_PoolPtr + 1;// min.dummy = first  // 2 
                Recursion(split, a_End, Layers + 1);
            }
        }
        uint32_t CalculateSplit(uint32_t a_Begin, uint32_t a_End)
        {
            uint32_t size = a_End - a_Begin;
            uint32_t bestSplit = a_Begin;
            uint32_t bestAxis = 0;

            for (uint32_t axis = 0; axis < 3; axis++)
            {
                // Sort on all the axis
                SortAxis(a_Begin, a_End, axis);

                Box boxLeft;
                Box boxRight;

                std::vector<float> areaLeft;
                areaLeft.resize(a_End);

                std::vector<float> areaRight;
                areaRight.resize(a_End);

                // it is comparing all the posible outcomes and storing that left, right area
                for (uint32_t indexLeft = 0; indexLeft < size; indexLeft++)
                {
                    uint32_t indexRight = size - indexLeft - 1;
                    Box newBoxL;
                    Box newBoxR;

                    m_List[(uint64_t)a_Begin + (uint64_t)indexLeft]->BoundingBox(newBoxL);
                    m_List[(uint64_t)a_Begin + (uint64_t)indexRight]->BoundingBox(newBoxR);

                    // Makes it bigger each time it loops through the for loop 
                    boxLeft = Expand(newBoxL, boxLeft);
                    boxRight = Expand(newBoxR, boxRight);

                    areaLeft[(uint64_t)a_Begin + (uint64_t)indexLeft] = Area(boxLeft);
                    areaRight[(uint64_t)a_Begin + (uint64_t)indexRight] = Area(boxRight);
                }

                float bestCost = FLT_MAX;
                for (uint32_t mid = a_Begin + 1; mid < a_End; ++mid)
                {
                    // cost = A(L) * N(L) + area
                    float cost = areaLeft[mid - 1] * (float)(mid - a_Begin) + areaRight[mid] * (float)(a_End - mid);
                    if (cost < bestCost + FLT_EPSILON)
                    {
                        bestSplit = mid;
                        bestCost = cost;
                        bestAxis = axis;
                    }
                }
            }

            SortAxis(a_Begin, a_End, bestAxis);

            return bestSplit;
        };
        void SortAxis(uint32_t a_Begin, uint32_t a_End, uint32_t axis)
        {
            std::sort(m_List.begin() + a_Begin, m_List.begin() + a_End,
                [&](const T& a, const T& b)
                {
                    Box aBox;
                    Box bBox;

                    a->BoundingBox(aBox);
                    b->BoundingBox(bBox);

                    point3 aC = Center(aBox);
                    point3 bC = Center(bBox);
                    return aC[axis] < bC[axis];
                });
        }
        //inline bool BoxRayIntersection(const Util::Ray& a_R, uint32_t index, float a_Min, float a_Max) const
        inline bool BoxRayIntersection(const Util::Ray& a_R, uint32_t index) const
        {
            const __m128 orgin = _mm_load_ps(a_R.m_Orig.cell), rDir = _mm_load_ps(a_R.m_RDir.cell);
            const __m128 t1 = _mm_mul_ps(_mm_sub_ps(m_Pool[index].m_MinM, orgin), rDir);
            const __m128 t2 = _mm_mul_ps(_mm_sub_ps(m_Pool[index].m_MaxM, orgin), rDir);
            const __m128 vmax4 = _mm_max_ps(t1, t2), vmin4 = _mm_min_ps(t1, t2);
            float* vmax = (float*)&vmax4, * vmin = (float*)&vmin4;
            float tmax = fminf(vmax[0], fminf(vmax[1], vmax[2]));
            float tmin = fmaxf(vmin[0], fmaxf(vmin[1], vmin[2]));
            return tmax >= tmin && tmax >= 0;
        };

        bool HitTree(const Util::Ray& a_R, float a_Min, float a_Max, rtc::HitData& a_HitData, uint32_t index)
        {
            if (m_Pool[index].max.dummy - m_Pool[index].min.dummy == 1) // one 
                    return m_List[(uint64_t)m_Pool[index].min.dummy]->HitObject(a_R, a_Min, a_Max, a_HitData);
            else
            {
                if (!BoxRayIntersection(a_R, index)) // Does a intersection check
                    return false;

                //a_HitData.depth++;
                index++;
                bool hitLeft = HitTree(a_R, a_Min, a_Max, a_HitData, m_Pool[index].min.dummy); // here we check if we hit the left object
                // Same goes for the right side
                // if left has been hit the we continue down the line on the left side of the tree
                 index++;
                bool hitRight = HitTree(a_R, a_Min, hitLeft ? a_HitData.t : a_Max, a_HitData, m_Pool[index].min.dummy);

                return (hitLeft | hitRight);
            }
        }
    private:
        Box* m_Pool;
        int m_PoolPtr = 0;
        std::vector<T> m_List;
    };
}