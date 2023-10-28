// Copyright © 2023 Ministry of Land, Infrastructure and Transport

#include "PLATEAUExtentEditorVPClient.h"
#include "PLATEAUEditor/Public/ExtentEditor/PLATEAUExtentEditor.h"

#include <plateau/dataset/i_dataset_accessor.h>

#include "PLATEAUMeshCodeGizmo.h"
#include "PLATEAUFeatureInfoDisplay.h"

#include "EditorModeManager.h"
#include "CanvasTypes.h"

#include "SEditorViewport.h"
#include "AdvancedPreviewScene.h"
#include "SPLATEAUExtentEditorViewport.h"

#include "AssetViewerSettings.h"
#include "CameraController.h"
#include "EditorViewportClient.h"
#include "PLATEAUImportSettings.h"

#define LOCTEXT_NAMESPACE "FPLATEAUExtentEditorViewportClient"

DECLARE_STATS_GROUP(TEXT("PLATEAUExtentEditor"), STATGROUP_PLATEAUExtentEditor, STATCAT_PLATEAUSDK);
DECLARE_CYCLE_STAT(TEXT("FeatureInfoDisplay"), STAT_FeatureInfoDisplay, STATGROUP_PLATEAUExtentEditor);

namespace {
    /**
     * @brief 最大パネル読み込み並列数
     */
    constexpr int MaxLoadPanelParallelCount = 8;

    /**
     * @brief 最大パネル追加並列数
     */
    constexpr int MaxAddComponentParallelCount = 8;
}


FPLATEAUExtentEditorViewportClient::FPLATEAUExtentEditorViewportClient(TWeakPtr<FPLATEAUExtentEditor> InExtentEditor,
                                                                       const TSharedRef<SPLATEAUExtentEditorViewport>& InPLATEAUExtentEditorViewport,
                                                                       const TSharedRef<FAdvancedPreviewScene>& InPreviewScene) : FEditorViewportClient(
    nullptr, &InPreviewScene.Get(), StaticCastSharedRef<SEditorViewport>(InPLATEAUExtentEditorViewport)), ExtentEditorPtr(InExtentEditor) {
    InPreviewScene->SetFloorVisibility(false);
}

FPLATEAUExtentEditorViewportClient::~FPLATEAUExtentEditorViewportClient() {
    UAssetViewerSettings::Get()->OnAssetViewerSettingsChanged().RemoveAll(this);
}

void FPLATEAUExtentEditorViewportClient::Initialize(std::shared_ptr<plateau::dataset::IDatasetAccessor> InDatasetAccessor) {
    DatasetAccessor = InDatasetAccessor;
    const auto& MeshCodes = DatasetAccessor->getMeshCodes();
    const auto ExtentEditor = ExtentEditorPtr.Pin();

    InitCamera();

    // 範囲選択ソースが変更された場合はメッシュコードの選択状態をリセット
    const auto& AreaSourcePath = ExtentEditor->IsImportFromServer() ? UTF8_TO_TCHAR(ExtentEditor->GetServerDatasetID().c_str()) : ExtentEditor->GetSourcePath();
    if (ExtentEditor->GetAreaSourcePath() != AreaSourcePath) {
        ExtentEditor->SetAreaSourcePath(AreaSourcePath);
        ExtentEditor->ResetAreaMeshCodeMap();
    }

    // メッシュコードギズモ生成と選択状態復帰
    auto GeoReference = ExtentEditor->GetGeoReference();
    MeshCodeGizmos.Reset();
    for (const auto& MeshCode : MeshCodes) {
        // 2次メッシュ以下の次数は省く
        if (MeshCode.getLevel() <= 2)
            continue;

        MeshCodeGizmos.AddDefaulted();
        MeshCodeGizmos.Last().Init(MeshCode, GeoReference.GetData());
        if (ExtentEditor->GetAreaMeshCodeMap().Contains(UTF8_TO_TCHAR(MeshCode.get().c_str()))) {
            MeshCodeGizmos.Last().SetbSelectedArray(ExtentEditor->GetAreaMeshCodeMap()[UTF8_TO_TCHAR(MeshCode.get().c_str())].GetbSelectedArray());
        }
    }
}

void FPLATEAUExtentEditorViewportClient::ResetSelectedArea() {
    for (auto& Gizmo : MeshCodeGizmos) {
        Gizmo.ResetSelectedArea();
        ExtentEditorPtr.Pin()->SetAreaMeshCodeMap(Gizmo.GetRegionMeshID(), Gizmo);
    }
}

void FPLATEAUExtentEditorViewportClient::InitCamera() {
    ToggleOrbitCamera(false);
    SetCameraSetup(FVector::ZeroVector, FRotator::ZeroRotator, FVector(0.0, 0, 8000.0), FVector::Zero(), FVector(0, 0, 8000.0), FRotator(-90, -90, 0));
    CameraController->AccessConfig().bLockedPitch = true;
    CameraController->AccessConfig().MaximumAllowedPitchRotation = -90;
    CameraController->AccessConfig().MinimumAllowedPitchRotation = -90;
}

void FPLATEAUExtentEditorViewportClient::Tick(float DeltaSeconds) {
    if (DatasetAccessor == nullptr)
        return;

    FPLATEAUMeshCodeGizmo::SetShowLevel5Mesh(GetViewTransform().GetLocation().Z < 10000.0);

    // ベースマップ
    const auto ExtentEditor = ExtentEditorPtr.Pin();
    auto GeoReference = ExtentEditor->GetGeoReference();
    if (Basemap == nullptr) {
        Basemap = MakeUnique<FPLATEAUBasemap>(GeoReference, SharedThis(this));
    }

    TArray<FVector> CornerWorldPositions;
    CornerWorldPositions.Add(GetWorldPosition(0, 0));
    CornerWorldPositions.Add(GetWorldPosition(0, Viewport->GetSizeXY().Y));
    CornerWorldPositions.Add(GetWorldPosition(Viewport->GetSizeXY().X, 0));
    CornerWorldPositions.Add(GetWorldPosition(Viewport->GetSizeXY().X, Viewport->GetSizeXY().Y));

    FVector MinPosition = CornerWorldPositions[0];
    FVector MaxPosition = CornerWorldPositions[1];
    for (int i = 1; i < 4; ++i) {
        MinPosition.X = FMath::Min(MinPosition.X, CornerWorldPositions[i].X);
        MinPosition.Y = FMath::Min(MinPosition.Y, CornerWorldPositions[i].Y);
        MaxPosition.X = FMath::Max(MaxPosition.X, CornerWorldPositions[i].X);
        MaxPosition.Y = FMath::Max(MaxPosition.Y, CornerWorldPositions[i].Y);
    }

    const TVec3d RawMinPosition(MinPosition.X, MinPosition.Y, MinPosition.Z);
    const TVec3d RawMaxPosition(MaxPosition.X, MaxPosition.Y, MaxPosition.Z);

    auto MinCoordinate = GeoReference.GetData().unproject(RawMinPosition);
    auto MaxCoordinate = GeoReference.GetData().unproject(RawMaxPosition);

    // Unproject後の最小最大を再計算
    const auto TempMinCoordinate = MinCoordinate;
    MinCoordinate.latitude = FMath::Min(TempMinCoordinate.latitude, MaxCoordinate.latitude);
    MinCoordinate.longitude = FMath::Min(TempMinCoordinate.longitude, MaxCoordinate.longitude);
    MaxCoordinate.latitude = FMath::Max(TempMinCoordinate.latitude, MaxCoordinate.latitude);
    MaxCoordinate.longitude = FMath::Max(TempMinCoordinate.longitude, MaxCoordinate.longitude);

    Extent = FPLATEAUExtent(plateau::geometry::Extent(MinCoordinate, MaxCoordinate));

    Basemap->UpdateAsync(Extent, DeltaSeconds);

    // 視点移動
    if (IsCameraMoving) {
        SetViewLocation(TrackingStartedCameraPosition);
        FVector CurrentCursorPosition;
        if (!TryGetWorldPositionOfCursor(CurrentCursorPosition))
            return;

        const auto Offset = CurrentCursorPosition - TrackingStartedPosition;

        SetViewLocation(TrackingStartedCameraPosition - Offset);
    }
}

void FPLATEAUExtentEditorViewportClient::Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI) {
    FEditorViewportClient::Draw(View, PDI);

    for (const auto& Gizmo : MeshCodeGizmos) {
        // 範囲内のギズモのみ描画
        if (GizmoContains(Gizmo)) {
            Gizmo.DrawExtent(View, PDI);
        }
    }

    if (IsLeftMouseButtonMoved || IsLeftMouseAndShiftButtonMoved) {
        // ドラッグ中に表示する範囲枠線
        const FBox Box(FVector(TrackingStartedPosition.X, TrackingStartedPosition.Y, 0), FVector(CachedWorldMousePos.X, CachedWorldMousePos.Y, 0));
        DrawWireBox(PDI, Box, IsLeftMouseButtonMoved ? FColor::White : FColor::Red, SDPG_World, 3.0f, 0, true);
    }
}

void FPLATEAUExtentEditorViewportClient::DrawCanvas(FViewport& InViewport, FSceneView& View, FCanvas& Canvas) {
    const double CameraDistance = GetViewTransform().GetLocation().Z;
    // 地物アイコン
    if (FeatureInfoDisplay == nullptr) {
        FeatureInfoDisplay = MakeShared<FPLATEAUFeatureInfoDisplay>(ExtentEditorPtr.Pin().Get()->GetGeoReference(), SharedThis(this));
    }

    int LoadingPanelCnt = FeatureInfoDisplay->CountLoadingPanels();
    int AddComponentCnt = 0;
    for (const auto& Gizmo : MeshCodeGizmos) {
        // 範囲内のギズモのみ描画
        if (GizmoContains(Gizmo)) {
            if (CameraDistance < 4000.0) {
                FeatureInfoDisplay->SetVisibility(Gizmo, EPLATEAUFeatureInfoVisibility::Detailed);
                if (LoadingPanelCnt < MaxLoadPanelParallelCount && FeatureInfoDisplay->CreatePanelAsync(Gizmo, *DatasetAccessor))
                    LoadingPanelCnt++;
                if (AddComponentCnt < MaxAddComponentParallelCount && FeatureInfoDisplay->AddComponent(Gizmo))
                    AddComponentCnt++;
            } else if (CameraDistance < 9000.0) {
                FeatureInfoDisplay->SetVisibility(Gizmo, EPLATEAUFeatureInfoVisibility::Visible);
                if (LoadingPanelCnt < MaxLoadPanelParallelCount && FeatureInfoDisplay->CreatePanelAsync(Gizmo, *DatasetAccessor))
                    LoadingPanelCnt++;
                if (AddComponentCnt < MaxAddComponentParallelCount && FeatureInfoDisplay->AddComponent(Gizmo))
                    AddComponentCnt++;
            } else {
                FeatureInfoDisplay->SetVisibility(Gizmo, EPLATEAUFeatureInfoVisibility::Hidden);
            }

            if (const auto ItemCount = FeatureInfoDisplay.Get()->GetItemCount(Gizmo.GetRegionMeshID()); 0 < ItemCount) {
                Gizmo.DrawRegionMeshID(InViewport, View, Canvas, Gizmo.GetRegionMeshID(), CameraDistance, ItemCount);
            }
        }
    }
}

void FPLATEAUExtentEditorViewportClient::TrackingStarted(const FInputEventState& InInputState, bool bIsDragging, bool bNudge) {
    if (!TryGetWorldPositionOfCursor(TrackingStartedPosition))
        return;

    TrackingStartedCameraPosition = GetViewLocation();
    if (InInputState.IsLeftMouseButtonPressed() && InInputState.IsShiftButtonPressed()) {
        IsLeftMouseAndShiftButtonPressed = true;
        CachedWorldMousePos = GetWorldPosition(CachedMouseX, CachedMouseY);
    } else if (InInputState.IsLeftMouseButtonPressed()) {
        IsLeftMouseButtonPressed = true;
        CachedWorldMousePos = GetWorldPosition(CachedMouseX, CachedMouseY);
    } else if (InInputState.IsRightMouseButtonPressed()) {
        IsCameraMoving = true;
    }
}

void FPLATEAUExtentEditorViewportClient::CapturedMouseMove(FViewport* InViewport, int32 InMouseX, int32 InMouseY) {
    if (IsLeftMouseButtonPressed) {
        IsLeftMouseButtonPressed = false;
        IsLeftMouseButtonMoved = true;
    } else if (IsLeftMouseAndShiftButtonPressed) {
        IsLeftMouseAndShiftButtonPressed = false;
        IsLeftMouseAndShiftButtonMoved = true; 
    }
    
    if (IsLeftMouseButtonMoved || IsLeftMouseAndShiftButtonMoved) {
        // 既存のカーソルを非表示
        SetRequiredCursor(true, false);
        SetRequiredCursorOverride(true, EMouseCursor::None);

        // カーソル移動状態を保持
        ApplyRequiredCursorVisibility(true);

        // マウス座標更新
        CachedWorldMousePos = GetWorldPosition(InMouseX, InMouseY);
    } else if (IsCameraMoving) {
        // グラブカーソル表示
        SetRequiredCursor(false, true);
        SetRequiredCursorOverride(true, EMouseCursor::GrabHand);

        // カーソル移動状態を保持
        ApplyRequiredCursorVisibility(true);
    }
}

void FPLATEAUExtentEditorViewportClient::TrackingStopped() {
    if (IsLeftMouseButtonPressed) {
        for (auto& Gizmo : MeshCodeGizmos) {
            CachedWorldMousePos = GetWorldPosition(CachedMouseX, CachedMouseY);
            Gizmo.ToggleSelectArea(CachedWorldMousePos.X, CachedWorldMousePos.Y);
            ExtentEditorPtr.Pin()->SetAreaMeshCodeMap(Gizmo.GetRegionMeshID(), Gizmo);
        }
    } else if (IsLeftMouseButtonMoved || IsLeftMouseAndShiftButtonMoved) {
        const auto bRightSideMousePosition = TrackingStartedPosition.X < CachedWorldMousePos.X;
        const auto MinX = bRightSideMousePosition ? TrackingStartedPosition.X : CachedWorldMousePos.X;
        const auto MaxX = bRightSideMousePosition ? CachedWorldMousePos.X : TrackingStartedPosition.X;

        const auto bUpperSideMousePosition = TrackingStartedPosition.Y < CachedWorldMousePos.Y;
        const auto MinY = bUpperSideMousePosition ? TrackingStartedPosition.Y : CachedWorldMousePos.Y;
        const auto MaxY = bUpperSideMousePosition ? CachedWorldMousePos.Y : TrackingStartedPosition.Y;

        const auto ExtentMin = FVector2d(MinX, MinY);
        const auto ExtentMax = FVector2d(MaxX, MaxY);
        
        for (auto& Gizmo : MeshCodeGizmos) {
            Gizmo.SetSelectArea(ExtentMin, ExtentMax, IsLeftMouseButtonMoved);
            ExtentEditorPtr.Pin()->SetAreaMeshCodeMap(Gizmo.GetRegionMeshID(), Gizmo);
        }
    }

    IsLeftMouseButtonPressed = false;
    IsLeftMouseAndShiftButtonPressed = false;
    IsLeftMouseButtonMoved = false;
    IsLeftMouseAndShiftButtonMoved = false;
    IsCameraMoving = false;
}

bool FPLATEAUExtentEditorViewportClient::ShouldScaleCameraSpeedByDistance() const {
    return true;
}

void FPLATEAUExtentEditorViewportClient::SwitchFeatureInfoDisplay(const int Lod, const bool bCheck) const {
    // 現在読み込まれているギズモ全てに対して表示するべきLodアイコンが切り替わったことを通知
    FeatureInfoDisplay->SwitchFeatureInfoDisplay(MeshCodeGizmos, Lod, bCheck);
}

bool FPLATEAUExtentEditorViewportClient::GizmoContains(const FPLATEAUMeshCodeGizmo Gizmo) const {
    const auto ExtentEditor = ExtentEditorPtr.Pin();
    const auto RawMin = ExtentEditor->GetGeoReference().GetData().project(Extent.GetNativeData().min);
    const auto RawMax = ExtentEditor->GetGeoReference().GetData().project(Extent.GetNativeData().max);
    const auto MinX = FGenericPlatformMath::Min(RawMin.x, RawMax.x);
    const auto MinY = FGenericPlatformMath::Min(RawMin.y, RawMax.y);
    const auto MaxX = FGenericPlatformMath::Max(RawMin.x, RawMax.x);
    const auto MaxY = FGenericPlatformMath::Max(RawMin.y, RawMax.y);

    return MinX <= Gizmo.GetMin().X + Gizmo.GetSize().X && Gizmo.GetMax().X - Gizmo.GetSize().X <= MaxX && MinY <= Gizmo.GetMin().Y + Gizmo.GetSize().Y && Gizmo.GetMax().Y - Gizmo.GetSize().Y <= MaxY;
}

FVector FPLATEAUExtentEditorViewportClient::GetWorldPosition(uint32 X, uint32 Y) {
    FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(Viewport, GetScene(), EngineShowFlags).SetRealtimeUpdate(IsRealtime()));

    const FSceneView* View = CalcSceneView(&ViewFamily);

    const auto Location = FViewportCursorLocation(View, this, X, Y);

    const FPlane Plane(FVector::ZeroVector, FVector::UpVector);
    const auto StartPoint = Location.GetOrigin();
    const auto EndPoint = Location.GetOrigin() + Location.GetDirection() * 100000.0;
    FVector Position;
    FMath::SegmentPlaneIntersection(StartPoint, EndPoint, Plane, Position);
    return Position;
}

bool FPLATEAUExtentEditorViewportClient::TryGetWorldPositionOfCursor(FVector& Position) {
    const auto CursorLocation = GetCursorWorldLocationFromMousePos();
    const FPlane Plane(FVector::ZeroVector, FVector::UpVector);
    const auto StartPoint = CursorLocation.GetOrigin();
    const auto EndPoint = CursorLocation.GetOrigin() + CursorLocation.GetDirection() * 100000.0;
    return FMath::SegmentPlaneIntersection(StartPoint, EndPoint, Plane, Position);
}

bool FPLATEAUExtentEditorViewportClient::SetViewLocationByMeshCode(FString meshCode) {
    const auto MeshCode = plateau::dataset::MeshCode(TCHAR_TO_UTF8(*meshCode));

    std::set<plateau::dataset::MeshCode> MeshCodes = DatasetAccessor->getMeshCodes();
    if (MeshCodes.find(MeshCode) == MeshCodes.end())
        return false;

    const auto ExtentEditor = ExtentEditorPtr.Pin();     
    const auto Box = ExtentEditor->GetBoxByExtent(MeshCode.getExtent());
    if (!Box.IsValid) return false;
    FocusViewportOnBox(Box, true);
    return true;
}


#undef LOCTEXT_NAMESPACE
