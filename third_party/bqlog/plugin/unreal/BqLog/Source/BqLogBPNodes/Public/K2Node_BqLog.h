/* Copyright (C) 2025 Tencent.
* BQLOG is licensed under the Apache License, Version 2.0.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
#pragma once

#include "CoreMinimal.h"
#include "BqLog.h"
#include "K2Node.h"
#include "K2Node_AddPinInterface.h"
#include "K2Node_BqLog.generated.h"

UCLASS()
class BQLOGBPNODES_API UK2Node_BqLogFormat : public UK2Node, public IK2Node_AddPinInterface
{
	GENERATED_BODY()
public:
	static const FName LogInstancePinName;
	static const FName LogLevelPinName;
	static const FName LogFormatStringPinName;
	static const FName CategoryPinName; 
	static const FName ArgPinPrefix;
	static const FName ReturnPinName; 

	virtual void PostLoad() override;
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	virtual void GetMenuActions(class FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual FText GetMenuCategory() const override;
	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, class UEdGraph* SourceGraph) override;
	virtual void ValidateNodeDuringCompilation(class FCompilerResultsLog& MessageLog) const override;
	virtual void ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) override;

	virtual bool CanAddPin() const override;
	virtual void AddInputPin() override;
	void RemoveArgumentPin(UEdGraphPin* Pin);

	virtual void AddPin() { AddInputPin(); }

	
	virtual bool CanRemovePin(const UEdGraphPin* Pin) const
#if ENGINE_MAJOR_VERSION >= 5
	override
#endif
	;
	
	virtual void RemoveInputPin(class UEdGraphPin* Pin)
#if ENGINE_MAJOR_VERSION >= 5
	override
#endif
	;

#if ENGINE_MAJOR_VERSION >= 5 || (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 24)
	virtual void GetNodeContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const override;
#else
	virtual void GetContextMenuActions(const struct FGraphNodeContextMenuBuilder& Context) const override;
#endif
	
	virtual bool SupportsAddPin() const { return true; }
	virtual bool SupportsRemovePin() const { return true; }
	
#if WITH_EDITOR
	virtual int32 GetNumArguments() const;
	virtual void AddArgument();
	virtual void RemoveArgument(int32 Index);
	virtual bool CanRemoveArgument(int32 Index) const;
#endif


	virtual void NotifyPinConnectionListChanged(UEdGraphPin* Pin) override;

	void OnLogInstanceChanged();
	void RefreshCategoryPinStatus();

    virtual void PinDefaultValueChanged(UEdGraphPin* Pin) override;

	//~ Begin UObject Interface
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	//~ End UObject Interface

private:
	/** When adding arguments to the node, their names are placed here and are generated as pins during construction */
	UPROPERTY(VisibleAnywhere, Category = "Arguments", meta = (DisplayName = "Arguments"))
	TArray<FName> ArgumentNames;

	
	UEdGraphPin* GetLogExecutePin() const;
	UEdGraphPin* GetLogThenPin() const;
	UEdGraphPin* GetLogInstancePin() const;
	UEdGraphPin* GetLogLevelPin() const;
	UEdGraphPin* GetLogFormatStringPin() const;

	UEdGraphPin* CreateNextArgPin();
	void GetArgPins(TArray<UEdGraphPin*>& OutPins) const;
	static FName GetArgNameFromPin(const UEdGraphPin* Pin);
	FName GetUniqueArgumentName() const;

	struct FBqLogMenuItem
	{
		FName Id;
		FText Label;
		FText Tooltip;
		TFunction<void()> Action;
	};

	void CollectBqLogMenuItems(TArray<FBqLogMenuItem>& OutItems) const;

private:
	UPROPERTY()
	TSubclassOf<UBqLog> SavedLogType = nullptr;
	UPROPERTY(Transient)
	TSet<FName> PinsToPurge;
};