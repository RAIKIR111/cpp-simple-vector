#pragma once

#include <cassert>
#include <initializer_list>
#include <stdio.h>
#include <algorithm>
#include <stdexcept>

#include "array_ptr.h"

class ReserveProxyObj {
public:
    ReserveProxyObj(size_t capacity) : reserve_capacity_(capacity) {}

    size_t GetReserveCapacity() const {
        return reserve_capacity_;
    }
private:
    size_t reserve_capacity_ = 0;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size) : items_(size), size_(size), capacity_(size)  {
        // Напишите тело конструктора самостоятельно
        std::generate(begin(), end(), []() {return Type{};});
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value) : items_(size), size_(size), capacity_(size) {
        // Напишите тело конструктора самостоятельно
        std::generate(begin(), end(), [value]() {return Type{value};});
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init) : items_(init.size()), size_(init.size()), capacity_(init.size()) {
        // Напишите тело конструктора самостоятельно
        size_t counter = 0;
        for (const Type& item : init) {
            items_[counter++] = item;
        }
    }

    SimpleVector(const SimpleVector& other) : items_(other.capacity_), size_(other.size_), capacity_(other.capacity_) {
        std::copy(other.begin(), other.end(), begin());
    }

    SimpleVector(SimpleVector&& other) : items_(std::move(other.items_)), size_(other.size_), capacity_(other.capacity_) {
        other.size_ = 0;
        other.capacity_ = 0;
    }

    SimpleVector(const ReserveProxyObj& reserveObj) {
        Reserve(reserveObj.GetReserveCapacity());
    }

    SimpleVector& operator=(const SimpleVector& rhs) {
        if (this == &rhs) return *this;

        SimpleVector obj(rhs);

        swap(obj);
        return *this;
    }

    SimpleVector& operator=(SimpleVector&& rhs) {
        if (this != &rhs) {
            SimpleVector temp(std::move(rhs));
            swap(temp);
        }
        return *this;
    }

    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(const Type& item) {
        // Напишите тело самостоятельно
        if (size_ < capacity_) items_[size_++] = item;
        else {
            size_t new_capacity = (capacity_ == 0) ? 1 : capacity_ * 2;
            ArrayPtr<Type> new_array(new_capacity);

            std::copy(begin(), end(), new_array.Get());

            new_array[size_++] = item;

            items_.swap(new_array);
            capacity_ = new_capacity;
        }
    }

    void PushBack(Type&& item) {
        if (size_ < capacity_) items_[size_++] = std::move(item);
        else {
            size_t new_capacity = (capacity_ == 0) ? 1 : capacity_ * 2;
            ArrayPtr<Type> new_array(new_capacity);

            std::move(begin(), end(), new_array.Get());

            new_array[size_++] = std::move(item);

            items_.swap(new_array);
            capacity_ = new_capacity;
        }
    }
    
    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, const Type& value) {
        assert(pos >= begin() && pos <= end());
        // Напишите тело самостоятельно
        if (capacity_ == 0) capacity_ = 1;
        else if (capacity_ == size_) capacity_ *= 2;

        const size_t pos_copy = pos - begin();
        ArrayPtr<Type> new_array(capacity_);
        
        std::copy(begin(), begin() + pos_copy, new_array.Get());
        std::copy_backward(begin() + pos_copy, end(), new_array.Get() + size_ + 1);
        ++size_;

        new_array[pos_copy] = value;

        items_.swap(new_array);

        return begin() + pos_copy;
    }

    Iterator Insert(ConstIterator pos, Type&& value) {
        assert(pos - begin() <= end() - begin());

        if (capacity_ == 0) capacity_ = 1;
        else if (capacity_ == size_) capacity_ *= 2;

        const size_t pos_copy = pos - begin();
        ArrayPtr<Type> new_array(capacity_);

        std::move(begin(), begin() + pos_copy, new_array.Get());
        std::move_backward(begin() + pos_copy, end(), new_array.Get() + size_ + 1);
        ++size_;

        new_array[pos_copy] = std::move(value);

        items_.swap(new_array);

        return begin() + pos_copy;
    }
    
    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        // Напишите тело самостоятельно
        assert(size_ != 0);
        --size_;
    }
    
    // Обменивает значение с другим вектором
    void swap(SimpleVector& other) noexcept {
        // Напишите тело самостоятельно
        items_.swap(other.items_);
        std::swap(other.capacity_, this->capacity_);
        std::swap(other.size_, this->size_);
    }
    
    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator poss) {
        // Напишите тело самостоятельно
        auto pos = const_cast<Type*>(poss);

        assert(pos >= begin() && pos < end());

        size_t pos_copy = pos - begin();
        const size_t const_pos_copy = pos_copy;
        for (auto it = pos + 1; it != end(); ++it) {
            At(pos_copy) = std::move(*it);
            ++pos_copy;
        }

        --size_;
        return begin() + const_pos_copy;
    }

    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
        // Напишите тело самостоятельно
        return size_;
    }

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
        // Напишите тело самостоятельно
        return capacity_;
    }

    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        if (size_ == 0) return true;
        return false;
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {
            ArrayPtr<Type> new_array(new_capacity);
            std::move(begin(), end(), new_array.Get());
            items_.swap(new_array);
            capacity_ = new_capacity;
        }
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
        assert(index < size_);
        // Напишите тело самостоятельно
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        assert(index < size_);
        // Напишите тело самостоятельно
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        // Напишите тело самостоятельно
        if (index >= size_) throw std::out_of_range("Index is greater than size_!");
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        // Напишите тело самостоятельно
        if (index >= size_) throw std::out_of_range("Index is greater than size_!");
        return const_cast<Type&>(items_[index]);
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        // Напишите тело самостоятельно
        size_ = 0;
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
        // Напишите тело самостоятельно
        if (new_size <= size_) {
            size_ = new_size;
            return;
        }
        
        if (new_size <= capacity_) {
            for (auto i = size_; i < new_size; i++) {
                items_[i] = std::move(Type{});
            }
            size_ = new_size;
            return;
        }

        if (new_size > capacity_) {
            ArrayPtr<Type> new_arr(new_size);
            std::move(begin(), end(), new_arr.Get());

            for (auto i = size_; i < new_size; i++) {
                new_arr[i] = std::move(Type{});
            }

            new_arr.swap(items_);

            size_ = new_size;
            capacity_ = new_size;
        }

        /*ArrayPtr<Type> new_arr(new_size);
        auto it = std::move(begin(), end(), new_arr.Get());
        const auto default_value = Type();
        std::fill(it, new_arr.Get() + new_size, std::move(Type{}));

        new_arr.swap(items_);

        size_ = new_size;
        capacity_ = new_size;*/
    }

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        // Напишите тело самостоятельно
        return (size_ != 0) ? &(items_[0]) : nullptr;
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        // Напишите тело самостоятельно
        return (size_ != 0) ? &(items_[size_]) : nullptr;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        // Напишите тело самостоятельно
        return (size_ != 0) ? &(items_[0]) : nullptr;
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        // Напишите тело самостоятельно
        return (size_ != 0) ? &(items_[size_]) : nullptr;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        // Напишите тело самостоятельно
        return (size_ != 0) ? &(items_[0]) : nullptr;
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        // Напишите тело самостоятельно
        return (size_ != 0) ? &(items_[size_]) : nullptr;
    }

private:
    ArrayPtr<Type> items_;

    size_t size_ = 0;
    size_t capacity_ = 0;
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs) && !(rhs < lhs);
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(!(lhs < rhs) && !(rhs < lhs));
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return (lhs < rhs) || (!(lhs < rhs) && !(rhs < lhs));
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return (rhs < lhs) || (!(lhs < rhs) && !(rhs < lhs));
} 