//
// Created by misovicpavle on 9/9/26.
//

#ifndef COMPILER_CONTAINERS_H
#define COMPILER_CONTAINERS_H
#include <cstddef> //for size_t

template <typename T, size_t N>
class StaticArray {
    private:
        T m_data[N];
    public:
        constexpr size_t size() const { return N; }

        [[nodiscard]] const T& operator[](size_t i) const { return m_data[i]; }
        [[nodiscard]] T& operator[](size_t i) { return m_data[i]; }

        const T* data() const { return (T*)m_data; }
        T* data() { return (T*)m_data; }

        const T* begin() const { return (T*)m_data[0]; }
        T* begin() { return (T*)m_data[0]; }

        const T* end() const { return (T*)m_data[N-1]; }
        T* end() { return (T*)m_data[N-1]; }
};


//NO HEAP YET - STACK VECTOR IMPLEMENTATION
template <typename T, size_t N>
class FixedVector {
    private:
        T m_data[N];
        size_t m_size {0};
    public:
        /*
         0 1 2 3 4 ... memory index
         a b c - - ... stored values
               |=> end() = m_data(m_data[0]) + 3(m_size)
         */
        constexpr size_t size() const { return m_size; }
        constexpr size_t capacity() const { return N; }
        constexpr bool empty() const { return m_size == 0; }

        [[nodiscard]] const T& operator[](size_t i) const { return m_data[i]; }
        [[nodiscard]] T& operator[](size_t i) { return m_data[i]; }

        const T* begin() const { return m_data; }
        T* begin() { return m_data; }

        const T* end() const { return m_data + m_size; }
        T* end() { return m_data + m_size; }

        // Modifiers:
        bool push_back(const T& value) {
            if (m_size >= N) { return false; }
            if (m_size < N) { m_data[m_size++] = value; }
            return true;
        }

        bool pop_back() {
            if (m_size == 0) { return false; }
            --m_size;
            return true;
        }

        bool insert(size_t index, const T& value) {
            if (index > m_size || m_size >= N) { return false; }
            ++m_size;
            //shift all elements to the right
            for (size_t i = m_size; i > index; --i) { m_data[i] = m_data[i-1]; }
            m_data[index] = value;
        }

        bool erase(size_t index) {
            if (index >= m_size) { return false; }
            //shift all elements to the left
            for (size_t i = index; i < m_size; ++i) { m_data[i] = m_data[i+1]; }
            --m_size;
            return true;
        }

        void clear() { m_size = 0; }
};



#include <cstring> // FIX: needed for strlen

class StringView {
    private:
        const char* m_data;
        size_t m_size;
    public:
        StringView() : m_data(nullptr), m_size(0) {}
        StringView(const char* ptr, size_t size) : m_data(ptr), m_size(size) {} // FIX
        StringView(const char* str) {
            if (str) {
                m_data = str;
                m_size = strlen(str);
            } else {
                m_data = nullptr;
                m_size = 0;
            }
        }

        //member access const/non-const
        [[nodiscard]] const char& operator[](size_t index) const { return m_data[index]; } // FIX

        //member access begin/end const/non-const
        const char* begin() const { return m_data; }
        const char* end() const { return m_data + m_size; }

        //assigment operator (non-const)
        StringView& operator=(const char* str) { // FIX: no [[nodiscard]], non-const return
            m_data = str;
            m_size = str ? strlen(str) : 0; // FIX: strlen(nullptr) is UB
            return *this;
        }

        //member access size/empty
        [[nodiscard]] size_t size() const { return m_size; } // FIX: removed const on return
        [[nodiscard]] bool empty() const { return m_size == 0; } // FIX

        //comparison operators
        [[nodiscard]] bool operator==(const StringView& other) const { // FIX
            if (m_size != other.m_size) { return false; }
            for (size_t i = 0; i < m_size; ++i) {
                if (m_data[i] != other.m_data[i]) { return false; }
            }
            return true;
        }

        [[nodiscard]] bool operator!=(const StringView& other) const {
            return !(*this == other);
        }

        [[nodiscard]] bool operator>(const StringView& other) const { // FIX
            size_t min_size = (m_size < other.m_size) ? m_size : other.m_size;
            for (size_t i = 0; i < min_size; ++i) {
                if (m_data[i] > other.m_data[i]) { return true; }
                if (m_data[i] < other.m_data[i]) { return false; }
            }
            return m_size > other.m_size;
        }

        [[nodiscard]] bool operator<(const StringView& other) const { // FIX
            size_t min_size = (m_size < other.m_size) ? m_size : other.m_size;
            for (size_t i = 0; i < min_size; ++i) {
                if (m_data[i] < other.m_data[i]) { return true; }
                if (m_data[i] > other.m_data[i]) { return false; }
            }
            return m_size < other.m_size;
        }

        [[nodiscard]] bool operator>=(const StringView& other) const { return !(*this < other); } // FIX
        [[nodiscard]] bool operator<=(const StringView& other) const { return !(*this > other); } // FIX

        // FIX: removed custom move assignment (see below)

        //check front/back
        [[nodiscard]] const char& front() const { return m_data[0]; } // FIX
        [[nodiscard]] const char& back() const { return m_data[m_size - 1]; } // FIX
};
#endif //COMPILER_CONTAINERS_H
