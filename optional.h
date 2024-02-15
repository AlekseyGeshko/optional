#include <stdexcept>
#include <utility>

// Исключение этого типа должно генерироватся при обращении к пустому optional
class BadOptionalAccess : public std::exception {
public:
    using exception::exception;

    virtual const char* what() const noexcept override {
        return "Bad optional access";
    }
};

void sayHello(){
	std::cout << "Hello!";
}

template <typename T>
class Optional {
private:
    alignas(T) unsigned char data[sizeof(T)];
    bool is_initialized_ = false;

public:
    Optional() = default;

    Optional(const T& elem) {
        new (data) T(elem);
        is_initialized_ = true;
    }

    Optional(T&& elem) {
        new (data) T(std::move(elem));
        is_initialized_ = true;
    }

    Optional(const Optional& other) {
        if (other.is_initialized_) {
            new (data) T(*other);
            is_initialized_ = true;
        }
    }

    Optional(Optional&& other) {
        if (other.is_initialized_) {
            new (data) T(std::move(*other));
            is_initialized_ = true;
        }
    }

    Optional& operator=(const T& elem) {
        if (is_initialized_) {
            **this = elem;
        } else {
            new (data) T(elem);
            is_initialized_ = true;
        }
        return *this;
    }

    Optional& operator=(T&& elem) {
        if (is_initialized_) {
            **this = std::move(elem);
        } else {
            new (data) T(std::move(elem));
            is_initialized_ = true;
        }
        return *this;
    }

    Optional& operator=(const Optional& other) {
        if (!other.is_initialized_) {
            Reset();
        } else if (is_initialized_) {
            **this = *other;
        } else {
            new (data) T(*other);
            is_initialized_ = true;
        }
        return *this;
    }

    Optional& operator=(Optional&& other) {
        if (!other.is_initialized_) {
            Reset();
        } else if (is_initialized_) {
            **this = std::move(*other);
        } else {
            new (data) T(std::move(*other));
            is_initialized_= true;
        }
        return *this;
    }

    bool HasValue() const {
        return is_initialized_;
    }

    T& operator*() {
        return *reinterpret_cast<T*>(data);
    }

    const T& operator*() const {
        return *reinterpret_cast<const T*>(data);
    }

    T* operator->() {
        return reinterpret_cast<T*>(data);
    }

    const T* operator->() const {
        return reinterpret_cast<const T*>(data);
    }

    T& Value() {
        if (!is_initialized_) {
            throw BadOptionalAccess();
        }
        return **this;
    }

    const T& Value() const {
               if (!is_initialized_) {
            throw BadOptionalAccess();
        }
        return **this;
    }

    void Reset() {
        if (is_initialized_) {
            Value().~T();
            is_initialized_ = false;
        }
    }

    ~Optional() {
        Reset();
    }
};