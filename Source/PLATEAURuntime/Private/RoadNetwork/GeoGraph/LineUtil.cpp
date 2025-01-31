#include "RoadNetwork/GeoGraph/LineUtil.h"

#include "RoadNetwork/Util/PLATEAUVector2DEx.h"

FLine::FLine(const FVector& InP0, const FVector& InP1)
    : P0(InP0), P1(InP1) {
}

float FLine::GetSqrMag() const {
    return (P0 - P1).SizeSquared();
}

float FLine::GetMag() const {
    return (P0 - P1).Size();
}

FVector FLine::GetDirectionA2B() const {
    return (P1 - P0).GetSafeNormal();
}

bool FLineUtil::LineIntersection(const FVector2D& A, const FVector2D& B, const FVector2D& C, const FVector2D& D,
    FVector2D& OutIntersection, float& OutT1, float& OutT2) {
    OutT1 = OutT2 = 0.0f;
    OutIntersection = FVector2D::ZeroVector;

    const auto Deno = FPLATEAUVector2DEx::Cross(B - A, D - C);
    if (FMath::Abs(Deno) < Epsilon) {
        return false;
    }

    OutT1 = FPLATEAUVector2DEx::Cross(C - A, D - C) / Deno;
    OutT2 = FPLATEAUVector2DEx::Cross(B - A, A - C) / Deno;
    OutIntersection = FMath::Lerp(A, B, OutT1);
    return true;
}

bool FLineUtil::SegmentIntersection(const FVector2D& S1St, const FVector2D& S1En,
    const FVector2D& S2St, const FVector2D& S2En,
    FVector2D& OutIntersection, float& OutT1, float& OutT2) {
    const bool Result = LineIntersection(S1St, S1En, S2St, S2En, OutIntersection, OutT1, OutT2);
    return Result && OutT1 >= 0.0f && OutT1 <= 1.0f && OutT2 >= 0.0f && OutT2 <= 1.0f;
}

float FLineUtil::GetLineSegmentLength(const TArray<FVector>& Vertices) {
    float Length = 0.0f;
    for (int32 i = 0; i < Vertices.Num() - 1; ++i) {
        Length += (Vertices[i + 1] - Vertices[i]).Size();
    }
    return Length;
}

FVector2D FLineUtil::GetNearestPoint(const FRay2D& Self, const FVector2D& P, float& OutT) {
    const FVector2D D = Self.Direction;
    OutT = FVector2D::DotProduct(Self.Direction, P - Self.Origin);
    return Self.Origin + OutT * D;
}
