// Copyright © 2023 Ministry of Land, Infrastructure and Transport

#pragma once

#include "CoreMinimal.h"
#include "AdvancedPreviewSceneModule.h"
#include "PLATEAUGeometry.h"
#include "PLATEAUEditor/Private/PLATEAUFeatureImportSettingsDetails.h"
#include <plateau/network/client.h>

class FEditorViewportClient;
class SDockTab;
class FViewportTabContent;
class UPLATEAUSDKEditorUtilityWidget;

/**
 * @brief 範囲選択画面の表示、操作、情報取得、設定を行うためのインスタンスメソッドを提供します。
 *
 */
class PLATEAUEDITOR_API FPLATEAUExtentEditor : public TSharedFromThis<FPLATEAUExtentEditor> {
public:
    FPLATEAUExtentEditor(const TSharedRef<class FPLATEAUEditorStyle>& InStyle);
    ~FPLATEAUExtentEditor();

    static const FName TabId;

    void RegisterTabSpawner(const TSharedRef<class FTabManager>& TabManager);
    void UnregisterTabSpawner(const TSharedRef<class FTabManager>& TabManager);

    TSharedRef<SDockTab> SpawnTab(const FSpawnTabArgs& Args);

    const FString& GetSourcePath() const;
    void SetSourcePath(const FString& Path);

    FPLATEAUGeoReference GetGeoReference() const;
    void SetGeoReference(const FPLATEAUGeoReference& InGeoReference);

    const TOptional<FPLATEAUExtent>& GetExtent() const;
    void SetExtent(const FPLATEAUExtent& InExtent);
    void ResetExtent();

    TSharedRef<FPLATEAUEditorStyle> GetEditorStyle() const;

    const bool IsImportFromServer() const;
    void SetImportFromServer(bool InBool);

    std::shared_ptr<plateau::network::Client> GetClientPtr() const;
    void SetClientPtr(const std::shared_ptr<plateau::network::Client>& InClientPtr);

    const std::string& GetServerDatasetID() const;
    void SetServerDatasetID(const std::string& InID);

    const plateau::dataset::PredefinedCityModelPackage& GetLocalPackageMask() const;
    void SetLocalPackageMask(const plateau::dataset::PredefinedCityModelPackage& InPackageMask);

    const plateau::dataset::PredefinedCityModelPackage& GetServerPackageMask() const;
    void SetServerPackageMask(const plateau::dataset::PredefinedCityModelPackage& InPackageMask);
private:
    FString SourcePath;
    FPLATEAUGeoReference GeoReference;
    TOptional<FPLATEAUExtent> Extent;
    TSharedPtr<FPLATEAUEditorStyle> Style;

    bool bImportFromServer = false;
    std::shared_ptr<plateau::network::Client> ClientPtr;
    std::string ServerDatasetID;
    plateau::dataset::PredefinedCityModelPackage LocalPackageMask;
    plateau::dataset::PredefinedCityModelPackage ServerPackageMask;

    FAdvancedPreviewSceneModule::FOnPreviewSceneChanged OnPreviewSceneChangedDelegate;
    TWeakObjectPtr<class APLATEAUCityModelLoader> Loader;
};
