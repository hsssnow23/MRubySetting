
#include "MRubySettingPrivatePCH.h"

#include "MRubySettingBPL.h"

#define MR_STATE MRubyState.Get()->MRB
#define G_CONFIG "config_interface"

extern "C"
mrb_value mrb_def_config(mrb_state* mrb, mrb_value value)
{
	mrb_value block;
	mrb_get_args(mrb, "&", &block);

	mrb_value config = mrb_gv_get(mrb, mrb_intern_cstr(mrb, G_CONFIG));

	mrb_value result = mrb_yield(mrb, block, config);

	config = mrb_funcall(mrb, config, "merge", 1, result);

	mrb_gv_set(mrb, mrb_intern_cstr(mrb, G_CONFIG), config);
	return config;
}

UMRubySetting::UMRubySetting(const class FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	MRubyState = MakeShareable(new FMRubyState());
	mrb_gv_set(MR_STATE, mrb_intern_cstr(MR_STATE, G_CONFIG), mrb_hash_new(MR_STATE));
	mrb_define_method(MR_STATE, MR_STATE->kernel_module, "config", mrb_def_config, MRB_ARGS_BLOCK());
}

void UMRubySetting::SetConfigString(FString Key, FString Val)
{
	mrb_value Config = mrb_gv_get(MR_STATE, mrb_intern_cstr(MR_STATE, G_CONFIG));
	if (mrb_nil_p(Config))
	{
		Config = mrb_hash_new(MR_STATE);
	}
	mrb_hash_set(MR_STATE, Config, mrb_str_new_cstr(MR_STATE, TCHAR_TO_ANSI(*Key)), mrb_str_new_cstr(MR_STATE, TCHAR_TO_ANSI(*Val)));
	mrb_gv_set(MR_STATE, mrb_intern_cstr(MR_STATE, G_CONFIG), Config);
}

UMRubyValue* UMRubySetting::GetConfig(FString Key)
{
	UMRubyValue* MRubyValue = NewObject<UMRubyValue>();
	mrb_value Config = mrb_gv_get(MR_STATE, mrb_intern_cstr(MR_STATE, G_CONFIG));
	MRubyValue->Value = mrb_hash_get(MR_STATE, Config, mrb_str_new_cstr(MR_STATE, TCHAR_TO_ANSI(*Key)));
	return MRubyValue;
}

bool UMRubySetting::ExecuteSource(UMRubyAsset* MRubyAsset)
{
	int Arena = mrb_gc_arena_save(MR_STATE);
	mrbc_context* Context = mrbc_context_new(MR_STATE);
	mrbc_filename(MR_STATE, Context, "*interactive*");
	mrb_load_string_cxt(MR_STATE, TCHAR_TO_ANSI(*MRubyAsset->Source), Context);
	mrbc_context_free(MR_STATE, Context);
	if (MR_STATE->exc) {
		mrb_value EXC = mrb_obj_value(MR_STATE->exc);
		mrb_value Backtrace = mrb_exc_backtrace(MR_STATE, EXC);
		for (mrb_int n = mrb_ary_len(MR_STATE, Backtrace), i = 0; i < n; ++i) {
			mrb_value v = mrb_ary_ref(MR_STATE, Backtrace, i);
			UE_LOG(LogTemp, Error, TEXT("%s"), ANSI_TO_TCHAR(RSTRING_PTR(v)));
		}
		mrb_gc_arena_restore(MR_STATE, Arena);
		return false;
	}
	mrb_gc_arena_restore(MR_STATE, Arena);
	return true;
}


FMRubyState::FMRubyState()
{
	MRB = mrb_open();
}

FMRubyState::~FMRubyState()
{
	mrb_close(MRB);
}


bool UMRubyValue::ToBool()
{
	return mrb_bool(Value);
}

int32 UMRubyValue::ToInt()
{
	return mrb_fixnum(Value);
}

float UMRubyValue::ToFloat()
{
	return mrb_float(Value);
}

FString UMRubyValue::ToString()
{
	return FString(ANSI_TO_TCHAR(RSTRING_PTR(Value)));
}


UMRubySetting* UMRubySettingBlueprintLibrary::MakeMRubySetting()
{
	return NewObject<UMRubySetting>();
}


UMRubyAsset::UMRubyAsset(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}


UMRubyAssetFactory::UMRubyAssetFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UMRubyAsset::StaticClass();
	bCreateNew = false;
	bEditorImport = true;
	bText = true;
	Formats.Add(TEXT("mrb;MRuby Asset"));
}

bool UMRubyAssetFactory::DoesSupportClass(UClass* Class)
{
	return (Class == UMRubyAsset::StaticClass());
}

UClass* UMRubyAssetFactory::ResolveSupportedClass()
{
	return UMRubyAsset::StaticClass();
}

UObject* UMRubyAssetFactory::FactoryCreateText(
	UClass* InClass,
	UObject* InParent,
	FName InName,
	EObjectFlags Flags,
	UObject* Context,
	const TCHAR* Type,
	const TCHAR*& Buffer,
	const TCHAR* BufferEnd,
	FFeedbackContext* Warn
	)
{
	UMRubyAsset* NewMRubyAsset =
		CastChecked<UMRubyAsset>(StaticConstructObject(InClass, InParent, InName, Flags));

	NewMRubyAsset->Source = FString(Buffer);

	return NewMRubyAsset;
}