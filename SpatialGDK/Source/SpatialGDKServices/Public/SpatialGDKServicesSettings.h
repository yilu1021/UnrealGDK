// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#pragma once

#include "SpatialGDKServicesModule.h"

#include "SpatialGDKServicesSettings.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSpatialGDKServicesSettings, Log, All);

namespace SpatialGDKServicesConstants
{
#if PLATFORM_WINDOWS
	const FString Extension = TEXT("exe");
#elif PLATFORM_MAC
	const FString Extension = TEXT("");
#endif

	const FString GDKProgramPath = FSpatialGDKServicesModule::GetSpatialGDKPluginDirectory(TEXT("SpatialGDK/Binaries/ThirdParty/Improbable/Programs"));

	static inline const FString CreateExePath(FString Path, FString ExecutableName)
	{
		FString ExecutableFile = FPaths::SetExtension(ExecutableName, Extension);
		return FPaths::Combine(Path, ExecutableFile);
	}

	const FString SpotExe = CreateExePath(GDKProgramPath, TEXT("spot"));
	const FString SchemaCompilerExe = CreateExePath(GDKProgramPath, TEXT("schema_compiler"));
	const FString SpatialOSDirectory = FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::ProjectDir(), TEXT("/../spatial/")));
	const FString SpatialOSRuntimePinnedVersion("11-20200205T105018Z-7668e9b");

}

UCLASS(config = SpatialGDKEditorSettings, defaultconfig)
class SPATIALGDKSERVICES_API USpatialGDKServicesSettings : public UObject
{
	GENERATED_BODY()

public:
	USpatialGDKServicesSettings(const FObjectInitializer& ObjectInitializer);

private:


public:
	/** If the Development Authentication Flow is used, the client will try to connect to the cloud rather than local deployment. */
	UPROPERTY(EditAnywhere, config, Category = "Cloud Connection")
	bool bUseDevelopmentAuthenticationFlow;

	/** The token created using 'spatial project auth dev-auth-token' */
	UPROPERTY(EditAnywhere, config, Category = "Cloud Connection")
	FString DevelopmentAuthenticationToken;

	/** The deployment to connect to when using the Development Authentication Flow. If left empty, it uses the first available one (order not guaranteed when there are multiple items). The deployment needs to be tagged with 'dev_login'. */
	UPROPERTY(EditAnywhere, config, Category = "Cloud Connection")
	FString DevelopmentDeploymentToConnect;

	// The location of spatial on your machine (falls back to using the PATH on Windows or /usr/local/bin on MacOS if this is left blank)
	UPROPERTY(EditAnywhere, config, Category = "Spatial Tools Configuration", meta = (ConfigRestartRequired = true))
	FDirectoryPath SpatialPath;

	FORCEINLINE FString GetSpatialExe() const
	{
#if PLATFORM_MAC
		if (SpatialPath.Path.IsEmpty())
		{
			SpatialPath.Path = TEXT("/usr/local/bin");
		}
#endif

		FString ExecutableFile = FPaths::SetExtension(TEXT("spatial"), SpatialGDKServicesConstants::Extension);
		return FPaths::Combine(SpatialPath.Path, ExecutableFile);
	}
};
