#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#if WITH_EDITOR
#include "ModuleInterface.h"
#include "ISettingsModule.h"
#include "ISettingsContainer.h"
#include "Item.h"

static const FName Settings_Container = TEXT("Project");
static const FName Settings_ProjectName = TEXT("GSEduItems");
static const FName SettingsSection_Data = TEXT("ItemReg");
#endif // WITH_EDITOR
#define LOCTEXT_NAMESPACE "GSEduItemsEditor"

class FGSEduItemsEditor : public FDefaultModuleImpl
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE(FGSEduItemsEditor, GSEduItems)

void FGSEduItemsEditor::StartupModule()
{
#if WITH_EDITOR
	ISettingsModule* Settings = (ISettingsModule*)FModuleManager::Get().GetModule("Settings");
	auto Container = Settings->GetContainer(Settings_Container);
	Container->DescribeCategory(Settings_ProjectName, LOCTEXT("Settings_ModuleSectionName", "GSEduItems"), LOCTEXT("Settings_ModuleSectionDesc", "Settings for the GSEduItems module."));
	Settings->RegisterSettings(Settings_Container, Settings_ProjectName, SettingsSection_Data, LOCTEXT("Settings_DataSectionName", "Item Discovery"), LOCTEXT("Settings_DataSectionDesc", "Settings controlling how items are discovered."), GetMutableDefault<UItemDataHelpers>());
#endif // WITH_EDITOR
}

void FGSEduItemsEditor::ShutdownModule()
{
#if WITH_EDITOR
	ISettingsModule* Settings = (ISettingsModule*)FModuleManager::Get().GetModule("Settings");
	Settings->UnregisterSettings(Settings_Container, Settings_ProjectName, SettingsSection_Data);
#endif // WITH_EDITORs
}
#undef LOCTEXT_NAMESPACE