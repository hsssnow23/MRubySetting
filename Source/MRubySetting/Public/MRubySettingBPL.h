
#pragma once

#include "Engine.h"
#include "Factories/Factory.h"

extern "C" {
	#include "mruby.h"
	#include "mruby/variable.h"
	#include "mruby/hash.h"
	#include "mruby/array.h"
	#include "mruby/compile.h"
	#include "mruby/string.h"
	#include "mruby/error.h"
}

#include "MRubySettingBPL.generated.h"


USTRUCT()
struct FMRubyState {
	GENERATED_USTRUCT_BODY()

public:
	mrb_state* MRB;

	FMRubyState();
	~FMRubyState();
};

UCLASS(Blueprintable)
class UMRubyValue : public UObject
{
	GENERATED_BODY()

public:
	mrb_value Value;

	UFUNCTION(BlueprintPure, Category = "MRuby")
	bool ToBool();
	UFUNCTION(BlueprintPure, Category = "MRuby")
	int32 ToInt();
	UFUNCTION(BlueprintPure, Category = "MRuby")
	float ToFloat();
	UFUNCTION(BlueprintPure, Category = "MRuby")
	FString ToString();
};


UCLASS(Blueprintable)
class UMRubySetting : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	TSharedPtr<FMRubyState> MRubyState;

	UFUNCTION(BlueprintCallable, Category = "MRuby")
	void SetConfigString(FString Key, FString Val);

	UFUNCTION(BlueprintCallable, Category = "MRuby")
	UMRubyValue* GetConfig(FString Key);

	UFUNCTION(BlueprintCallable, Category = "MRuby")
	bool ExecuteSource(UMRubyAsset* MRubyAsset);
};


UCLASS()
class UMRubySettingBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "MRuby")
	static UMRubySetting* MakeMRubySetting();
};


UCLASS()
class UMRubyAsset : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditAnywhere)
	FString Source;
};


UCLASS()
class UMRubyAssetFactory : public UFactory
{
	GENERATED_UCLASS_BODY()

	virtual bool DoesSupportClass(UClass* Class) override;
	virtual UClass* ResolveSupportedClass() override;
	virtual UObject* FactoryCreateText(
		UClass* InClass,
		UObject* InParent,
		FName InName,
		EObjectFlags Flags,
		UObject* Context,
		const TCHAR* Type,
		const TCHAR*& Buffer,
		const TCHAR* BufferEnd,
		FFeedbackContext* Warn
		) override;
};