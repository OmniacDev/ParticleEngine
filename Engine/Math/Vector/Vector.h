#pragma once

#include <type_traits>
#include <cmath>

#include "../Types.h"

template<typename T>
struct TVector2 {

    static_assert(std::is_arithmetic_v<T>, "Must be an arithmetic type");

    T X;
    T Y;

    explicit TVector2(T X, T Y) : X(X), Y(Y) {}

    TVector2<T> operator+(const TVector2<T> &V) const { return TVector2<T>(X + V.X, Y + V.Y); }

    TVector2<T> operator-(const TVector2<T> &V) const { return TVector2<T>(X - V.X, Y - V.Y); }

    TVector2<T> operator*(const TVector2<T> &V) const { return TVector2<T>(X * V.X, Y * V.Y); }

    TVector2<T> operator/(const TVector2<T> &V) const { return TVector2<T>(X / V.X, Y / V.Y); }

    TVector2<T> &operator+=(const TVector2<T> &V) {
        X += V.X;
        Y += V.Y;
        return *this;
    }

    TVector2<T> &operator-=(const TVector2<T> &V) {
        X -= V.X;
        Y -= V.Y;
        return *this;
    }

    TVector2<T> &operator*=(const TVector2<T> &V) {
        X *= V.X;
        Y *= V.Y;
        return *this;
    }

    TVector2<T> &operator/=(const TVector2<T> &V) {
        X /= V.X;
        Y /= V.Y;
        return *this;
    }

    TVector2<T> operator+(const T V) const { return TVector2<T>(X + V, Y + V); }

    TVector2<T> operator-(const T V) const { return TVector2<T>(X - V, Y - V); }

    TVector2<T> operator*(const T V) const { return TVector2<T>(X * V, Y * V); }

    TVector2<T> operator/(const T V) const { return TVector2<T>(X / V, Y / V); }

    TVector2<T> &operator+=(const T V) {
        X += V;
        Y += V;
        return *this;
    }

    TVector2<T> &operator-=(const T V) {
        X -= V;
        Y -= V;
        return *this;
    }

    TVector2<T> &operator*=(const T V) {
        X *= V;
        Y *= V;
        return *this;
    }

    TVector2<T> &operator/=(const T V) {
        X /= V;
        Y /= V;
        return *this;
    }

    bool operator==(const TVector2<T> &V) const { return (X == V.X) && (Y == V.Y); }

    bool operator!=(const TVector2<T> &V) const { return (X != V.X) || (Y != V.Y); }
};

template<typename T>
struct TVector3 {

    static_assert(std::is_arithmetic_v<T>, "Must be an arithmetic type");

    T X;
    T Y;
    T Z;

    explicit TVector3(T X, T Y, T Z) : X(X), Y(Y), Z(Z) {}

    TVector3<T> operator+(const TVector3<T> &V) const { return TVector3<T>(X + V.X, Y + V.Y, Z + V.Z); }

    TVector3<T> operator-(const TVector3<T> &V) const { return TVector3<T>(X - V.X, Y - V.Y, Z - V.Z); }

    TVector3<T> operator*(const TVector3<T> &V) const { return TVector3<T>(X * V.X, Y * V.Y, Z * V.Z); }

    TVector3<T> operator/(const TVector3<T> &V) const { return TVector3<T>(X / V.X, Y / V.Y, Z / V.Z); }

    TVector3<T> &operator+=(const TVector3<T> &V) {
        X += V.X;
        Y += V.Y;
        Z += V.Z;
        return *this;
    }

    TVector3<T> &operator-=(const TVector3<T> &V) {
        X -= V.X;
        Y -= V.Y;
        Z -= V.Z;
        return *this;
    }

    TVector3<T> &operator*=(const TVector3<T> &V) {
        X *= V.X;
        Y *= V.Y;
        Z *= V.Z;
        return *this;
    }

    TVector3<T> &operator/=(const TVector3<T> &V) {
        X /= V.X;
        Y /= V.Y;
        Z /= V.Z;
        return *this;
    }

    TVector3<T> operator+(const T V) const { return TVector3<T>(X + V, Y + V, Z + V); }

    TVector3<T> operator-(const T V) const { return TVector3<T>(X - V, Y - V, Z - V); }

    TVector3<T> operator*(const T V) const { return TVector3<T>(X * V, Y * V, Z * V); }

    TVector3<T> operator/(const T V) const { return TVector3<T>(X / V, Y / V, Z / V); }

    TVector3<T> &operator+=(const T V) {
        X += V;
        Y += V;
        Z += V;
        return *this;
    }

    TVector3<T> &operator-=(const T V) {
        X -= V;
        Y -= V;
        Z -= V;
        return *this;
    }

    TVector3<T> &operator*=(const T V) {
        X *= V;
        Y *= V;
        Z *= V;
        return *this;
    }

    TVector3<T> &operator/=(const T V) {
        X /= V;
        Y /= V;
        Z /= V;
        return *this;
    }

    bool operator==(const TVector3<T> &V) const { return (X == V.X) && (Y == V.Y) && (Z == V.Z); }

    bool operator!=(const TVector3<T> &V) const { return (X != V.X) || (Y != V.Y) || (Z != V.Z); }
};

using FVector2 = TVector2<float>;
using IVector2 = TVector2<int>;

using FVector3 = TVector3<float>;
using IVector3 = TVector3<int>;

template <typename T> TVector2<T> Normalize(const TVector2<T>& V) { return V / Length(V); }
template <typename T> TVector2<T> Orthogonal(const TVector2<T>& V) { return TVector2<T>(V.Y, -V.X); }
template <typename T> T Length(const TVector2<T>& V) { return (T)(std::sqrt((V.X * V.X) + (V.Y * V.Y))); }
template <typename T> T Dot(const TVector2<T>& V, const TVector2<T>& W) { return (V.X * W.X) + (V.Y * W.Y); }

template <typename T> TVector3<T> Normalize(const TVector3<T>& V) { return V / Length(V); }
template <typename T> TVector3<T> Cross(const TVector3<T>& V, const TVector3<T>& W) { return TVector3<T>((V.Y * W.Z) - (V.Z * W.Y),(V.X * W.Z) - (V.Z * W.X),(V.X * W.Y) - (V.Y * W.X)); }
template <typename T> T Length(const TVector3<T>& V) { return (T)(std::sqrt((V.X * V.X) + (V.Y * V.Y) + (V.Z * V.Z))); }
template <typename T> T Dot(const TVector3<T>& V, const TVector3<T>& W) { return (V.X * W.X) + (V.Y * W.Y) + (V.Z * W.Z); }

