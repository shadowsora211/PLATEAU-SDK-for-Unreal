#pragma once

#include "CoreMinimal.h"
#include "RoadNetwork/PLATEAURnDef.h"
#include "RnModel.generated.h"
class URnRoad;
class URnIntersection;
class URnSideWalk;
class URnLane;
class URnIntersectionEdge;
class URnLineString;
class URnWay;
class UPLATEAUCityObjectGroup;
class URnRoadBase;


USTRUCT(BlueprintType)
struct FRnModelCalibrateIntersectionBorderOption
{
    GENERATED_BODY()
public:
    // 交差点の停止線を道路側に移動させる量
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PLATEAU")
    float MaxOffsetMeter = 5.0f;

    // 道路の長さがこれ以下にならないように交差点の移動量を減らす
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PLATEAU")
    float NeedRoadLengthMeter = 23.0f;
};

UCLASS()
class URnModel : public UObject
{
public:
    const FString& GetFactoryVersion() const;

    void SetFactoryVersion(const FString& InFactoryVersion);

private:
    GENERATED_BODY()

public:
    static constexpr float Epsilon = SMALL_NUMBER;
    static constexpr EAxisPlane Plane = EAxisPlane::Xz;

    URnModel();

    void Init(){}

    // 道路を追加
    void AddRoadBase(const TRnRef_T<URnRoadBase>& RoadBase);

    // 道路を追加
    void AddRoad(const TRnRef_T<URnRoad>& Road);

    // 道路を削除
    void RemoveRoad(const TRnRef_T<URnRoad>& Road);

    // 交差点を追加
    void AddIntersection(const TRnRef_T<URnIntersection>& Intersection);

    // 交差点を削除
    void RemoveIntersection(const TRnRef_T<URnIntersection>& Intersection);

    // 歩道を追加
    void AddSideWalk(const TRnRef_T<URnSideWalk>& SideWalk);

    // 歩道を削除
    void RemoveSideWalk(const TRnRef_T<URnSideWalk>& SideWalk);

    // 道路を取得
    const TArray<TRnRef_T<URnRoad>>& GetRoads() const;

    // 交差点を取得
    const TArray<TRnRef_T<URnIntersection>>& GetIntersections() const;

    // 歩道を取得
    const TArray<TRnRef_T<URnSideWalk>>& GetSideWalks() const;

    // 指定したCityObjectGroupを含む道路を取得
    TRnRef_T<URnRoad> GetRoadBy(UPLATEAUCityObjectGroup* TargetTran) const;

    // 指定したCityObjectGroupを含む交差点を取得
    TRnRef_T<URnIntersection> GetIntersectionBy(UPLATEAUCityObjectGroup* TargetTran) const;

    // 指定したCityObjectGroupを含む歩道を取得
    TRnRef_T<URnSideWalk> GetSideWalkBy(UPLATEAUCityObjectGroup* TargetTran) const;

    // 指定したCityObjectGroupを含む道路/交差点を取得
    TRnRef_T<URnRoadBase> GetRoadBaseBy(UPLATEAUCityObjectGroup* TargetTran) const;

    // 指定した道路/交差点に接続されている道路/交差点を取得
    TArray<TRnRef_T<URnRoadBase>> GetNeighborRoadBases(const TRnRef_T<URnRoadBase>& RoadBase) const;

    // 指定した道路/交差点に接続されている道路を取得
    TArray<TRnRef_T<URnRoad>> GetNeighborRoads(const TRnRef_T<URnRoadBase>& RoadBase) const;

    // 指定した道路/交差点に接続されている交差点を取得
    TArray<TRnRef_T<URnIntersection>> GetNeighborIntersections(const TRnRef_T<URnRoadBase>& RoadBase) const;

    // 指定した道路/交差点に接続されている歩道を取得
    TArray<TRnRef_T<URnSideWalk>> GetNeighborSideWalks(const TRnRef_T<URnRoadBase>& RoadBase) const;

    // 交差点の境界線を調整する
    void CalibrateIntersectionBorder(const FRnModelCalibrateIntersectionBorderOption& Option);

    // 道路ネットワークを作成する
    static TRnRef_T<URnModel> Create();

    // 指定したRoadBaseに接続されている道路/交差点を取得
    TArray<TRnRef_T<URnRoadBase>> GetConnectedRoadBases(const TRnRef_T<URnRoadBase>& RoadBase) const;

    // 指定したRoadBaseに接続されている道路を取得
    TArray<TRnRef_T<URnRoad>> GetConnectedRoads(const TRnRef_T<URnRoadBase>& RoadBase) const;

    // 指定したRoadBaseに接続されている交差点を取得
    TArray<TRnRef_T<URnIntersection>> GetConnectedIntersections(const TRnRef_T<URnRoadBase>& RoadBase) const;

    // 指定したRoadBaseに接続されている歩道を取得
    TArray<TRnRef_T<URnSideWalk>> GetConnectedSideWalks(const TRnRef_T<URnRoadBase>& RoadBase) const;

    // 指定したRoadBaseに接続されているRoadBaseを取得
    TArray<TRnRef_T<URnRoadBase>> GetConnectedRoadBasesRecursive(const TRnRef_T<URnRoadBase>& RoadBase) const;

    /// <summary>
    /// 連続した道路を一つのRoadにまとめる
    /// </summary>
    void MergeRoadGroup();

private:

    // 自動生成で作成されたときのバージョン
    FString FactoryVersion;

    // 道路リスト
    UPROPERTY()
    TArray<TObjectPtr<URnRoad>> Roads;

    // 交差点リスト
    UPROPERTY()
    TArray< TObjectPtr<URnIntersection>> Intersections;

    // 歩道リスト
    UPROPERTY()
    TArray< TObjectPtr<URnSideWalk>> SideWalks;

};
