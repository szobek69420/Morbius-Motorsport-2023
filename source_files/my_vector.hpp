#ifndef MY_VECTOR_HPP
#define MY_VECTOR_HPP

#include <utility>

namespace gigachad {

    template <typename T>
    class vector {
    public:

        class iterator;
        class const_iterator;
        class reverse_iterator;
        class const_reverse_iterator;


        class iterator {
            friend const_iterator;

        private:
            T* pointer;

        public:
            iterator(T* pData) : pointer(pData) {}
            ~iterator() {}

            T& operator*() { return (*pointer); }
            iterator operator++(int dummy) { iterator old = *this; pointer++; return old; }
            iterator& operator++() { pointer++; return *this; }
            iterator operator--(int dummy) { iterator old = *this; pointer--; return old; }
            iterator& operator--() { pointer--; return *this; }

            bool operator==(const iterator& masik) { return (pointer == masik.pointer); }
            bool operator!=(const iterator& masik) { return !(pointer == masik.pointer); }
            bool operator<(const iterator& masik) { return (pointer < masik.pointer); }
            bool operator<=(const iterator& masik) { return (pointer <= masik.pointer); }
            bool operator>(const iterator& masik) { return (pointer > masik.pointer); }
            bool operator>=(const iterator& masik) { return (pointer >= masik.pointer); }

            iterator& operator+(int szam) { pointer += szam; return *this; }
            iterator& operator-(int szam) { pointer -= szam; return *this; }

            friend int operator-(const iterator& it1, const iterator& it2) { return (it1.pointer - it2.pointer); }
        };

        class const_iterator {
        private:
            const T* pointer;
        public:
            const_iterator(const T* pData) : pointer(pData) {}
            const_iterator(const iterator& masik) { pointer = masik.pointer; }
            ~const_iterator() {}

            const T& operator*() { return (*pointer); }
            const_iterator operator++(int dummy) { const_iterator old = *this; pointer++; return old; }
            const_iterator& operator++() { pointer++; return *this; }
            const_iterator operator--(int dummy) { const_iterator old = *this; pointer--; return old; }
            const_iterator& operator--() { pointer--; return *this; }

            bool operator==(const const_iterator& masik) { return (pointer == masik.pointer); }
            bool operator!=(const const_iterator& masik) { return !(pointer == masik.pointer); }
            bool operator<(const const_iterator& masik) { return (pointer < masik.pointer); }
            bool operator<=(const const_iterator& masik) { return (pointer <= masik.pointer); }
            bool operator>(const const_iterator& masik) { return (pointer > masik.pointer); }
            bool operator>=(const const_iterator& masik) { return (pointer >= masik.pointer); }

            const_iterator& operator+(int szam) { pointer += szam; return *this; }
            const_iterator& operator-(int szam) { pointer -= szam; return *this; }

            friend int operator-(const const_iterator& it1, const const_iterator& it2) { return (it1.pointer - it2.pointer); }
        };

        class reverse_iterator {
            friend const_reverse_iterator;

        private:
            T* pointer;

        public:
            reverse_iterator(T* pData) : pointer(pData) {}
            ~reverse_iterator() {}

            T& operator*() { return (*pointer); }
            reverse_iterator operator++(int dummy) { reverse_iterator old = *this; pointer--; return old; }
            reverse_iterator& operator++() { pointer--; return *this; }
            reverse_iterator operator--(int dummy) { reverse_iterator old = *this; pointer++; return old; }
            reverse_iterator& operator--() { pointer++; return *this; }

            bool operator==(const reverse_iterator& masik) { return (pointer == masik.pointer); }
            bool operator!=(const reverse_iterator& masik) { return !(pointer == masik.pointer); }
            bool operator<(const reverse_iterator& masik) { return (pointer > masik.pointer); }
            bool operator<=(const reverse_iterator& masik) { return (pointer >= masik.pointer); }
            bool operator>(const reverse_iterator& masik) { return (pointer < masik.pointer); }
            bool operator>=(const reverse_iterator& masik) { return (pointer <= masik.pointer); }

            reverse_iterator& operator+(int szam) { pointer -= szam; return *this; }
            reverse_iterator& operator-(int szam) { pointer += szam; return *this; }

            friend int operator-(const reverse_iterator& it1, const reverse_iterator& it2) { return (it2.pointer - it1.pointer); }
        };

        class const_reverse_iterator {
        private:
            const T* pointer;
        public:
            const_reverse_iterator(const T* pData) : pointer(pData) {}
            const_reverse_iterator(const reverse_iterator& masik) { pointer = masik.pointer; }
            ~const_reverse_iterator() {}

            const T& operator*() { return (*pointer); }
            const_reverse_iterator operator++(int dummy) { const_reverse_iterator old = *this; pointer--; return old; }
            const_reverse_iterator& operator++() { pointer--; return *this; }
            const_reverse_iterator operator--(int dummy) { const_reverse_iterator old = *this; pointer++; return old; }
            const_reverse_iterator& operator--() { pointer++; return *this; }

            bool operator==(const const_reverse_iterator& masik) { return (pointer == masik.pointer); }
            bool operator!=(const const_reverse_iterator& masik) { return !(pointer == masik.pointer); }
            bool operator<(const const_reverse_iterator& masik) { return (pointer > masik.pointer); }
            bool operator<=(const const_reverse_iterator& masik) { return (pointer >= masik.pointer); }
            bool operator>(const const_reverse_iterator& masik) { return (pointer < masik.pointer); }
            bool operator>=(const const_reverse_iterator& masik) { return (pointer <= masik.pointer); }

            const_reverse_iterator& operator+(int szam) { pointer -= szam; return *this; }
            const_reverse_iterator& operator-(int szam) { pointer += szam; return *this; }

            friend int operator-(const const_reverse_iterator& it1, const const_reverse_iterator& it2) { return (it2.pointer - it1.pointer); }
        };

    private:
        T* pArray;
        size_t length_;
        size_t capacity_;

        static const size_t RESIZE_MULTIPLIER = 2;

    public:
        vector(unsigned int start_size = 0);
        vector(const vector& otter);
        ~vector();

        size_t size()const { return length_; }
        size_t capacity() const { return capacity_; }
        bool empty() const { if (length_ == 0) return true; else return false; }

        T& at(size_t index);
        const T& at(size_t index) const;

        void resize(size_t _size, T fill = T());//allocates memory and initializes elements (affects capacity() and size())
        void reserve(size_t _size);//allocates memory and leaves it uninitialized (affects only capacity())
        void clear();
        void shrink_to_fit();

        void push_back(const T& _element);
        template <typename... Args>
        void emplace_back(Args&&... args);
        void pop_back();

        iterator erase(const_iterator _where);
        iterator erase(const_iterator _first, const_iterator _last);
        iterator insert(const_iterator _where, const T& _element);
        iterator insert(const_iterator _where, const_iterator _first, const_iterator _last);

        //iteration
        T& front() { return pArray[0]; }
        const T& front() const { return pArray[0]; }
        T& back() { return pArray[length_ - 1]; }
        const T& back() const { return pArray[length_ - 1]; }

        iterator begin() { iterator it(pArray); return it; }
        const_iterator begin() const { const_iterator it(pArray); return it; }
        iterator end() { iterator it(pArray + length_); return it; }
        const_iterator end() const { const_iterator it(pArray + length_); return it; }

        reverse_iterator rbegin() { reverse_iterator it(pArray + length_ - 1); return it; }
        const_reverse_iterator rbegin() const { const_reverse_iterator it(pArray + length_ - 1); return it; }
        reverse_iterator rend() { reverse_iterator it(pArray - 1); return it; }
        const_reverse_iterator rend() const { const_reverse_iterator it(pArray - 1); return it; }

        const T* data() const { return pArray; }

        //operator overloads
        vector& operator=(const vector& otter);

        T& operator[](size_t index);
        const T& operator[](size_t index) const;
    };


    //vector

    template <typename T>
    vector<T>::vector(unsigned int start_size)
    {
        pArray = 0;
        length_ = 0;
        capacity_ = 0;
    }

    template <typename T>
    vector<T>::vector(const vector<T>& otter)
    {
        length_ = otter.length_;
        capacity_ = otter.capacity_;

        pArray = new T[capacity_];

        for (unsigned int i = 0; i < length_; i++)
            pArray[i] = otter.pArray[i];
    }

    template <typename T>
    vector<T>::~vector()
    {
        if (capacity_ != 0)
            delete[] pArray;
        pArray = 0;
        length_ = 0;
        capacity_ = 0;
    }

    template <typename T>
    T& vector<T>::at(size_t index)
    {
        if (index >= length_)
            exit(-1);

        return pArray[index];
    }

    template <typename T>
    const T& vector<T>::at(size_t index) const
    {
        if (index >= length_)
            exit(-1);

        return pArray[index];
    }


    template <typename T>
    void vector<T>::resize(size_t _size, T fill)
    {
        size_t oldSize = capacity_;

        this->reserve(_size);

        if (oldSize < _size)
        {
            for (size_t i = oldSize; i < _size; i++)
                pArray[i] = fill;

            length_ = _size;
        }
    }

    template <typename T>
    void vector<T>::reserve(size_t _size)
    {
        T* oldArr = pArray;

        if (_size == 0)
        {
            if (capacity_ != 0)
                delete[] oldArr;

            pArray = 0;
            length_ = 0;
            capacity_ = 0;
        }
        else if (_size <= length_)
        {
            pArray = new T[_size];

            for (size_t i = 0; i < _size; i++)
                pArray[i] = oldArr[i];

            delete[] oldArr;
            length_ = _size;
            capacity_ = _size;
        }
        else //if (_size > length_)
        {
            pArray = new T[_size];

            for (size_t i = 0; i < length_; i++)
                pArray[i] = oldArr[i];

            if (capacity_ != 0)
                delete[] oldArr;

            capacity_ = _size;
        }
    }


    template <typename T>
    void vector<T>::clear()
    {
        this->resize(0);
    }

    template<typename T>
    void vector<T>::shrink_to_fit()
    {
        this->resize(this->length_);
    }

    template <typename T>
    void vector<T>::push_back(const T& element)
    {
        if (length_ == capacity_)//novelni kell a meretet
        {
            if (length_ == 0)
                this->reserve(1);
            else
                this->reserve(capacity_ * RESIZE_MULTIPLIER);
        }

        pArray[length_] = T(element);
        length_++;
    }

    template <typename T>
    template <typename... Args>
    void vector<T>::emplace_back(Args&&... args) {
        if (length_ == capacity_)//novelni kell a meretet
        {
            if (length_ == 0)
                this->reserve(1);
            else
                this->reserve(capacity_ * RESIZE_MULTIPLIER);
        }

        new (&pArray[length_]) T(std::forward<Args>(args)...); // construct the new element in place
        length_++; // increment the size of the vector
    }

    template <typename T>
    void vector<T>::pop_back()
    {
        erase(begin() + length_ - 1);
        return;
    }


    template <typename T>
    typename vector<T>::iterator vector<T>::erase(vector<T>::const_iterator _where)
    {
        int _index = _where - this->begin();
        if (_index < 0)//tomb elol akar torolni
            exit(-1);
        else if (_index >= length_)//tomb mogul akar torolni
            exit(-1);

        if (capacity_ == 1)
        {
            this->reserve(0);
            return this->end();
        }

        if ((size_t)(capacity_ / RESIZE_MULTIPLIER) >= (length_ - 1))
        {
            capacity_ = (size_t)(capacity_ / RESIZE_MULTIPLIER);
        }

        T* oldArr = pArray;
        pArray = new T[capacity_];

        for (size_t i = 0; i < _index; i++)
            pArray[i] = oldArr[i];
        for (size_t i = _index + 1; i < length_; i++)
            pArray[i - 1] = oldArr[i];

        delete[] oldArr;

        length_--;
        return this->begin() + _index;
    }

    template <typename T>
    typename vector<T>::iterator vector<T>::erase(vector<T>::const_iterator _first, vector<T>::const_iterator _last)
    {
        int _index1 = _first - this->begin(), _index2 = _last - this->begin();

        if (_index1 >= _index2)
            return this->begin() + _index1;

        if (_index1 < 0 || _index2 < 0)
            exit(-1);
        else if (_index1 >= length_ || _index2 > length_)
            exit(-1);

        size_t _deleted = _index2 - _index1;
        size_t _newLength = length_ - _deleted;

        if (_newLength == 0)
        {
            this->reserve(0);
            return this->end();
        }
        else
        {
            while (capacity_ > _newLength* RESIZE_MULTIPLIER)//addig csokkenti a kapacitast, hogy a torles utan is normalis foglalasa legyen a vektornak
                capacity_ /= RESIZE_MULTIPLIER;

            T* oldArr = pArray;
            pArray = new T[capacity_];
            size_t pArrayIndex = 0;

            for (size_t i = 0; i < _index1; i++)
                pArray[pArrayIndex++] = oldArr[i];
            for (size_t i = _index2; i < length_; i++)
                pArray[pArrayIndex++] = oldArr[i];

            length_ = _newLength;

            delete[] oldArr;

            return this->begin() + _index1;
        }
    }

    template <typename T>
    typename vector<T>::iterator vector<T>::insert(vector<T>::const_iterator _where, const T& _element)
    {
        length_++;
        if (length_ > capacity_)
            capacity_ = capacity_ == 0 ? 1 : capacity_ * RESIZE_MULTIPLIER;

        int _index = _where - this->begin();
        if (_index < 0)//tomb ele akar beszurni
            exit(-1);
        else if (_index > this->size())//tomb moge akar beszurni
            exit(-1);

        T* oldArr = pArray;
        pArray = new T[capacity_];

        for (size_t i = 0; i < _index; i++)
            pArray[i] = oldArr[i];
        pArray[_index] = _element;
        for (size_t i = _index + 1; i < capacity_; i++)
            pArray[i] = oldArr[i - 1];

        if (capacity_ > 1)
            delete[] oldArr;

        return this->begin() + _index;
    }

    template <typename T>
    typename vector<T>::iterator vector<T>::insert(vector<T>::const_iterator _where, vector<T>::const_iterator _first, vector<T>::const_iterator _last)
    {
        //checking _where
        int _index = _where - this->begin();
        if (_index < 0)//tomb ele akar beszurni
            exit(-1);
        else if (_index > this->size())//tomb moge akar beszurni
            exit(-1);

        //checking _first and _last
        int _inserted = _last - _first;
        if (_inserted <= 0)
            return this->begin() + _index;

        //changing capacity_
        length_ += _inserted;
        while (length_ > capacity_)
            capacity_ = capacity_ == 0 ? 1 : capacity_ * RESIZE_MULTIPLIER;

        T* oldArr = pArray;
        pArray = new T[capacity_];

        for (size_t i = 0; i < _index; i++)//old elements before _where
            pArray[i] = oldArr[i];
        for (size_t i = _index; _first != _last; _first++)//new elements
            pArray[i++] = *_first;
        for (size_t i = _index + _inserted; i < capacity_; i++)//old elements after _where
            pArray[i] = oldArr[i - _inserted];

        if (capacity_ > 1)
            delete[] oldArr;

        return this->begin() + _index;
    }


    //operator overloads

    template <typename T>
    vector<T>& vector<T>::operator=(const vector<T>& otter)
    {
        if (this == &otter)
            return (*this);

        if (capacity_ != 0)
            delete[] pArray;

        length_ = otter.length_;
        capacity_ = otter.capacity_;

        pArray = new T[capacity_];

        for (size_t i = 0; i < length_; i++)
            pArray[i] = otter.pArray[i];

        return (*this);
    }


    template <typename T>
    T& vector<T>::operator[](size_t index)
    {
        return at(index);
    }

    template <typename T>
    const T& vector<T>::operator[](size_t index) const
    {
        return at(index);
    }
}


#endif // MY_VECTOR_HPP

