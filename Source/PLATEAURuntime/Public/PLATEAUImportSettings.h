// Copyright 2023 Ministry of Land, Infrastructure and Transport

#pragma once

#include <plateau/polygon_mesh/mesh_extract_options.h>

#include "CoreMinimal.h"

#include <plateau/dataset/city_model_package.h>

#include "PLATEAUImportSettings.generated.h"

#define LOCTEXT_NAMESPACE "PLATEAUImportSettings"

namespace plateau {
    namespace dataset {
        enum class PredefinedCityModelPackage : uint32;
    }
}

UENUM(BlueprintType)
enum class EPLATEAUMeshGranularity : uint8 {
    //! 最小地物単位(LOD2, LOD3の各部品)
    PerAtomicFeatureObject = 0,
    //! 主要地物単位(建築物、道路等)
    PerPrimaryFeatureObject = 1,
    //! 都市モデル地域単位(GMLファイル内のすべてを結合)
    PerCityModelArea = 2
};

UENUM(BlueprintType)
enum class EPLATEAUTexturePackingResolution : uint8 {
    H2048W2048 = 0 UMETA(DisplayName = "2048x2048"),
    H4096W4096 = 1 UMETA(DisplayName = "4096x4096"),
    H8192W8192 = 2 UMETA(DisplayName = "8192x8192")
};

USTRUCT()
struct PLATEAURUNTIME_API FPLATEAUFeatureImportSettings {
    GENERATED_USTRUCT_BODY()

public:
    UPROPERTY(EditAnywhere, Category = "Import Settings")
        bool bImport = true;

    UPROPERTY(EditAnywhere, Category = "Import Settings")
        bool bImportTexture = true;

    UPROPERTY(EditAnywhere, Category = "Import Settings")
        bool bIncludeAttrInfo = true;
    
    UPROPERTY(EditAnywhere, Category = "Import Settings")
        bool bSetCollider = true;

    UPROPERTY(EditAnywhere, Category = "Import Settings")
        EPLATEAUMeshGranularity MeshGranularity = EPLATEAUMeshGranularity::PerPrimaryFeatureObject;

    UPROPERTY(EditAnywhere, Category = "Import Settings")
        bool bEnableTexturePacking = true;

    UPROPERTY(EditAnywhere, Category = "Import Settings")
        EPLATEAUTexturePackingResolution TexturePackingResolution = EPLATEAUTexturePackingResolution::H4096W4096;

    UPROPERTY(EditAnywhere, Category = "Import Settings", meta = (ClampMin = 0, UIMin = 0, ClamMax = 3, UIMax = 3))
        int MinLod = 0;

    UPROPERTY(EditAnywhere, Category = "Import Settings", meta = (ClampMin = 0, UIMin = 0, ClamMax = 3, UIMax = 3))
        int MaxLod = 3;

    UPROPERTY(EditAnywhere, Category = "Import Settings")
        UMaterialInterface* FallbackMaterial;

    /* 
    * @brief 地図タイルを付与するかどうかを指定します。地形パッケージでのみ使用されます。
    */
    UPROPERTY(EditAnywhere, Category = "Import Settings")
        bool bAttachMapTile;

    /*
    * @brief 地図タイルのURLを指定します。地形パッケージでのみ使用されます。
    */
    UPROPERTY(EditAnywhere, Category = "Import Settings")
        FString MapTileUrl;

    /*
    * @brief 地図タイルのズームレベルを指定します。地形パッケージでのみ使用されます。
    */
    UPROPERTY(EditAnywhere, Category = "Import Settings")
        int ZoomLevel;
};

UCLASS()
class PLATEAURUNTIME_API UPLATEAUImportSettings : public UObject {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category = "Import Settings")
        FPLATEAUFeatureImportSettings Building;
    UPROPERTY(EditAnywhere, Category = "Import Settings")
        FPLATEAUFeatureImportSettings Road;
    UPROPERTY(EditAnywhere, Category = "Import Settings")
        FPLATEAUFeatureImportSettings Vegetation;
    UPROPERTY(EditAnywhere, Category = "Import Settings")
        FPLATEAUFeatureImportSettings CityFurniture;
    UPROPERTY(EditAnywhere, Category = "Import Settings")
        FPLATEAUFeatureImportSettings Relief;
    UPROPERTY(EditAnywhere, Category = "Import Settings")
        FPLATEAUFeatureImportSettings DisasterRisk;
    UPROPERTY(EditAnywhere, Category = "Import Settings")
        FPLATEAUFeatureImportSettings LandUse;
    UPROPERTY(EditAnywhere, Category = "Import Settings")
        FPLATEAUFeatureImportSettings UrbanPlanningDecision;
    UPROPERTY(EditAnywhere, Category = "Import Settings")
        FPLATEAUFeatureImportSettings Railway;
    UPROPERTY(EditAnywhere, Category = "Import Settings")
        FPLATEAUFeatureImportSettings Waterway;
    UPROPERTY(EditAnywhere, Category = "Import Settings")
        FPLATEAUFeatureImportSettings WaterBody;
    UPROPERTY(EditAnywhere, Category = "Import Settings")
        FPLATEAUFeatureImportSettings Bridge;
    UPROPERTY(EditAnywhere, Category = "Import Settings")
        FPLATEAUFeatureImportSettings Track;
    UPROPERTY(EditAnywhere, Category = "Import Settings")
        FPLATEAUFeatureImportSettings Square;
    UPROPERTY(EditAnywhere, Category = "Import Settings")
        FPLATEAUFeatureImportSettings Tunnel;
    UPROPERTY(EditAnywhere, Category = "Import Settings")
        FPLATEAUFeatureImportSettings UndergroundFacility;
    UPROPERTY(EditAnywhere, Category = "Import Settings")
        FPLATEAUFeatureImportSettings UndergroundBuilding;
    UPROPERTY(EditAnywhere, Category = "Import Settings")
        FPLATEAUFeatureImportSettings Area;
    UPROPERTY(EditAnywhere, Category = "Import Settings")
        FPLATEAUFeatureImportSettings OtherConstruction;
    UPROPERTY(EditAnywhere, Category = "Import Settings")
        FPLATEAUFeatureImportSettings Generic;
    UPROPERTY(EditAnywhere, Category = "Import Settings")
        FPLATEAUFeatureImportSettings Unknown;

    FPLATEAUFeatureImportSettings GetFeatureSettings(plateau::dataset::PredefinedCityModelPackage Package) const {
        switch (Package) {
        case plateau::dataset::PredefinedCityModelPackage::Building: return Building;
        case plateau::dataset::PredefinedCityModelPackage::Road: return Road;
        case plateau::dataset::PredefinedCityModelPackage::Vegetation: return Vegetation;
        case plateau::dataset::PredefinedCityModelPackage::CityFurniture: return CityFurniture;
        case plateau::dataset::PredefinedCityModelPackage::Relief: return Relief;
        case plateau::dataset::PredefinedCityModelPackage::DisasterRisk: return DisasterRisk;
        case plateau::dataset::PredefinedCityModelPackage::LandUse: return LandUse;
        case plateau::dataset::PredefinedCityModelPackage::UrbanPlanningDecision: return UrbanPlanningDecision;
        case plateau::dataset::PredefinedCityModelPackage::Railway: return Railway;
        case plateau::dataset::PredefinedCityModelPackage::Waterway: return Waterway;
        case plateau::dataset::PredefinedCityModelPackage::WaterBody: return WaterBody;
        case plateau::dataset::PredefinedCityModelPackage::Bridge: return Bridge;
        case plateau::dataset::PredefinedCityModelPackage::Track: return Track;
        case plateau::dataset::PredefinedCityModelPackage::Square: return Square;
        case plateau::dataset::PredefinedCityModelPackage::Tunnel: return Tunnel;
        case plateau::dataset::PredefinedCityModelPackage::UndergroundFacility: return UndergroundFacility;
        case plateau::dataset::PredefinedCityModelPackage::UndergroundBuilding: return UndergroundBuilding;
        case plateau::dataset::PredefinedCityModelPackage::Area: return Area;
        case plateau::dataset::PredefinedCityModelPackage::OtherConstruction: return OtherConstruction;
        case plateau::dataset::PredefinedCityModelPackage::Generic: return Generic;
        case plateau::dataset::PredefinedCityModelPackage::Unknown: return Unknown;
        default: return Unknown;
        }
    }

    FPLATEAUFeatureImportSettings& GetFeatureSettingsRef(plateau::dataset::PredefinedCityModelPackage Package) {
        switch (Package) {
        case plateau::dataset::PredefinedCityModelPackage::Building: return Building;
        case plateau::dataset::PredefinedCityModelPackage::Road: return Road;
        case plateau::dataset::PredefinedCityModelPackage::Vegetation: return Vegetation;
        case plateau::dataset::PredefinedCityModelPackage::CityFurniture: return CityFurniture;
        case plateau::dataset::PredefinedCityModelPackage::Relief: return Relief;
        case plateau::dataset::PredefinedCityModelPackage::DisasterRisk: return DisasterRisk;
        case plateau::dataset::PredefinedCityModelPackage::LandUse: return LandUse;
        case plateau::dataset::PredefinedCityModelPackage::UrbanPlanningDecision: return UrbanPlanningDecision;
        case plateau::dataset::PredefinedCityModelPackage::Railway: return Railway;
        case plateau::dataset::PredefinedCityModelPackage::Waterway: return Waterway;
        case plateau::dataset::PredefinedCityModelPackage::WaterBody: return WaterBody;
        case plateau::dataset::PredefinedCityModelPackage::Bridge: return Bridge;
        case plateau::dataset::PredefinedCityModelPackage::Track: return Track;
        case plateau::dataset::PredefinedCityModelPackage::Square: return Square;
        case plateau::dataset::PredefinedCityModelPackage::Tunnel: return Tunnel;
        case plateau::dataset::PredefinedCityModelPackage::UndergroundFacility: return UndergroundFacility;
        case plateau::dataset::PredefinedCityModelPackage::UndergroundBuilding: return UndergroundBuilding;
        case plateau::dataset::PredefinedCityModelPackage::Area: return Area;
        case plateau::dataset::PredefinedCityModelPackage::OtherConstruction: return OtherConstruction;
        case plateau::dataset::PredefinedCityModelPackage::Generic: return Generic;
        case plateau::dataset::PredefinedCityModelPackage::Unknown: return Unknown;
        default: return Unknown;
        }
    }
    
    static FName GetFeaturePlacementSettingsPropertyName(plateau::dataset::PredefinedCityModelPackage Package) {
        switch (Package) {
        case plateau::dataset::PredefinedCityModelPackage::Building: return GET_MEMBER_NAME_CHECKED(UPLATEAUImportSettings, Building);
        case plateau::dataset::PredefinedCityModelPackage::Road: return GET_MEMBER_NAME_CHECKED(UPLATEAUImportSettings, Road);
        case plateau::dataset::PredefinedCityModelPackage::Vegetation: return GET_MEMBER_NAME_CHECKED(UPLATEAUImportSettings, Vegetation);
        case plateau::dataset::PredefinedCityModelPackage::CityFurniture: return GET_MEMBER_NAME_CHECKED(UPLATEAUImportSettings, CityFurniture);
        case plateau::dataset::PredefinedCityModelPackage::Relief: return GET_MEMBER_NAME_CHECKED(UPLATEAUImportSettings, Relief);
        case plateau::dataset::PredefinedCityModelPackage::DisasterRisk: return GET_MEMBER_NAME_CHECKED(UPLATEAUImportSettings, DisasterRisk);
        case plateau::dataset::PredefinedCityModelPackage::LandUse: return GET_MEMBER_NAME_CHECKED(UPLATEAUImportSettings, LandUse);
        case plateau::dataset::PredefinedCityModelPackage::UrbanPlanningDecision: return GET_MEMBER_NAME_CHECKED(UPLATEAUImportSettings, UrbanPlanningDecision);
        case plateau::dataset::PredefinedCityModelPackage::Railway: return GET_MEMBER_NAME_CHECKED(UPLATEAUImportSettings, Railway);
        case plateau::dataset::PredefinedCityModelPackage::Waterway: return GET_MEMBER_NAME_CHECKED(UPLATEAUImportSettings, Waterway);
        case plateau::dataset::PredefinedCityModelPackage::WaterBody: return GET_MEMBER_NAME_CHECKED(UPLATEAUImportSettings, WaterBody);
        case plateau::dataset::PredefinedCityModelPackage::Bridge: return GET_MEMBER_NAME_CHECKED(UPLATEAUImportSettings, Bridge);
        case plateau::dataset::PredefinedCityModelPackage::Track: return GET_MEMBER_NAME_CHECKED(UPLATEAUImportSettings, Track);
        case plateau::dataset::PredefinedCityModelPackage::Square: return GET_MEMBER_NAME_CHECKED(UPLATEAUImportSettings, Square);
        case plateau::dataset::PredefinedCityModelPackage::Tunnel: return GET_MEMBER_NAME_CHECKED(UPLATEAUImportSettings, Tunnel);
        case plateau::dataset::PredefinedCityModelPackage::UndergroundFacility: return GET_MEMBER_NAME_CHECKED(UPLATEAUImportSettings, UndergroundFacility);
        case plateau::dataset::PredefinedCityModelPackage::UndergroundBuilding: return GET_MEMBER_NAME_CHECKED(UPLATEAUImportSettings, UndergroundBuilding);
        case plateau::dataset::PredefinedCityModelPackage::Area: return GET_MEMBER_NAME_CHECKED(UPLATEAUImportSettings, Area);
        case plateau::dataset::PredefinedCityModelPackage::OtherConstruction: return GET_MEMBER_NAME_CHECKED(UPLATEAUImportSettings, OtherConstruction);
        case plateau::dataset::PredefinedCityModelPackage::Generic: return GET_MEMBER_NAME_CHECKED(UPLATEAUImportSettings, Generic);
        case plateau::dataset::PredefinedCityModelPackage::Unknown: return GET_MEMBER_NAME_CHECKED(UPLATEAUImportSettings, Unknown);
        default: return GET_MEMBER_NAME_CHECKED(UPLATEAUImportSettings, Unknown);
        }
    }
    
    static TArray<plateau::dataset::PredefinedCityModelPackage> GetAllPackages() {
        return {
            plateau::dataset::PredefinedCityModelPackage::Building,
            plateau::dataset::PredefinedCityModelPackage::Road,
            plateau::dataset::PredefinedCityModelPackage::Vegetation,
            plateau::dataset::PredefinedCityModelPackage::CityFurniture,
            plateau::dataset::PredefinedCityModelPackage::Relief,
            plateau::dataset::PredefinedCityModelPackage::DisasterRisk,
            plateau::dataset::PredefinedCityModelPackage::LandUse,
            plateau::dataset::PredefinedCityModelPackage::UrbanPlanningDecision,
            plateau::dataset::PredefinedCityModelPackage::Railway,
            plateau::dataset::PredefinedCityModelPackage::Waterway,
            plateau::dataset::PredefinedCityModelPackage::WaterBody,
            plateau::dataset::PredefinedCityModelPackage::Bridge,
            plateau::dataset::PredefinedCityModelPackage::Track,
            plateau::dataset::PredefinedCityModelPackage::Square,
            plateau::dataset::PredefinedCityModelPackage::Tunnel,
            plateau::dataset::PredefinedCityModelPackage::UndergroundFacility,
            plateau::dataset::PredefinedCityModelPackage::UndergroundBuilding,
            plateau::dataset::PredefinedCityModelPackage::Area,
            plateau::dataset::PredefinedCityModelPackage::OtherConstruction,
            plateau::dataset::PredefinedCityModelPackage::Generic,
            plateau::dataset::PredefinedCityModelPackage::Unknown,
        };
    }

    static TMap<EPLATEAUMeshGranularity, FText> GetGranularityTexts() {
        TMap<EPLATEAUMeshGranularity, FText> Items;
        Items.Add(EPLATEAUMeshGranularity::PerAtomicFeatureObject, LOCTEXT("AtomicFeatureObject", "最小地物単位"));
        Items.Add(EPLATEAUMeshGranularity::PerPrimaryFeatureObject, LOCTEXT("PrimaryFeatureObject", "主要地物単位"));
        Items.Add(EPLATEAUMeshGranularity::PerCityModelArea, LOCTEXT("CityModelArea", "地域単位"));
        return Items;
    }
    
    static plateau::polygonMesh::MeshGranularity ConvertGranularity(EPLATEAUMeshGranularity Value) {
        // TODO: 共通化
        switch (Value) {
        case EPLATEAUMeshGranularity::PerAtomicFeatureObject:
            return plateau::polygonMesh::MeshGranularity::PerAtomicFeatureObject;            
        case EPLATEAUMeshGranularity::PerPrimaryFeatureObject:
            return plateau::polygonMesh::MeshGranularity::PerPrimaryFeatureObject;
        case EPLATEAUMeshGranularity::PerCityModelArea:
            return plateau::polygonMesh::MeshGranularity::PerCityModelArea;
        }

        return plateau::polygonMesh::MeshGranularity::PerPrimaryFeatureObject;
    }

    static TMap<int64, FText> GetCategoryNames() {
        TMap<int64, FText> Items;
        Items.Add(static_cast<int64>(plateau::dataset::PredefinedCityModelPackage::Building), LOCTEXT("CategoryBuilding", "建築物"));
        Items.Add(static_cast<int64>(plateau::dataset::PredefinedCityModelPackage::Road), LOCTEXT("CategoryRoad", "道路"));
        Items.Add(static_cast<int64>(plateau::dataset::PredefinedCityModelPackage::Vegetation), LOCTEXT("CategoryVegetation", "植生"));
        Items.Add(static_cast<int64>(plateau::dataset::PredefinedCityModelPackage::CityFurniture), LOCTEXT("CategoryCityFurniture", "都市設備"));
        Items.Add(static_cast<int64>(plateau::dataset::PredefinedCityModelPackage::Relief), LOCTEXT("CategoryRelief", "起伏"));
        Items.Add(static_cast<int64>(plateau::dataset::PredefinedCityModelPackage::DisasterRisk), LOCTEXT("CategoryDisasterRisk", "災害リスク"));
        Items.Add(static_cast<int64>(plateau::dataset::PredefinedCityModelPackage::LandUse), LOCTEXT("CategoryLandUse", "土地利用"));
        Items.Add(static_cast<int64>(plateau::dataset::PredefinedCityModelPackage::UrbanPlanningDecision), LOCTEXT("CategoryUrbanPlanningDecision", "都市計画決定情報"));
        Items.Add(static_cast<int64>(plateau::dataset::PredefinedCityModelPackage::Railway), LOCTEXT("CategoryRailway", "鉄道"));
        Items.Add(static_cast<int64>(plateau::dataset::PredefinedCityModelPackage::Waterway), LOCTEXT("CategoryWaterway", "航路"));
        Items.Add(static_cast<int64>(plateau::dataset::PredefinedCityModelPackage::WaterBody), LOCTEXT("CategoryWaterBody", "水部"));
        Items.Add(static_cast<int64>(plateau::dataset::PredefinedCityModelPackage::Bridge), LOCTEXT("CategoryBridge", "橋梁"));
        Items.Add(static_cast<int64>(plateau::dataset::PredefinedCityModelPackage::Track), LOCTEXT("CategoryTrack", "徒歩道"));
        Items.Add(static_cast<int64>(plateau::dataset::PredefinedCityModelPackage::Square), LOCTEXT("CategorySquare", "広場"));
        Items.Add(static_cast<int64>(plateau::dataset::PredefinedCityModelPackage::Tunnel), LOCTEXT("CategoryTunnel", "トンネル"));
        Items.Add(static_cast<int64>(plateau::dataset::PredefinedCityModelPackage::UndergroundFacility), LOCTEXT("CategoryUndergroundFacility", "地下埋設物"));
        Items.Add(static_cast<int64>(plateau::dataset::PredefinedCityModelPackage::UndergroundBuilding), LOCTEXT("CategoryUndergroundBuilding", "地下街"));
        Items.Add(static_cast<int64>(plateau::dataset::PredefinedCityModelPackage::Area), LOCTEXT("CategoryArea", "区域"));
        Items.Add(static_cast<int64>(plateau::dataset::PredefinedCityModelPackage::OtherConstruction), LOCTEXT("CategoryOtherConstruction", "その他の構造物"));
        Items.Add(static_cast<int64>(plateau::dataset::PredefinedCityModelPackage::Generic), LOCTEXT("CategoryGeneric", "汎用都市"));
        Items.Add(static_cast<int64>(plateau::dataset::PredefinedCityModelPackage::Unknown), LOCTEXT("CategoryUnknown", "その他"));
        return Items;
    }

    static TMap<int64, FText> GetFilteringNames() {
        TMap<int64, FText> Items;
        Items.Add(static_cast<int64>(plateau::dataset::PredefinedCityModelPackage::Building), LOCTEXT("FilteringBuilding", "建造物(Building)"));
        Items.Add(static_cast<int64>(plateau::dataset::PredefinedCityModelPackage::Road), LOCTEXT("FilteringRoad", "道路 (Road)"));
        Items.Add(static_cast<int64>(plateau::dataset::PredefinedCityModelPackage::Vegetation), LOCTEXT("FilteringVegetation", "植生(Vegetation)"));
        Items.Add(static_cast<int64>(plateau::dataset::PredefinedCityModelPackage::CityFurniture), LOCTEXT("FilteringCityFurniture", "都市設備 (CityFurniture)"));
        Items.Add(static_cast<int64>(plateau::dataset::PredefinedCityModelPackage::Relief), LOCTEXT("FilteringRelief", "土地起伏(Relief)"));
        Items.Add(static_cast<int64>(plateau::dataset::PredefinedCityModelPackage::DisasterRisk), LOCTEXT("FilteringDisasterRisk", "災害リスク (DisasterRisk)"));
        Items.Add(static_cast<int64>(plateau::dataset::PredefinedCityModelPackage::LandUse), LOCTEXT("FilteringLandUse", "土地利用 (LandUse)"));
        Items.Add(static_cast<int64>(plateau::dataset::PredefinedCityModelPackage::UrbanPlanningDecision), LOCTEXT("FilteringUrbanPlanningDecision", "都市計画決定情報 (UrbanPlanningDecision)"));
        Items.Add(static_cast<int64>(plateau::dataset::PredefinedCityModelPackage::Railway), LOCTEXT("FilteringRailway", "鉄道(Railway)"));
        Items.Add(static_cast<int64>(plateau::dataset::PredefinedCityModelPackage::Waterway), LOCTEXT("FilteringWaterway", "航路(Waterway)"));
        Items.Add(static_cast<int64>(plateau::dataset::PredefinedCityModelPackage::WaterBody), LOCTEXT("FilteringWaterBody", "水部(WaterBody)"));
        Items.Add(static_cast<int64>(plateau::dataset::PredefinedCityModelPackage::Bridge), LOCTEXT("FilteringBridge", "橋梁(Bridge)"));
        Items.Add(static_cast<int64>(plateau::dataset::PredefinedCityModelPackage::Track), LOCTEXT("FilteringTrack", "徒歩道(Track)"));
        Items.Add(static_cast<int64>(plateau::dataset::PredefinedCityModelPackage::Square), LOCTEXT("FilteringSquare", "広場(Square)"));
        Items.Add(static_cast<int64>(plateau::dataset::PredefinedCityModelPackage::Tunnel), LOCTEXT("FilteringTunnel", "トンネル(Tunnel)"));
        Items.Add(static_cast<int64>(plateau::dataset::PredefinedCityModelPackage::UndergroundFacility), LOCTEXT("FilteringUndergroundFacility", "地下埋設物(UndergroundFacility)"));
        Items.Add(static_cast<int64>(plateau::dataset::PredefinedCityModelPackage::UndergroundBuilding), LOCTEXT("FilteringUndergroundBuilding", "地下街(UndergroundBuilding)"));
        Items.Add(static_cast<int64>(plateau::dataset::PredefinedCityModelPackage::Area), LOCTEXT("FilteringArea", "区域(Area)"));
        Items.Add(static_cast<int64>(plateau::dataset::PredefinedCityModelPackage::OtherConstruction), LOCTEXT("FilteringOtherConstruction", "その他の構造物(OtherConstruction)"));
        Items.Add(static_cast<int64>(plateau::dataset::PredefinedCityModelPackage::Generic), LOCTEXT("FilteringGeneric", "汎用都市(Generic)"));
        Items.Add(static_cast<int64>(plateau::dataset::PredefinedCityModelPackage::Unknown), LOCTEXT("FilteringUnknown", "その他 (Unknown)"));
        return Items;
    }

    static FString GetDefaultFallbackMaterialName(const int64 Package) {
        const auto Pkg = static_cast<plateau::dataset::PredefinedCityModelPackage>(Package);
        switch (Pkg) {
        case plateau::dataset::PredefinedCityModelPackage::Building: return "PlateauDefaultBuildingMaterialInstance";
        case plateau::dataset::PredefinedCityModelPackage::Road: return "PlateauDefaultRoadMaterialInstance";
        case plateau::dataset::PredefinedCityModelPackage::UrbanPlanningDecision: return "PlateauDefaultUrbanPlanningDecisionMaterialInstance";
        case plateau::dataset::PredefinedCityModelPackage::LandUse: return "PlateauDefaultLandUseMaterialInstance";
        case plateau::dataset::PredefinedCityModelPackage::CityFurniture: return "PlateauDefaultCityFurnitureMaterialInstance";
        case plateau::dataset::PredefinedCityModelPackage::Vegetation: return "PlateauDefaultVegetationMaterialInstance"; 
        case plateau::dataset::PredefinedCityModelPackage::Relief: return "PlateauDefaultReliefMaterialInstance";
        case plateau::dataset::PredefinedCityModelPackage::DisasterRisk: return "PlateauDefaultDisasterMaterialInstance";
        case plateau::dataset::PredefinedCityModelPackage::Railway: return "PlateauDefaultRailwayMaterialInstance";
        case plateau::dataset::PredefinedCityModelPackage::Waterway: return "PlateauDefaultWaterwayMaterialInstance";
        case plateau::dataset::PredefinedCityModelPackage::WaterBody: return "PlateauDefaultWaterBodyMaterialInstance";
        case plateau::dataset::PredefinedCityModelPackage::Bridge: return "PlateauDefaultBridgeMaterialInstance";
        case plateau::dataset::PredefinedCityModelPackage::Track: return "PlateauDefaultTrackMaterialInstance";
        case plateau::dataset::PredefinedCityModelPackage::Square: return "PlateauDefaultSquareMaterialInstance";
        case plateau::dataset::PredefinedCityModelPackage::Tunnel: return "PlateauDefaultTunnelMaterialInstance";
        case plateau::dataset::PredefinedCityModelPackage::UndergroundFacility: return "PlateauDefaultUndergroundFacilityMaterialInstance";
        case plateau::dataset::PredefinedCityModelPackage::UndergroundBuilding: return "PlateauDefaultUndergroundBuildingMaterialInstance";
        case plateau::dataset::PredefinedCityModelPackage::Area: return "PlateauDefaultLandUseMaterialInstance";    // 土地利用を流用
        case plateau::dataset::PredefinedCityModelPackage::Unknown: return "PlateauDefaultUnknownMaterialInstance";
        default: return "";
        }
    }
};

#undef LOCTEXT_NAMESPACE
