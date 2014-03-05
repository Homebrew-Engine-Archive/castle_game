#ifndef PERSIST_VALUE
#define PERSIST_VALUE

#include <stack>

template<class T>
class PersistValue
{
    std::stack<T> values;
public:
    PersistValue(const T &value) {
        values.push(value);
    }
    PersistValue(const PersistValue &that) {
        if(that.HasValue()) {
            values.push(that.Get());
        }
    }
    inline bool HasValue() const {
        return !values.empty();
    }
    inline T& Get() {
        return values.top();
    }
    inline void Push(const T &value) {
        values.push(value);
    }
    inline void Recover() {
        values.pop();
    }
    inline operator T() const {
        return Get();
    }
    PersistValue<T> &operator=(const PersistValue<T> &that) {
        values.clear();
        if(that.HasValue()) {
            values.push(that.Get());
        }
        return *this;
    }
    PersistValue<T> &operator=(const T &val) {
        values.clear();
        values.push(val);
        return *this;
    }
    inline void Clear() {
        values.clear();
    }
};

#endif
