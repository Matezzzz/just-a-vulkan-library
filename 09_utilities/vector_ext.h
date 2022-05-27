#ifndef VECTOR_EXT_H
#define VECTOR_EXT_H

#include <vector>

using std::vector;


template<typename T>
class vector_ext : public vector<T>{
public:
    using vector<T>::vector;

    /**
     * @brief Create a new vector from converted elements of the current one
     * 
     * @tparam T_dst type to convert to
     * @tparam Args additional arguments required for conversion. Global for all objects
     * @param conversion_func function converting vector type -> T_dst
     * @param args additional argument values
     */
    template<typename T_dst, typename... Args>
    vector_ext<T_dst> convert(T_dst (*conversion_func)(const T&, Args&...), Args... args) const{
        vector_ext<T_dst> x;
        x.reserve(this->size());
        for (const T& a : (*this)){
            x.push_back(conversion_func(a, args...));
        }
        return x;
    }
    //add given element to the vector and return a reference to the vector
    vector_ext<T>& add(const T& t){
        this->push_back(t);
        return *this;
    }
    
    //add given element to the vector and return a copy of the vector with the new element
    vector_ext<T> add(const T& t) const{
        vector_ext<T> a = *this;
        return a.add(t);
    }

    //concatenate two vectors - add all values to the end, then return a reference to the vector with added values
    vector_ext<T>& add(const vector_ext<T>& t){
        this->insert(this->end(), t.begin(), t.end());
        return *this;
    }

    //concatenate two vectors - add all values to the end, then return a new vector with all values
    vector_ext<T> add(const vector_ext<T>& t) const{
        vector_ext<T> a = *this;
        return a.add(t);
    }

    //add a given value if the condition is true
    vector_ext<T>& addIf(bool cond, const T& t){
        if (cond) this->push_back(t);
        return *this;
    }
};

#endif