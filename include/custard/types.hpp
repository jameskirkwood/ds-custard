#ifndef CUSTARD_TYPES_HPP
#define CUSTARD_TYPES_HPP

#include <iostream>

#include <nds/arm9/math.h>

namespace custard {

//-----------------------------------------------------------------------------
template <class T> inline T max(T a, T b) { return a > b ? a : b; }
template <class T> inline T min(T a, T b) { return a < b ? a : b; }
template <class T> inline T abs(T x) { return x > 0 ? x : -x; }
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template <class T> inline T pow(T x, int i) {
//-----------------------------------------------------------------------------
  if (i < 0) return T(1.0) / pow(x, -i);
  if (i == 0) return T(1.0);
  if (i == 1) return x;
  if (i == 2) return x * x;
  if (i & 1) return pow(x, i - 1) * x;
  return pow(pow(x, i >> 1), 2);
}

//-----------------------------------------------------------------------------
class q12 {
//-----------------------------------------------------------------------------
  int32 i;
  static q12 with_i(int j) { q12 q; q.i = j; return q; }
public:
  q12() {}
  q12(float f) : i(f * (1 << 12)) {}
  q12(double f) : i(f * (1 << 12)) {}
  q12(int i) : i(i << 12) {}
  explicit operator float() const { return float(i) / (1 << 12); }
  q12 operator+(const q12& q) const { return with_i(i + q.i); }
  q12& operator+=(const q12& q) { i = (*this + q).i; return *this; }
  q12 operator-(const q12& q) const { return with_i(i - q.i); }
  q12& operator-=(const q12& q) { i = (*this - q).i; return *this; }
  q12 operator*(const q12& q) const { return with_i(mulf32(i, q.i)); }
  q12& operator*=(const q12& q) { i = (*this * q).i; return *this; }
  q12 operator/(const q12& q) const { return with_i(divf32(i, q.i)); }
  q12& operator/=(const q12& q) { i = (*this / q).i; return *this; }
  q12 operator<<(const int& j) const { return with_i(i << j); }
  q12& operator<<=(const int& j) { i = (*this << j).i; return *this; }
  q12 operator>>(const int& j) const { return with_i(i >> j); }
  q12& operator>>=(const int& j) { i = (*this >> j).i; return *this; }
  q12 operator-() const { return with_i(-i); }
  bool operator<(const q12& q) const { return i < q.i; }
  bool operator>(const q12& q) const { return i > q.i; }
  bool operator<=(const q12& q) const { return i <= q.i; }
  bool operator>=(const q12& q) const { return i >= q.i; }
  bool operator==(const q12& q) const { return i == q.i; }
  bool operator!=(const q12& q) const { return i != q.i; }
  friend std::ostream& operator<< (std::ostream& os, const q12& q) {
    return os << float(q);
  }
};

//-----------------------------------------------------------------------------
template <class T> struct vec2 {
//-----------------------------------------------------------------------------
  T x, y;
  vec2() : x(), y() {}
  vec2(T x, T y) : x(x), y(y) {}
  vec2 max(T t) const {
    return vec2(custard::max(x, t), custard::max(y, t));
  }
  vec2 max(vec2 v) const {
    return vec2(custard::max(x, v.x), custard::max(y, v.y));
  }
  vec2 min(T t) const {
    return vec2(custard::min(x, t), custard::min(y, t));
  }
  vec2 min(vec2 v) const {
    return vec2(custard::min(x, v.x), custard::min(y, v.y));
  }
  T area() const { return x * y; }
  vec2 operator+(T t) const { return vec2(x + t, y + t); }
  vec2 operator+(vec2 v) const { return vec2(x + v.x, y + v.y); }
  vec2 operator-(T t) const { return vec2(x - t, y - t); }
  vec2 operator-(vec2 v) const { return vec2(x - v.x, y - v.y); }
  vec2 operator*(T t) const { return vec2(x * t, y * t); }
  vec2 operator*(vec2 v) const { return vec2(x * v.x, y * v.y); }
  vec2 operator/(T t) const { return vec2(x / t, y / t); }
  vec2 operator/(vec2 v) const { return vec2(x / v.x, y / v.y); }
  vec2 operator>>(int t) const { return vec2(x >> t, y >> t); }
  vec2 operator-() const { return vec2(-x, -y); }
};

//-----------------------------------------------------------------------------
typedef vec2<int> ivec2;
//-----------------------------------------------------------------------------

// discrete range iterable
//-----------------------------------------------------------------------------
template <class T=int> class range {
//-----------------------------------------------------------------------------
  T t0, t1, step; // t0 included, t1 excluded, t0 <= t1
public:
  struct iterator {
    T t;
    T operator*() const { return t; }
    bool operator!=(iterator other) const { return t != other.t; }
    void operator++() { ++t; }
  };
  range(T t1) : t0(), t1(t1), step(1) {}
  range(T t0, T t1, T step=1) : t0(t0), t1(t1), step(step) {}
  iterator begin() { return {t0}; }
  iterator end() { return {t1}; }
};

// two-dimensional discrete range iterable
//-----------------------------------------------------------------------------
template <> class range<ivec2> {
//-----------------------------------------------------------------------------
  ivec2 v0, v1; // v0 included, v1 excluded, v0 <= v1
public:
  struct iterator {
    ivec2 v; int xend;
    ivec2 operator*() const { return v; }
    bool operator!=(int yend) const { return v.y != yend; }
    void operator++() {
      ++v.x;
      if (v.x == xend) {
        v.x = 0;
        ++v.y;
      }
    }
  };
  range(ivec2 v1) : v0(), v1(v1) {}
  range(ivec2 v0, ivec2 v1) : v0(v0), v1(v1) {}
  iterator begin() { return {v0, v1.x}; }
  int end() { return v1.y; }
};

// two-dimensional zero-based range helper
//-----------------------------------------------------------------------------
inline range<ivec2> range2(int xend, int yend) {
//-----------------------------------------------------------------------------
  return range(ivec2(xend, yend));
}

}

#endif
