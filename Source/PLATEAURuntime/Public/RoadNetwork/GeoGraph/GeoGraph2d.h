#pragma once

#include "CoreMinimal.h"
#include "LineUtil.h"
#include "Math/Vector2D.h"
#include "Math/Vector.h"
#include "Containers/Array.h"
#include "RoadNetwork/RnDef.h"
#include "RoadNetwork/Util/RnEx.h"
#include "RoadNetwork/Util/Vector2DEx.h"

class PLATEAURUNTIME_API FGeoGraph2D {
public:
    static constexpr float Eps = 1e-5f;

    struct FVector2DEquitable {
        float Tolerance;

        FVector2DEquitable(float InTolerance) : Tolerance(InTolerance) {}

        bool Equals(const FVector2D& X, const FVector2D& Y) const;
    };

    static TArray<FVector> ComputeConvexVolume(const TArray<FVector>& Vertices, const FVector2D& (*ToVec2)(const FVector&));

    static bool IsClockwise(const TArray<FVector2D>& Vertices)
    {
        return IsClockwise<FVector2D>(Vertices, [](const FVector2D& V) { return V; });
    }

    template<class T>
    static bool IsClockwise(const TArray<T>& Vertices, TFunction<FVector2D(const T&)> ToVec2)
    {
        if (Vertices.Num() <= 2) {
            return false;
        }

        float Sum = 0;
        for (int32 i = 0; i < Vertices.Num(); ++i) {
            const FVector2D& V1 = ToVec2(Vertices[i]);
            const FVector2D& V2 = ToVec2(Vertices[(i + 1) % Vertices.Num()]);            
            Sum += FVector2DEx::Cross(V1, V2);
        }

        return Sum < 0;
    }

    static bool Contains(const TArray<FVector2D>& Vertices, const FVector2D& Point);

    template<typename T>
    static TArray<T> ComputeConvexVolume(
        const TArray<T>& Vertices,
        TFunction<FVector(const T&)> ToVec3,
        EAxisPlane Plane,
        float SameLineTolerance = 0.0f);

    static TArray<FVector> ComputeConvexVolume(
        const TArray<FVector>& Vertices,
        TFunction<FVector2D(const FVector&)> ToVec2);

    static TArray<FVector2D> ComputeConvexVolume(const TArray<FVector2D>& Vertices);

    static TArray<int32> GetNearVertexTable(
        const TArray<FVector>& Vertices,
        TFunction<float(const FVector&, const FVector&)> CalcDistance,
        float Epsilon = 0.1f);

    static TArray<FVector> ComputeMeshOutlineVertices(
        const TArray<FVector>& Vert,
        const TArray<int32>& Triangles,
        TFunction<FVector2D(const FVector&)> ToVec2,
        float Epsilon = 0.1f);

    template<typename T>
    class FComputeOutlineResult {
    public:
        bool Success = false;
        bool HasSelfCrossing = false;
        TArray<T> Outline;

        FComputeOutlineResult(){}
        FComputeOutlineResult(bool success, bool hasSelfCrossing, const TArray<T>& outline)
            : Success(success)
            , HasSelfCrossing(hasSelfCrossing)
            , Outline(outline) {
        }
    };

    template<class T>
    static FComputeOutlineResult<T> ComputeOutline(
        const TArray<T>& Vertices,
        TFunction<FVector(const T&)> ToVec3,
        EAxisPlane Plane,
        TFunction<TArray<T>(const T&)> GetNeighbor);

    template<class T>
    static void RemoveSelfCrossing(
        TArray<T>& Points,
        TFunction<FVector2D(T)> Selector,
        TFunction<T(T, T, T, T, const FVector2D&, float, float)> CreateIntersectionPoint);

    static float CalcTotalAngle(const TArray<FVector>& Points, TFunction<FVector2D(const FVector&)> ToVec2);
    static float CalcTotalAngle(const TArray<FVector2D>& Points);

    static bool IsConvex(const TArray<FVector2D>& Points);

    template<class T>
    static int32 FindMostLeftBottom(const TArray<T>& Points, TFunction<FVector2D(const T&)> ToVec2)
    {
        int32 Result = 0;
        for (int32 i = 1; i < Points.Num(); ++i) 
        {
            auto V = ToVec2(Points[i]);
            auto V2 = ToVec2(Points[Result]);
            if (V.X < V2.X ||
                (V.X == V2.X && V.Y < V2.Y)) {
                Result = i;
            }
        }
        return Result;
    }
    static float Cross(const FVector2D& A, const FVector2D& B);  
private:
    static bool IsLastClockwise(const TArray<FVector2D>& List);
};

template<typename T>
TArray<T> FGeoGraph2D::ComputeConvexVolume(
    const TArray<T>& Vertices,
    TFunction<FVector(const T&)> ToVec3,
    EAxisPlane Plane,
    float SameLineTolerance)
{
    if (Vertices.Num() <= 3) {
        return Vertices;
    }

    auto ToVec2 = [&](const T& A) -> FVector2D
    {
        return FAxisPlaneEx::GetTangent(ToVec3(A), Plane);
    };

    auto IsLastClockwise = [&](const TArray<T>& List)
    {
        if (List.Num() <= 2) {
            return true;
        }
        const FVector2D& V1 = ToVec2(List[List.Num() - 1]);
        const FVector2D& V2 = ToVec2(List[List.Num() - 2]);
        const FVector2D& V3 = ToVec2(List[List.Num() - 3]);

        const FVector2D D1 = V1 - V2;
        const FVector2D D2 = V2 - V3;

        // Cross product in 2D
        float Cross = D1.X * D2.Y - D1.Y * D2.X;
        return Cross > 0;
    };

    // Find the leftmost bottom point
    int32 StartIndex = FindMostLeftBottom<T>(Vertices, ToVec2);
    TArray<T> Points = { Vertices[StartIndex] };
    TArray<int32> Stack = { StartIndex };

    // Sort points by angle and distance
    TArray<TTuple<int32, float>> Angles;
    for (int32 i = 0; i < Vertices.Num(); ++i) {
        if (i == StartIndex) continue;

        FVector2D Diff = ToVec2(Vertices[i]) - ToVec2(Vertices[StartIndex]);
        float Angle = FMath::Atan2(Diff.Y, Diff.X);
        Angles.Add(MakeTuple(i, Angle));
    }

    Angles.Sort([](const TTuple<int32, float>& A, const TTuple<int32, float>& B) {
        return A.Value < B.Value;
        });

    // Graham scan
    for (const auto& IndexAngle : Angles) {
        while (Stack.Num() >= 2) {
            FVector2D P1 = ToVec2(Vertices[Stack[Stack.Num() - 2]]);
            FVector2D P2 = ToVec2(Vertices[Stack.Last()]);
            FVector2D P3 = ToVec2(Vertices[IndexAngle.Key]);

            FVector2D V1 = P2 - P1;
            FVector2D V2 = P3 - P2;

            if (Cross(V1, V2) > 0) {
                break;
            }
            Stack.Pop();
            Points.Pop();
        }

        Stack.Add(IndexAngle.Key);
        Points.Add(Vertices[IndexAngle.Key]);
    }

    return Points;
}

template<typename T>
void FGeoGraph2D::RemoveSelfCrossing(
    TArray<T>& Points,
    TFunction<FVector2D(T)> Selector,
    TFunction<T(T, T, T, T, const FVector2D&, float, float)> Creator)
{
    for (int32 i = 0; i < Points.Num() - 2; ++i) {
        FVector2D P1 = Selector(Points[i]);
        FVector2D P2 = Selector(Points[i + 1]);

        for (int32 j = i + 2; j < Points.Num() - 1;) {
            FVector2D P3 = Selector(Points[j]);
            FVector2D P4 = Selector(Points[j + 1]);

            FVector2D Intersection;
            float F1 = 0.f;
            float F2 = 0.f;
            if (FLineUtil::SegmentIntersection(P1, P2, P3, P4, Intersection, F1, F2)) {
                auto newNode = Creator(Points[i], Points[i + 1], Points[j], Points[j + 1], Intersection, F1, F2);
                Points.RemoveAt(i + 1, j - i);
                Points.Insert(newNode, i + 1);
                // もう一回最初から検索しなおす
                j = i + 2;
            }
            else
            {
                j++;
            }
        }
    }
}

template<typename T>
FGeoGraph2D::FComputeOutlineResult<T> FGeoGraph2D::ComputeOutline(
    const TArray<T>& Vertices,
    TFunction<FVector(const T&)> ToVec3,
    EAxisPlane Plane,
    TFunction<TArray<T>(const T&)> GetNeighbor)
{

    FComputeOutlineResult<T> Result;
    Result.Success = false;
    Result.HasSelfCrossing = false;

    if (Vertices.Num() == 0) {
        return Result;
    }

    TSet<T> VisitedPoints;
    auto& OutlinePoints = Result.Outline;

    // 配列をコピーする
    auto Keys = Vertices;
    if(Keys.Num() <= 2)
    {
        Result.Outline = Keys;
        Result.Success = false;
        return Result;
    }

    auto ToVec2 = [&](T A) { return FAxisPlaneEx::GetTangent(ToVec3(A), Plane); };

    Keys.Sort([&](const T& A, const T& B) 
        {
            auto A2 = ToVec2(A);
            auto B2 = ToVec2(B);
            if (A2.X < B2.X)
                return true;
            if (A2.X > B2.X)
                return false;
            return A2.Y < B2.Y;
        });

    struct EvalValue
    {
        float Angle;
        float SqrLen;
    };

    auto Search = [&](T Start, FVector2D Dir,
        // (angle, sqrLen), (angle, sqrLen) -> evaluation value
        TFunction<int(EvalValue, EvalValue)> Compare)
        // success, hasSelfCrossing, outlineVertices
        -> FComputeOutlineResult<T> {
        auto ret = new TArray<T>{ Start };
        auto hasCrossing = false;
        auto Eval = [](FVector2D axis, FVector2D a) -> EvalValue {
            auto ang = FVector2DEx::SignedAngle(axis, a);
            if (ang < 0.f)
                ang += 360.f;
            auto sqrLen = a.SquaredLength();
            EvalValue R;
            R.Angle = ang;
            R.SqrLen = sqrLen;
            return R;
            };

        while (true) {
            auto last = ToVec2(ret[ret.Num() - 1]);
            TArray<T> neighbors = GetNeighbor(ret[ret.Num() - 1]);
            if (neighbors.Num() == 0)
                break;


            // 途中につながるようなものは削除
            TArray<T> filtered = neighbors;
            if (ret.Num() > 1) {
                // 
                filtered.RemoveAll([&](T V) {
                    return V == ret[ret.Num() - 2];
                    });
            }

            if (filtered.Count == 0)
                filtered = neighbors;
            if (filtered.Count == 0)
                break;

            auto next = filtered[0];
            auto eval0 = Eval(Dir, ToVec2(next) - last);
            for (auto I = 1; I < filtered.Num(); ++I) {
                auto v = filtered[I];
                // 最も外側に近い点を返す
                auto eval1 = Eval(Dir, ToVec2(v) - last);
                if (Compare(eval0, eval1) < 0) {
                    next = v;
                    eval0 = eval1;
                }
            }

            // 先頭に戻ってきたら終了
            if (ret[0].Equals(next))
                return FComputeOutlineResult<T>(true, hasCrossing, ret);

            // 途中に戻ってくる場合
            auto index = ret.IndexOf(next);
            if (index >= 0) {
                // ループを検出したら終了
                if (index > 0 && ret[index - 1] == ret[ret.Num() - 1]) {
                    return FComputeOutlineResult<T>(false, hasCrossing, ret);
                }
                hasCrossing = true;
            }

            ret.Add(next);
            Dir = last - ToVec2(next);
        }

        return FComputeOutlineResult<T>(false, hasCrossing, ret);
        };

    auto leftSearch = Search(
        Keys[0]
        , -FVector2D::UnitY()
        , [](EvalValue A, EvalValue B) 
        {
            auto x = -RnEx::Compare(A.Angle, B.Angle);
            if(x == 0)
                x = RnEx::Compare(A.SqrLen, B.SqrLen);
            return x;
        });
    // 見つかったらそれでおしまい
    if (leftSearch.Success)
        return leftSearch;

    // 見つからない場合(３次元的なねじれの位置がある場合等)
    // 反時計回りにも探す
    auto rightSearch = Search(
        Keys[0]
        , FVector2D::UnitY()
        , [](EvalValue A, EvalValue B) {
            auto x = RnEx::Compare(A.Angle, B.Angle);
            if (x == 0)
                x = RnEx::Compare(A.SqrLen, B.SqrLen);
            return x;
        });

    // 右回りで見つかったらそれでおしまい
    if (rightSearch.Success) {
        return rightSearch;
    }

    // #TODO:どっちも見つからない場合はしょうがないので
    // 両方の結果をマージする
    Result.Outline = leftSearch.Outline;
    // 0番目は共通なので削除
    rightSearch.Outline.RemoveAt(0);
    while (rightSearch.Outline.Num() > 0) {
        auto v = rightSearch.Outline[0];
        rightSearch.Outline.RemoveAt(0);
        auto index = Result.Outline.IndexOf(v);
        if (index >= 0) {
            Result.Success = true;
            Result.Outline.RemoveAt(index, Result.Outline.Count - index);
            Result.Outline.Add(v);
            break;
        }
        Result.Outline.Add(v);
    }


    return Result;
}

