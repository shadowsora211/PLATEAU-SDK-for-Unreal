// Copyright 2023 Ministry of Land, Infrastructure and Transport

#pragma once

#include "CoreMinimal.h"
#include "PLATEAUGeometry.h"
#include "GameFramework/Actor.h"

#include <plateau/dataset/city_model_package.h>

#include "PLATEAUInstancedCityModel.generated.h"

struct FPLATEAUMinMaxLod {
    int MinLod = 0;
    int MaxLod = 0;
};

USTRUCT(BlueprintType)
struct PLATEAURUNTIME_API FPLATEAUCityObjectInfo {
    GENERATED_USTRUCT_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PLATEAU")
        FString DatasetName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PLATEAU")
        FString GmlName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PLATEAU")
        FString ID;
};


/**
 * @brief インポートされた3D都市モデルを表します。
 * 各地物のLod、CityGMLファイル名が分かるように、コンポーネント構造が以下のようになっています。
 *
 * RootComponent
 * |-{CityGMLファイル名}
 *  |-{Lod}
 *   |-{地物ID}
 * ...
 */
UCLASS()
class PLATEAURUNTIME_API APLATEAUInstancedCityModel : public AActor {
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    APLATEAUInstancedCityModel();

    UPROPERTY(EditAnywhere, Category = "PLATEAU")
        FPLATEAUGeoReference GeoReference;

    UPROPERTY(EditAnywhere, Category = "PLATEAU")
        FString DatasetName;

    UFUNCTION(BlueprintCallable, meta = (Category = "PLATEAU|CityGML"))
        FPLATEAUCityObjectInfo GetCityObjectInfo(USceneComponent* Component);

    /**
     * @brief 3D都市モデル内に含まれるパッケージ種を返します。
     */
    plateau::dataset::PredefinedCityModelPackage GetCityModelPackages() const;

    /**
     * @brief 3D都市モデル内の各地物について、引数に従って可視化・非可視化します。
     * @param InPackage 可視化するパッケージ
     * @param PackageToLodRangeMap パッケージごとのLodに関してのユーザー選択結果
     * @param bOnlyMaxLod trueの場合各地物について提供されている最大のLodのみ可視化します。
     * @return thisを返します。
     */
    APLATEAUInstancedCityModel* FilterByLods(const plateau::dataset::PredefinedCityModelPackage InPackage, const TMap<plateau::dataset::PredefinedCityModelPackage, FPLATEAUMinMaxLod>& PackageToLodRangeMap, const bool bOnlyMaxLod);

    /**
     * @brief 3D都市モデル内の各地物について、引数に従って可視化・非可視化します。
     * @param InCityObjectType 可視化する地物タイプ
     * @return thisを返します。
     */
    APLATEAUInstancedCityModel* FilterByFeatureTypes(const citygml::CityObject::CityObjectsType InCityObjectType);

    /**
     * @brief 3D都市モデル内に含まれるLodを取得します。
     * @param InPackage 検索対象のパッケージ。フラグによって複数指定可能です。
     * @return 存在するLodの最小最大値
     */
    FPLATEAUMinMaxLod GetMinMaxLod(const plateau::dataset::PredefinedCityModelPackage InPackage) const;

    /**
     * @brief フィルタリング処理を実行中かどうかを返します。
     */
    bool IsFiltering();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    
    /**
     * @brief 3D都市モデル内のGMLファイルComponentの一覧を取得します。
     */
    const TArray<TObjectPtr<USceneComponent>>& GetGmlComponents() const;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

private:
    TAtomic<bool> bIsFiltering;

    void FilterByFeatureTypesInternal(const citygml::CityObject::CityObjectsType InCityObjectType);
};
