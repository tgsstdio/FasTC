/*******************************************************************************
 * Copyright (c) 2012 Pavel Krajcevski
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 *    1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 *    2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 *    3. This notice may not be removed or altered from any source
 *    distribution.
 *
 ******************************************************************************/

#ifndef BASE_INCLUDE_VECTORBASE_H_
#define BASE_INCLUDE_VECTORBASE_H_

// !FIXME! For sqrt function. This increases compilation time by a LOT 
// but I couldn't guarantee any faster general-purpose implementation
#include <cmath>

namespace FasTC {

  template <typename T, const int N>
  class VectorBase {
   protected:

    // Vector representation
    T vec[N];

   public:
    typedef T ScalarType;
    
    VectorBase() { }
    VectorBase(const VectorBase<T, N> &other) {
      for(int i = 0; i < N; i++) vec[i] = other[i];
    }

    explicit VectorBase(const T *_vec) {
      for(int i = 0; i < N; i++) {
        vec[i] = _vec[i];
      }
    }

    static const int Size = N;

    // Accessors
    T &operator()(int idx) { return vec[idx]; }
    T &operator[](int idx) { return vec[idx]; }
    const T &operator()(int idx) const { return vec[idx]; }
    const T &operator[](int idx) const { return vec[idx]; }

    // Allow casts to the respective array representation...
    operator const T *() const { return vec; }
    VectorBase<T, N> &operator=(const T *v) {
      for(int i = 0; i < N; i++)
        vec[i] = v[i];
      return *this;
    }

    // Allows casting to other vector types if the underlying type system does as well...
    template<typename _T>
    operator VectorBase<_T, N>() const { 
      VectorBase<_T, N> ret;
      for(int i = 0; i < N; i++) {
        ret[i] = static_cast<_T>(vec[i]);
      }
      return ret;
    }

    // Vector operations
    template<typename _T>
    T Dot(const VectorBase<_T, N> &v) const {
      T sum = 0;
      for(int i = 0; i < N; i++)
        sum += vec[i] * v[i];
      return sum;
    }

    T LengthSq() const { return this->Dot(*this); }
    T Length() const { return sqrt(LengthSq()); }

    void Normalize() {
      T len = Length();
      for(int i = 0; i < N; i++) {
        vec[i] /= len;
      }
    }
  };

  // Operators
  template<typename VectorTypeOne, typename VectorTypeTwo>
  static inline VectorTypeOne VectorAddition(const VectorTypeOne &v1,
                                             const VectorTypeTwo &v2) {
    VectorTypeOne a(v1);
    for(int i = 0; i < VectorTypeOne::Size; i++) {
      a(i) += v2(i);
    }
    return a;
  }

  template<typename VectorTypeOne, typename VectorTypeTwo>
  static inline VectorTypeOne operator+(const VectorTypeOne &v1,
                                        const VectorTypeTwo &v2) {
    return VectorAddition(v1, v2);
  }

  template<typename VectorTypeOne, typename VectorTypeTwo>
  static inline VectorTypeOne &operator+=(VectorTypeOne &v1,
                                          const VectorTypeTwo &v2) {
    return v1 = VectorAddition(v1, v2);
  }

  template<typename VectorTypeOne, typename VectorTypeTwo>
  static inline VectorTypeOne VectorSubtraction(const VectorTypeOne &v1,
                                                const VectorTypeTwo &v2) {
    VectorTypeOne a(v1);
    for(int i = 0; i < VectorTypeOne::Size; i++) {
      a(i) -= v2(i);
    }
    return a;
  }

  template<typename VectorTypeOne, typename VectorTypeTwo>
  static inline VectorTypeOne operator-(const VectorTypeOne &v1,
                                        const VectorTypeTwo &v2) {
    return VectorSubtraction(v1, v2);
  }

  template<typename VectorTypeOne, typename VectorTypeTwo>
  static inline VectorTypeOne &operator-=(VectorTypeOne &v1,
                                          const VectorTypeTwo &v2) {
    return v1 = VectorSubtraction(v1, v2);
  }

  template<typename T>
  class VectorTraits {
   public:
    static const bool IsVector = false;
  };

  template<typename T, const int N>
  class VectorTraits<VectorBase<T, N> > {
   public:
    static const bool IsVector = true;
  };

  #define REGISTER_VECTOR_TYPE(TYPE) \
  template<> \
  class VectorTraits< TYPE > { \
   public: \
    static const bool IsVector = true; \
  }

  #define REGISTER_ONE_TEMPLATE_VECTOR_TYPE(TYPE) \
  template<typename T> \
  class VectorTraits< TYPE <T> > { \
   public: \
    static const bool IsVector = true; \
  }

  template<bool condition, typename TypeOne, typename TypeTwo>
  class VectorSwitch {
   private:
    const TypeOne &m_A;
    const TypeTwo &m_B;
   public:
    typedef TypeOne VectorType;
    typedef TypeTwo ScalarType;

    VectorSwitch(const TypeOne &a, const TypeTwo &b)
      : m_A(a), m_B(b) { }

    const VectorType &GetVector() { return m_A; }
    const ScalarType &GetScalar() { return m_B; }
  };

  template<typename TypeOne, typename TypeTwo>
  class VectorSwitch<false, TypeOne, TypeTwo> {
   private:
    const TypeOne &m_A;
    const TypeTwo &m_B;

   public:
    typedef TypeTwo VectorType;
    typedef TypeOne ScalarType;

    VectorSwitch(const TypeOne &a, const TypeTwo &b)
      : m_A(a), m_B(b) { }

    const VectorType &GetVector() { return m_B; }
    const ScalarType &GetScalar() { return m_A; }
  };

  template<typename VectorType, typename ScalarType>
  static inline VectorType ScalarMultiply(const VectorType &v, const ScalarType &s) {
    VectorType a(v);
    for(int i = 0; i < VectorType::Size; i++)
      a(i) = static_cast<typename VectorType::ScalarType>(a(i) * s);
    return a;
  }

  template<typename TypeOne, typename TypeTwo>
  static inline 
    typename VectorSwitch< VectorTraits<TypeOne>::IsVector, TypeOne, TypeTwo >::VectorType
  operator*(const TypeOne &v1, const TypeTwo &v2) {
    typedef VectorSwitch< VectorTraits<TypeOne>::IsVector, TypeOne, TypeTwo > VSwitch;
    VSwitch s(v1, v2);
    return ScalarMultiply(s.GetVector(), s.GetScalar());
  }

  template<typename VectorType, typename ScalarType>
  static inline VectorType ScalarDivide(const VectorType &v, const ScalarType &s) {
    VectorType a(v);
    for(int i = 0; i < VectorType::Size; i++)
      a(i) = static_cast<typename VectorType::ScalarType>(a(i) / s);
    return a;
  }

  template<typename TypeOne, typename TypeTwo>
  static inline
    typename VectorSwitch< VectorTraits<TypeOne>::IsVector, TypeOne, TypeTwo >::VectorType
  operator/(const TypeOne &v1, const TypeTwo &v2) {
    typedef VectorSwitch< VectorTraits<TypeOne>::IsVector, TypeOne, TypeTwo > VSwitch;
    VSwitch s(v1, v2);
    return ScalarDivide(s.GetVector(), s.GetScalar());
  }

  template<typename VectorType, typename ScalarType>
  static inline VectorType &operator*=(VectorType &v, const ScalarType &s) {
    return v = ScalarMultiply(v, s);
  }

  template<typename VectorType, typename ScalarType>
  static inline VectorType &operator/=(VectorType &v, const ScalarType &s) {
    return v = ScalarDivide(v, s);
  }
};

#endif  // BASE_INCLUDE_VECTORBASE_H_
