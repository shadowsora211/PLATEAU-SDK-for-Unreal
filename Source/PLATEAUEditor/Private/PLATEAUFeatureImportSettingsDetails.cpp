// Copyright Epic Games, Inc. All Rights Reserved.

#include "PLATEAUFeatureImportSettingsDetails.h"

#include <plateau/io/mesh_convert_options.h>
#include <plateau/udx/city_model_package.h>

#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "DetailWidgetRow.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Widgets/SBoxPanel.h"
#include "EditorFontGlyphs.h"
#include "Widgets/Input/SButton.h"
#include "Framework/Application/SlateApplication.h"

#include "PLATEAUImportSettings.h"

#define LOCTEXT_NAMESPACE "PLATEAUFeatureImportSettings"

using namespace plateau::udx;

namespace {
    TMap<EPLATEAUMeshGranularity, FText> GetGranularityTexts() {
        TMap<EPLATEAUMeshGranularity, FText> Items;
        Items.Add(EPLATEAUMeshGranularity::PerPrimaryFeatureObject, LOCTEXT("PrimaryFeatureObject", "主要地物単位"));
        Items.Add(EPLATEAUMeshGranularity::PerAtomicFeatureObject, LOCTEXT("AtomicFeatureObject", "最小地物単位"));
        Items.Add(EPLATEAUMeshGranularity::PerCityModelArea, LOCTEXT("CityModelArea", "地域単位"));
        return Items;
    }

    FName GetFeaturePlacementSettingsPropertyName(PredefinedCityModelPackage Package) {
        switch (Package) {
        case PredefinedCityModelPackage::Building: return GET_MEMBER_NAME_CHECKED(UPLATEAUImportSettings, Building);
        case PredefinedCityModelPackage::Road: return GET_MEMBER_NAME_CHECKED(UPLATEAUImportSettings, Road);
        case PredefinedCityModelPackage::Vegetation: return GET_MEMBER_NAME_CHECKED(UPLATEAUImportSettings, Vegetation);
        case PredefinedCityModelPackage::CityFurniture: return GET_MEMBER_NAME_CHECKED(UPLATEAUImportSettings, CityFurniture);
        case PredefinedCityModelPackage::Relief: return GET_MEMBER_NAME_CHECKED(UPLATEAUImportSettings, Relief);
        case PredefinedCityModelPackage::DisasterRisk: return GET_MEMBER_NAME_CHECKED(UPLATEAUImportSettings, DisasterRisk);
        case PredefinedCityModelPackage::LandUse: return GET_MEMBER_NAME_CHECKED(UPLATEAUImportSettings, LandUse);
        case PredefinedCityModelPackage::UrbanPlanningDecision: return GET_MEMBER_NAME_CHECKED(UPLATEAUImportSettings, UrbanPlanningDecision);
        case PredefinedCityModelPackage::Unknown: return GET_MEMBER_NAME_CHECKED(UPLATEAUImportSettings, Unknown);
        default: return GET_MEMBER_NAME_CHECKED(UPLATEAUImportSettings, Unknown);
        }
    }

    TArray<PredefinedCityModelPackage> GetAllPackages() {
        return {
            PredefinedCityModelPackage::Building,
            PredefinedCityModelPackage::Road,
            PredefinedCityModelPackage::Vegetation,
            PredefinedCityModelPackage::CityFurniture,
            PredefinedCityModelPackage::Relief,
            PredefinedCityModelPackage::DisasterRisk,
            PredefinedCityModelPackage::LandUse,
            PredefinedCityModelPackage::UrbanPlanningDecision,
            PredefinedCityModelPackage::Unknown,
        };
    }

    //+SVerticalBox::Slot()
    //    .AutoHeight()
    //    .Padding(0, 10, 0, 0)
    //    [SNew(STextBlock)
    //    .Text(GetDisplayName(Package))
    //    ]

    //+ SVerticalBox::Slot()
    //    .AutoHeight()
    //    .Padding(FMargin(0, 0, 0, 0))[
    //        SNew(SHorizontalBox)
    //            + SHorizontalBox::Slot()
    //            [
    //                SNew(STextBlock)
    //                .Text(LOCTEXT("ToggleImport", "インポートする"))
    //            ]
    //        + SHorizontalBox::Slot()
    //            [
    //                SNew(SCheckBox)
    //                .IsChecked(true)
    //            .OnCheckStateChanged_Lambda(
    //                [this, Package](ECheckBoxState State) {
    //                    FeatureSettingsMap[Package].IncludeAppearance = State != ECheckBoxState::Unchecked;
    //                })
    //            ]
    //    ]

    //    + SVerticalBox::Slot()
    //        .AutoHeight()
    //        .Padding(FMargin(0, 0, 0, 0))[
    //            SNew(SHorizontalBox)
    //                + SHorizontalBox::Slot()
    //                [
    //                    SNew(STextBlock)
    //                    .Text(LOCTEXT("IncludeAppearance", "テクスチャをインポートする"))
    //                ]
    //            + SHorizontalBox::Slot()
    //                [
    //                    SNew(SCheckBox)
    //                    .IsChecked(FeatureSettingsMap[Package].IncludeAppearance)
    //                .OnCheckStateChanged_Lambda(
    //                    [this, Package](ECheckBoxState State) {
    //                        FeatureSettingsMap[Package].IncludeAppearance = State != ECheckBoxState::Unchecked;
    //                    })
    //                ]
    //        ]

    //        + SVerticalBox::Slot()
    //            .AutoHeight()
    //            .Padding(FMargin(0, 0, 0, 0))[
    //                SNew(SHorizontalBox)
    //                    + SHorizontalBox::Slot()
    //                    [
    //                        SNew(STextBlock)
    //                        .Text(LOCTEXT("GenerateCollider", "Mesh Colliderをセットする"))
    //                    ]
    //                + SHorizontalBox::Slot()
    //                    [
    //                        SNew(SCheckBox)
    //                        .IsChecked(FeatureSettingsMap[Package].GenerateCollider)
    //                    .OnCheckStateChanged_Lambda(
    //                        [this, Package](ECheckBoxState State) {
    //                            FeatureSettingsMap[Package].GenerateCollider = State != ECheckBoxState::Unchecked;
    //                        })
    //                    ]
    //            ]

    //            + SVerticalBox::Slot()
    //                .AutoHeight()
    //                .Padding(20, 0, 0, 0)
    //                [SNew(SVerticalBox)
    //                // 最小LODスライダー
    //                + SVerticalBox::Slot()
    //                .AutoHeight()
    //                .Padding(0, 0, 0, 0)
    //                [SNew(SSlider)
    //                .MaxValue(3)
    //                .MinValue(Package == PredefinedCityModelPackage::Building || Package == PredefinedCityModelPackage::Unknown ? 0 : 1)
    //                .StepSize(1)
    //                .MouseUsesStep(true)
    //                .Value(FeatureSettingsMap[Package].MinLod)
    //                .OnValueChanged_Lambda(
    //                    [this, Package](int Value) {
    //                        FeatureSettingsMap[Package].MinLod = Value;
    //                    })]

    //            // 最大LODスライダー
    //            + SVerticalBox::Slot()
    //                .AutoHeight()
    //                .Padding(0, 0, 0, 0)
    //                [SNew(SSlider)
    //                .MaxValue(3)
    //                .MinValue(Package == PredefinedCityModelPackage::Building || Package == PredefinedCityModelPackage::Unknown ? 0 : 1)
    //                .StepSize(1)
    //                .MouseUsesStep(true)
    //                .Value(FeatureSettingsMap[Package].MaxLod)
    //                .OnValueChanged_Lambda(
    //                    [this, Package](int Value) {
    //                        FeatureSettingsMap[Package].MaxLod = Value;
    //                    })]

    //            + SVerticalBox::Slot()
    //                .AutoHeight()
    //                .Padding(0, 5, 0, 0)
    //                [
    //                    SNew(STextBlock)
    //                    .Text_Lambda(
    //                        [this, Package]() {
    //                            return  FText::Format(
    //                                LOCTEXT("LODRangeFormat", "最小LOD: {0}, 最大LOD: {1}"),
    //                                FText::AsNumber(FeatureSettingsMap[Package].MinLod),
    //                                FText::AsNumber(FeatureSettingsMap[Package].MaxLod));
    //                        })
    //                ]

    //            // モデル結合
    //            + SVerticalBox::Slot()
    //                .AutoHeight()
    //                .Padding(0, 0, 0, 10)
    //                [SNew(SHorizontalBox)
    //                + SHorizontalBox::Slot()
    //                [SNew(STextBlock)
    //                .Text(LOCTEXT("MeshGranularity", "モデル結合"))]
    //            + SHorizontalBox::Slot()
    //                [SNew(SComboButton)
    //                .OnGetMenuContent_Lambda(
    //                    [this, Package]() {
    //                        FMenuBuilder MenuBuilder(true, nullptr);
    //                        const auto Items = GetMeshGranularityTexts();
    //                        for (auto ItemIter = Items.CreateConstIterator(); ItemIter; ++ItemIter) {
    //                            FText ItemText = ItemIter->Value;
    //                            MeshGranularity Gran = ItemIter->Key;
    //                            FUIAction ItemAction(FExecuteAction::CreateLambda(
    //                                [this, Gran, Package]() {
    //                                    FeatureSettingsMap[Package].Granularity = Gran;
    //                                }));
    //                            MenuBuilder.AddMenuEntry(ItemText, TAttribute<FText>(), FSlateIcon(), ItemAction);
    //                        }
    //                        return MenuBuilder.MakeWidget();
    //                    })
    //                .ContentPadding(0.0f)
    //                        //.ButtonStyle(FEditorStyle::Get(), "ToggleButton")
    //                        //.ForegroundColor(FSlateColor::UseForeground())
    //                        .VAlign(VAlign_Center)
    //                        .ButtonContent()
    //                        [SNew(STextBlock).Text_Lambda(
    //                            [this, Package]() {
    //                                // TODO
    //                                return GetMeshGranularityTexts()[FeatureSettingsMap[Package].Granularity];
    //                            })]]]

    //                ];
}

void FPLATEAUFeatureSettingsDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) {

    TArray<TWeakObjectPtr<UObject>> ObjectsBeingCustomized;
    DetailBuilder.GetObjectsBeingCustomized(ObjectsBeingCustomized);

    auto ImportSettings = Cast<UPLATEAUImportSettings>(ObjectsBeingCustomized[0].Get());

    for (const auto& Package : GetAllPackages()) {
        const auto PropertyName = GetFeaturePlacementSettingsPropertyName(Package);
        FName CategoryName;
        FText LocalizedCategoryName;

        switch (Package) {
        case PredefinedCityModelPackage::Building:
            CategoryName = TEXT("Building");
            LocalizedCategoryName = LOCTEXT("Building", "建築物");
            break;
        case PredefinedCityModelPackage::Road:
            CategoryName = TEXT("Road");
            LocalizedCategoryName = LOCTEXT("Road", "道路");
            break;
        case PredefinedCityModelPackage::Vegetation:
            CategoryName = TEXT("Vegetation");
            LocalizedCategoryName = LOCTEXT("Vegetation", "植生");
            break;
        case PredefinedCityModelPackage::CityFurniture:
            CategoryName = TEXT("CityFurniture");
            LocalizedCategoryName = LOCTEXT("CityFurniture", "都市設備");
            break;
        case PredefinedCityModelPackage::Relief:
            CategoryName = TEXT("Relief");
            LocalizedCategoryName = LOCTEXT("Relief", "起伏");
            break;
        case PredefinedCityModelPackage::DisasterRisk:
            CategoryName = TEXT("DisasterRisk");
            LocalizedCategoryName = LOCTEXT("DisasterRisk", "災害リスク");
            break;
        case PredefinedCityModelPackage::LandUse:
            CategoryName = TEXT("LandUse");
            LocalizedCategoryName = LOCTEXT("LandUse", "土地利用");
            break;
        case PredefinedCityModelPackage::UrbanPlanningDecision:
            CategoryName = TEXT("UrbanPlanningDecision");
            LocalizedCategoryName = LOCTEXT("UrbanPlanningDecision", "都市計画決定情報");
            break;
        case PredefinedCityModelPackage::Unknown:
            CategoryName = TEXT("Unknown");
            LocalizedCategoryName = LOCTEXT("Unknown", "その他");
            break;
        }

        IDetailCategoryBuilder& Category =
            DetailBuilder.EditCategory(CategoryName, LocalizedCategoryName);

        const auto FeatureSettingsProperty = DetailBuilder.GetProperty(PropertyName);

        DetailBuilder.HideProperty(FeatureSettingsProperty);

        if (!FeatureSettingsRowMap.Find(Package)) {
            FeatureSettingsRowMap.Add(Package, FPLATEAUFeatureSettingsRow(Package));

            FeatureSettingsRowMap[Package].AddToCategory(Category, FeatureSettingsProperty);
        }
    }
}

void FPLATEAUFeatureSettingsRow::AddToCategory(IDetailCategoryBuilder& Category, TSharedPtr<IPropertyHandle> FeatureSettingsProperty) {
    auto ImportProperty = FeatureSettingsProperty->GetChildHandle(GET_MEMBER_NAME_CHECKED(FPLATEAUFeatureImportSettings, bImport));
    auto ImportTextureProperty = FeatureSettingsProperty->GetChildHandle(GET_MEMBER_NAME_CHECKED(FPLATEAUFeatureImportSettings, bImportTexture));
    auto SetColliderProperty = FeatureSettingsProperty->GetChildHandle(GET_MEMBER_NAME_CHECKED(FPLATEAUFeatureImportSettings, bSetCollider));
    auto MinLodProperty = FeatureSettingsProperty->GetChildHandle(GET_MEMBER_NAME_CHECKED(FPLATEAUFeatureImportSettings, MinLod));
    auto MaxLodProperty = FeatureSettingsProperty->GetChildHandle(GET_MEMBER_NAME_CHECKED(FPLATEAUFeatureImportSettings, MaxLod));
    auto GranularityProperty = FeatureSettingsProperty->GetChildHandle(GET_MEMBER_NAME_CHECKED(FPLATEAUFeatureImportSettings, MeshGranularity));

    // インポートする
    Category.AddCustomRow(FText::FromString(TEXT("Import")))
        .NameContent()[SNew(STextBlock).Text(LOCTEXT("Import", "インポートする"))]
        .ValueContent()[ImportProperty->CreatePropertyValueWidget()];

    // テクスチャをインポートする
    Category.AddCustomRow(FText::FromString(TEXT("Import Texture")))
        .NameContent()[SNew(STextBlock).Text(LOCTEXT("Import Texture", "テクスチャをインポートする"))]
        .ValueContent()[ImportTextureProperty->CreatePropertyValueWidget()];

    // MeshColliderをセットする
    Category.AddCustomRow(FText::FromString(TEXT("Set Collider")))
        .NameContent()[SNew(STextBlock).Text(LOCTEXT("Set Collider", "MeshColliderをセットする"))]
        .ValueContent()[SetColliderProperty->CreatePropertyValueWidget()];

    // 最小LOD
    Category.AddCustomRow(FText::FromString(TEXT("Min LOD")))
        .NameContent()[MinLodProperty->CreatePropertyNameWidget()]
        .ValueContent()[MinLodProperty->CreatePropertyValueWidget()];

    // 最大LOD
    Category.AddCustomRow(FText::FromString(TEXT("Max LOD")))
        .NameContent()[MaxLodProperty->CreatePropertyNameWidget()]
        .ValueContent()[MaxLodProperty->CreatePropertyValueWidget()];

    // モデル結合
    Category.AddCustomRow(FText::FromString(TEXT("Granularity")))
        .NameContent()[SNew(STextBlock).Text(LOCTEXT("MeshGranularity", "モデル結合"))]
        .ValueContent()
        [SNew(SComboButton)
        .OnGetMenuContent_Lambda(
            [GranularityProperty]() {
                FMenuBuilder MenuBuilder(true, nullptr);
                const auto Items = GetGranularityTexts();
                for (auto ItemIter = Items.CreateConstIterator(); ItemIter; ++ItemIter) {
                    FText ItemText = ItemIter->Value;
                    auto Gran = ItemIter->Key;
                    FUIAction ItemAction(FExecuteAction::CreateLambda(
                        [GranularityProperty, Gran]() {
                            GranularityProperty->SetValue(static_cast<uint8>(Gran));
                        }));
                    MenuBuilder.AddMenuEntry(ItemText, TAttribute<FText>(), FSlateIcon(), ItemAction);
                }
                return MenuBuilder.MakeWidget();
            })
        .ContentPadding(0.0f)
                //.ButtonStyle(FEditorStyle::Get(), "ToggleButton")
                //.ForegroundColor(FSlateColor::UseForeground())
                .VAlign(VAlign_Center)
                .ButtonContent()
                [SNew(STextBlock).Text_Lambda(
                    [GranularityProperty]() {
                        // TODO
                        const auto Texts = GetGranularityTexts();

                        if (!GranularityProperty.IsValid())
                            return Texts[EPLATEAUMeshGranularity::PerCityModelArea];

                        uint8 Out;
                        GranularityProperty->GetValue(Out);
                        return Texts[static_cast<EPLATEAUMeshGranularity>(Out)];
                    })]];
}

#undef LOCTEXT_NAMESPACE
