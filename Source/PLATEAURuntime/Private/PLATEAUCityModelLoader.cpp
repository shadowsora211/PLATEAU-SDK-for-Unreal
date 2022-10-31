// Fill out your copyright notice in the Description page of Project Settings.


#include "PLATEAUCityModelLoader.h"

#include "PLATEAUInstancedCityModel.h"
#include "plateau/udx/udx_file_collection.h"
#include "plateau/polygon_mesh/mesh_extractor.h"
#include "plateau/polygon_mesh/mesh_extract_options.h"
#include "PLATEAUMeshLoader.h"
#include "citygml/citygml.h"

using namespace plateau::udx;
using namespace plateau::polygonMesh;

APLATEAUCityModelLoader::APLATEAUCityModelLoader() {
    PrimaryActorTick.bCanEverTick = false;
}

void APLATEAUCityModelLoader::Load() {
#if WITH_EDITOR
    UE_LOG(LogTemp, Log, TEXT("-----Load Start-----"));

    // GeoReferenceを選択範囲の中心に更新
    const auto MinPoint = GeoReference.GetData().project(Extent.GetNativeData().min);
    const auto MaxPoint = GeoReference.GetData().project(Extent.GetNativeData().max);
    const auto NativeReferencePoint = (MinPoint + MaxPoint) / 2.0;
    GeoReference.ReferencePoint.X += NativeReferencePoint.x;
    GeoReference.ReferencePoint.Y += NativeReferencePoint.y;
    GeoReference.ReferencePoint.Z += NativeReferencePoint.z;

    LoadAsync();
#endif
}

void APLATEAUCityModelLoader::LoadAsync() {
    // アクター生成
    APLATEAUInstancedCityModel* ModelActor = GetWorld()->SpawnActor<APLATEAUInstancedCityModel>();
    CreateRootComponent(*ModelActor);
    ModelActor->SetActorLabel(FPaths::GetCleanFilename(Source));
    ModelActor->GeoReference = GeoReference;

    for (const auto& Package : UPLATEAUImportSettings::GetAllPackages()) {
        const auto Settings = ImportSettings->GetFeatureSettings(Package);
        if (!Settings.bImport)
            continue;

        // キャプチャ用ローカル変数
        const auto& GeoReferenceData = GeoReference.GetData();
        MeshExtractOptions ExtractOptions;
        ExtractOptions.reference_point = GeoReferenceData.getReferencePoint();
        ExtractOptions.mesh_axes = plateau::geometry::CoordinateSystem::NWU;
        ExtractOptions.coordinate_zone_id = GeoReferenceData.getZoneID();
        ExtractOptions.mesh_granularity = UPLATEAUImportSettings::ConvertGranularity(Settings.MeshGranularity);
        ExtractOptions.max_lod = Settings.MaxLod;
        ExtractOptions.min_lod = Settings.MinLod;
        ExtractOptions.export_appearance = Settings.bImportTexture;
        ExtractOptions.grid_count_of_side = 10;
        ExtractOptions.unit_scale = 0.01f;
        ExtractOptions.extent = Extent.GetNativeData();

        // 都市モデルパース、ポリゴンメッシュ抽出、ノード走査 各ファイルに対して行う
        citygml::ParserParams ParserParams;
        ParserParams.tesselate = true;

        Async(EAsyncExecution::Thread,
            [
                Package,
                Source = Source,
                ExtentData = Extent.GetNativeData(),
                ModelActor,
                ExtractOptions
            ]{
                // ファイル検索
                const auto UdxFileCollection =
                    UdxFileCollection::find(TCHAR_TO_UTF8(*Source))
                    ->filter(ExtentData);
                const auto GmlFiles = UdxFileCollection->getGmlFiles(Package);

                // 都市モデルパース、ポリゴンメッシュ抽出、ノード走査 各ファイルに対して行う
                citygml::ParserParams ParserParams;
                ParserParams.tesselate = true;

                for (const auto& GmlFile : *GmlFiles) {
                    const auto Destination = TCHAR_TO_UTF8(*(FPaths::ProjectContentDir() + "PLATEAU"));
                    std::string CopiedGmlPath;
                    try {
                        CopiedGmlPath = UdxFileCollection->fetch(Destination, GmlFileInfo(GmlFile));
                    }
                    catch (...) {
                        //TODO: Error Handling
                        UE_LOG(LogTemp, Error, TEXT("Failed to copy %s"), GmlFile.c_str());

                        continue;
                    }

                    std::shared_ptr<const citygml::CityModel> CityModel;
                    try {
                        CityModel = citygml::load(CopiedGmlPath, ParserParams);
                    }
                    catch (...) {
                        //TODO: Error Handling
                        UE_LOG(LogTemp, Error, TEXT("Failed to parse %s"), GmlFile.c_str());

                        continue;
                    }
                    const auto Model = MeshExtractor::extract(*CityModel, ExtractOptions);

                    USceneComponent* GmlRootComponent;
                    const FGraphEventRef Task = FFunctionGraphTask::CreateAndDispatchWhenReady(
                        [&GmlRootComponent, &ModelActor, &GmlFile] {
                            GmlRootComponent = NewObject<USceneComponent>(ModelActor, NAME_None);
                            // コンポーネント名設定(拡張子無しgml名)
                            const auto DesiredName = FPaths::GetBaseFilename(UTF8_TO_TCHAR(GmlFile.c_str()));
                            FString NewUniqueName = DesiredName;
                            if (!GmlRootComponent->Rename(*NewUniqueName, nullptr, REN_Test)) {
                                NewUniqueName = MakeUniqueObjectName(ModelActor, USceneComponent::StaticClass(), FName(DesiredName)).ToString();
                            }
                            GmlRootComponent->Rename(*NewUniqueName, nullptr, REN_DontCreateRedirectors);

                            check(GmlRootComponent != nullptr);
                            GmlRootComponent->Mobility = EComponentMobility::Static;
                            ModelActor->AddInstanceComponent(GmlRootComponent);
                            GmlRootComponent->RegisterComponent();
                            GmlRootComponent->AttachToComponent(ModelActor->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);

                    }, TStatId(), nullptr, ENamedThreads::GameThread);
                    Task->Wait();

                    FPLATEAUMeshLoader().LoadModel(ModelActor, GmlRootComponent, Model);
                }
            });
    }
}

void APLATEAUCityModelLoader::BeginPlay() {
    Super::BeginPlay();

}

void APLATEAUCityModelLoader::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);
}

void APLATEAUCityModelLoader::CreateRootComponent(AActor& Actor) {
    USceneComponent* ActorRootComponent = NewObject<USceneComponent>(&Actor,
        USceneComponent::GetDefaultSceneRootVariableName());

    check(ActorRootComponent != nullptr);
    ActorRootComponent->Mobility = EComponentMobility::Static;
    ActorRootComponent->bVisualizeComponent = true;
    Actor.SetRootComponent(ActorRootComponent);
    Actor.AddInstanceComponent(ActorRootComponent);
    ActorRootComponent->RegisterComponent();
    Actor.SetFlags(RF_Transactional);
    ActorRootComponent->SetFlags(RF_Transactional);
    GEngine->BroadcastLevelActorListChanged();
}
