#pragma once

#include <vector>

template <class T>
class FreeList
{
private:

    union FreeElement
    {
        T u_Element;
        int u_NextIndex{};
    };

    std::vector<FreeElement> m_Data;

    // Index of the first empty element in the array.
    int m_FirstFree;

public:
    // Creates a new free list.
    FreeList() : m_FirstFree(-1) {};

    // Inserts an u_Element to the free list and returns an index to it.
    int Insert(const T& Element) {
        // If there are emtpy elements in this array.
        if (m_FirstFree != -1)
        {
            // Get the first empty element in array.
            const int index = m_FirstFree;

            // Set the new first empty element index to the next one.
            m_FirstFree = m_Data[m_FirstFree].u_NextIndex;

            // Set the empty element to inserted one.
            m_Data[index].u_Element = Element;

            // Return the index of the inserted element.
            return index;
        }

        // If no empty elements, FirstFree == -1, so just push new element onto array.
        else
        {
            // Create new FreeElement
            FreeElement n_FreeElement;
            // Set its it data to passed input.
            n_FreeElement.u_Element = Element;
            // Push new element onto array.
            m_Data.push_back(n_FreeElement);
            // Return index of new element.
            return static_cast<int>(m_Data.size() - 1);
        }
    };

    // Removes the Nth u_Element from the free list.
    void Erase(int N) {
        // Set element's next emtpy index to last free one.
        m_Data[N].u_NextIndex = m_FirstFree;
        // Then mark element as emtpy by setting the first emtpy element to its index.
        m_FirstFree = N;
    };

    // Removes all elements from the free list.
    void Clear() {
        // Clear array.
        m_Data.clear();

        // Set first emtpy to -1, so new elements are pushed onto array.
        m_FirstFree = -1;
    };

    // Returns the range of valid indices.
    [[nodiscard]] int Range() const {
        // Return size of array.
        return static_cast<int>(m_Data.size());
    };

    //// OPERATORS

    // Returns the Nth u_Element.
    T& operator[](int N) {
        return m_Data[N].u_Element;
    };

    // Returns the Nth u_Element.
    const T& operator[](int N) const {
        return m_Data[N].u_Element;
    };
};