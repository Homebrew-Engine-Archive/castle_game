#ifndef TWIN_H_
#define TWIN_H_

namespace core
{
    // Twin<T> = std::pair<T, T>;

    template<class T>
    class Twin
    {
        T first;
        T second;
    public:
        typedef T value_type;
        
        constexpr Twin(value_type lhs, value_type rhs);

        constexpr value_type First() const;
        constexpr value_type Second() const;

        void SetFirst(value_type value);
        void SetSecond(value_type value);
    };

    template<class T>
    constexpr Twin<T>::Twin(value_type lhs, value_type rhs)
        : first(std::move(lhs))
        , second(std::move(rhs))
    {}

    template<class T>
    constexpr Twin<T>::value_type Twin<T>::First() const
    {
        return first;
    }

    template<class T>
    constexpr Twin<T>::value_type Twin<T>::Second() const
    {
        return second;
    }

    template<class T>
    void Twin<T>::SetFirst(value_type value)
    {
        first = value;
    }
    
    template<class T>
    void Twin<T>::SetSecond(value_type value)
    {
        second = value;
    }
}

#endif // TWIN_H_
