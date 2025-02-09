﻿#include "Character/RMGamePlayerCharacter.h"

ARMGamePlayerCharacter::ARMGamePlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	Tags.Add(FName(TEXT("RMGamePlayerCharacter")));
}

FPlayerCharacter& ARMGamePlayerCharacter::GetMyPlayer()
{
	return PlayerCharacter;
}

URMPlayerAnimInstance* ARMGamePlayerCharacter::GetAnimInst()
{
	URMPlayerAnimInstance *AnimInst = Cast<URMPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	return AnimInst;
}

void ARMGamePlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	UMaterialInstanceDynamic* BodySkin = GetMesh()->CreateDynamicMaterialInstance(0);
	if (BodySkin) {
		Material.Add(BodySkin);
	}
	TArray<UActorComponent*> Skel;//= GetComponentsByClass(USkeletalMeshComponent::StaticClass());
	this->GetComponents(USkeletalMeshComponent::StaticClass(), Skel);
	for (auto It : Skel) {
		USkeletalMeshComponent* S = Cast<USkeletalMeshComponent>(It);
		if (S) {
			if (S->ComponentHasTag(FName(TEXT("Head")))) {
				UMaterialInstanceDynamic* HeadSkin = S->CreateDynamicMaterialInstance(0);
				if (HeadSkin) {
					Material.Add(HeadSkin);
				}

				UMaterialInstanceDynamic* Eye = S->CreateDynamicMaterialInstance(1);
				if (Eye) {
					Material.Add(Eye);
				}
			}
		}
	}
	Load();
}

void ARMGamePlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ARMGamePlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//UpdateCustomLOD();
}

void ARMGamePlayerCharacter::Load()
{
	URMPlayerAnimInstance *AnimInst = GetAnimInst();
	if (AnimInst == nullptr) {
		return;
	}

	FPlayerCharacter& Me = GetMyPlayer();
	//모퍼값 세팅
	for (auto& It : Me.MorphTargetValue) {
		AnimInst->SetMorphTarget(It.Name, It.Value);
	}
	for (auto It : Me.Costume) {
		if (It.Value != TEXT("")) {
			ChangeCostumePart(It.Key, It.Value);
		}
	}
	//몸 스케일
	AnimInst->Scale = Me.Scale;

	//손 스케일
	AnimInst->HandScale = Me.HandScale;


	AnimInst->FacialBoneScale = Me.FacialBoneScale;


	//헤드 스케일에 따라 특정 모포가 영향을 받는다.
	AnimInst->HeadScale = Me.HeadScale;
	
	/*
	FVector2D Input(1.06f, 0.90302f);
	FVector2D Output1(1.0f, 0.0f);
	FVector2D Output2(0.0f, 1.0f);
	float V1 = FMath::GetMappedRangeValueClamped(Input, Output1, Me.HeadScale);
	float V2 = FMath::GetMappedRangeValueClamped(Input, Output2, Me.HeadScale);
	AnimInst->SetMorphTarget("HeadScaleUp", V1);
	AnimInst->SetMorphTarget("HeadScaleDown", V2);
	*/


	
	/*
	if (Me.HeadScale > 1.0f) {
		FVector2D InputRange(0.95f, 1.05);
		FVector2D Output(0.0f, 1.0f);
		float Value = FMath::GetMappedRangeValueClamped(InputRange,Output,Me.HeadScale);
		AnimInst->SetMorphTarget("HeadScaleUp", Value);
		AnimInst->SetMorphTarget("HeadScaleDown",0);
	}
	else {
		FVector2D InputRange(1.05,0.95f);
		FVector2D Output(0.0f, 1.0f);
		float Value = FMath::GetMappedRangeValueClamped(InputRange,Output,Me.HeadScale);
		AnimInst->SetMorphTarget("HeadScaleUp",0);
		AnimInst->SetMorphTarget("HeadScaleDown", Value);
	}
	*/


	//상체 스케일
	if (Me.TopSize > 0) {
		AnimInst->SetMorphTarget("TopSlim",0.0f);
		AnimInst->SetMorphTarget("TopBig",Me.TopSize);
	}
	else {
		AnimInst->SetMorphTarget("TopSlim",FMath::Abs(Me.TopSize));
		AnimInst->SetMorphTarget("TopBig",0);
	}


	if (Me.BottomSize > 0.0f) {
		AnimInst->SetMorphTarget("BottomSlim", 0.0f);
		AnimInst->SetMorphTarget("BottomBig", Me.BottomSize);
	}
	else {
		AnimInst->SetMorphTarget("BottomSlim", FMath::Abs(Me.BottomSize));
		AnimInst->SetMorphTarget("BottomBig",0);
	}

	if (Me.CheekSize < 0) {
		AnimInst->SetMorphTarget("CheekBlow", 0);
		AnimInst->SetMorphTarget("CheekSuck", FMath::Abs(Me.CheekSize));
	}
	else {
		AnimInst->SetMorphTarget("CheekBlow", Me.CheekSize);
		AnimInst->SetMorphTarget("CheekSuck", 0);
	}

	if (Material.Num()) {
		//FColor
		for (auto It : Material) {
			if (It) {
				It->SetVectorParameterValue(FName(TEXT("SkinColor")), Me.SkinColor);
			}
		}
		for (auto It : Me.EyeMaterialScalarParameter) {
			switch (It.Key) {
			case EEyeMaterialScalarParameter::EyeColor:
				Material[2]->SetScalarParameterValue(FName(TEXT("EyeColor")), It.Value);
				break;
			case EEyeMaterialScalarParameter::IrisBrightness:
				Material[2]->SetScalarParameterValue(FName(TEXT("IrisBRightness")), It.Value);
				break;
			case EEyeMaterialScalarParameter::PupilScale:
				Material[2]->SetScalarParameterValue(FName(TEXT("PupilScale")), It.Value);
				break;
			case EEyeMaterialScalarParameter::ScleraBrightness:
				Material[2]->SetScalarParameterValue(FName(TEXT("ScleraBrightness")), It.Value);
				break;

			}
		}
	}
}

void ARMGamePlayerCharacter::SetHandScale(float Scale)
{
	FPlayerCharacter& Me = GetMyPlayer();
	URMPlayerAnimInstance *AnimInst = GetAnimInst();
	if (AnimInst) {
		Me.HandScale = Scale;
		AnimInst->HandScale = Scale;
	}
}

void ARMGamePlayerCharacter::SetTopSize(float Size)
{
	FPlayerCharacter& Me = GetMyPlayer();
	URMPlayerAnimInstance *AnimInst = GetAnimInst();
	if (AnimInst) {
		Me.TopSize = Size;
		//상체 스케일
		if (Me.TopSize > 0) {
			AnimInst->SetMorphTarget("TopSlim", 0.0f);
			AnimInst->SetMorphTarget("TopBig", Me.TopSize);
		}
		else {
			AnimInst->SetMorphTarget("TopSlim", FMath::Abs(Me.TopSize));
			AnimInst->SetMorphTarget("TopBig", 0);
		}
	}
}

void ARMGamePlayerCharacter::SetBottomSize(float Size)
{
	FPlayerCharacter& Me = GetMyPlayer();
	URMPlayerAnimInstance *AnimInst = GetAnimInst();
	if (AnimInst) {
		Me.BottomSize = Size;
		if (Me.BottomSize > 0.0f) {
			AnimInst->SetMorphTarget("BottomSlim", 0.0f);
			AnimInst->SetMorphTarget("BottomBig", Me.BottomSize);
		}
		else {
			AnimInst->SetMorphTarget("BottomSlim", FMath::Abs(Me.BottomSize));
			AnimInst->SetMorphTarget("BottomBig", 0);
		}
	}
}

void ARMGamePlayerCharacter::SetHeadScale(float Scale)
{
	FPlayerCharacter& Me = GetMyPlayer();
	URMPlayerAnimInstance *AnimInst = GetAnimInst();
	if (AnimInst) {
		//헤드 스케일에 따라 특정 모포가 영향을 받는다.
		Me.HeadScale = Scale;
		AnimInst->HeadScale = Me.HeadScale;

		/*
		FVector2D Input(1.06f, 0.90302f);
		FVector2D Output1(1.0f, 0.0f);
		FVector2D Output2(0.0f, 1.0f);
		float V1 = FMath::GetMappedRangeValueClamped(Input, Output1, Me.HeadScale);
		float V2 = FMath::GetMappedRangeValueClamped(Input, Output2, Me.HeadScale);
		AnimInst->SetMorphTarget("HeadScaleUp", V1);
		AnimInst->SetMorphTarget("HeadScaleDown", V2);
		*/
		
	}
}

void ARMGamePlayerCharacter::SetScale(float Scale)
{
	FPlayerCharacter& Me = GetMyPlayer();
	URMPlayerAnimInstance *AnimInstance = GetAnimInst();
	if (AnimInstance) {
		Me.Scale = Scale;
		AnimInstance->Scale = Scale;
	}
}

void ARMGamePlayerCharacter::SetCostumePart(ECostumePart Part, FName RowName)
{
	FPlayerCharacter& Me = GetMyPlayer();
	if (ChangeCostumePart(Part, RowName)) {
		Me.Costume[Part] = RowName;
	}
}

void ARMGamePlayerCharacter::SetFacialBone(const FVector& Value)
{
	FPlayerCharacter& Me = GetMyPlayer();
	URMPlayerAnimInstance *AnimInstance = GetAnimInst();
	if (AnimInstance) {
		Me.FacialBoneScale = Value;
		AnimInstance->FacialBoneScale = Value;
	}

}

void ARMGamePlayerCharacter::SetEyeScalaParameter(EEyeMaterialScalarParameter Enum, float Value)
{
	/*
	FPlayerCharacter& Me = GetMyPlayer();
	switch (Enum) {
	case EEyeMaterialScalarParameter::EyeColor:
		Material[2]->SetScalarParameterValue(FName(TEXT("EyeColor")), Value);
		break;
	case EEyeMaterialScalarParameter::IrisBrightness:
		Material[2]->SetScalarParameterValue(FName(TEXT("IrisBRightness")), Value);
		break;
	case EEyeMaterialScalarParameter::PupilScale:
		Material[2]->SetScalarParameterValue(FName(TEXT("PupilScale")), Value);
		break;
	case EEyeMaterialScalarParameter::ScleraBrightness:
		Material[2]->SetScalarParameterValue(FName(TEXT("ScleraBrightness")), Value);
		break;
	}
	Me.EyeMaterialScalarParameter[Enum] = Value;
	*/
}


void ARMGamePlayerCharacter::SetSkinColor(const FLinearColor &SkinColor)
{
	FPlayerCharacter& Me = GetMyPlayer();
	if (Material.Num()) {
		//FColor
		Me.SkinColor = SkinColor;
		for (auto It : Material) {
			if (It) {
				It->SetVectorParameterValue(FName(TEXT("SkinColor")), Me.SkinColor);
			}
		}
	}
}

void ARMGamePlayerCharacter::SetCheekSize(float Size)
{
	FPlayerCharacter& Me = GetMyPlayer();
	URMPlayerAnimInstance *AnimInst = GetAnimInst();
	if (AnimInst) {
		Me.CheekSize = Size;
		if (Me.CheekSize < 0) {
			AnimInst->SetMorphTarget("CheekBlow", 0);
			AnimInst->SetMorphTarget("CheekSuck", FMath::Abs(Me.CheekSize));
		}
		else {
			AnimInst->SetMorphTarget("CheekBlow", Me.CheekSize);
			AnimInst->SetMorphTarget("CheekSuck", 0);
		}
	}
}

bool ARMGamePlayerCharacter::ChangeCostumePart(ECostumePart CostumePart, FName CostumeRowName)
{
	UDataTable* DataTable = URM_Singleton::GetSingleton(GetWorld())->CostumeDatatable;
	if (DataTable && DataTable->GetRowMap().Num() > 0) {
		FString ContextString;
		FCostumeTable* Row = DataTable->FindRow<FCostumeTable>(CostumeRowName, ContextString);
		if (Row) {
			if (Row->Part == CostumePart) {
				switch (CostumePart) {
				case ECostumePart::Hair:
				//case ECostumePart::Head:
				case ECostumePart::Top:
				case ECostumePart::Bottom:
				case ECostumePart::Shoes: {
						FString S = EnumToString(TEXT("ECostumePart"),(uint8)CostumePart);
						
						if (Row->SkeletalMesh.IsPending()) Row->SkeletalMesh.LoadSynchronous();
						if (Row->Material.IsPending()) Row->Material.LoadSynchronous();
						ChangeSkeletalMesh(*S, Row->SkeletalMesh.Get(), Row->Material.Get());
						
						if (CostumePart == ECostumePart::Head) {
							if (Row->IsCustomHead)
							{
								if (Row->Tex1.IsPending()) Row->Tex1.LoadSynchronous();
								if (Row->Tex2.IsPending()) Row->Tex2.LoadSynchronous();
								if (Row->Tex2.IsPending()) Row->Tex3.LoadSynchronous();
								
								Material[1]->SetTextureParameterValue("Defuse", Row->Tex1.Get());
								Material[1]->SetTextureParameterValue("Normal", Row->Tex2.Get());
								Material[1]->SetTextureParameterValue("split_AM_R_M",Row->Tex3.Get());

								FPlayerCharacter& Me = GetMyPlayer();
								SetSkinColor(Me.SkinColor);
							}
							else
							{
								UMaterialInstance* PresetHeadMaterial = Row->Material.Get();
								if (PresetHeadMaterial)
								{
									FLinearColor OutValue;
									PresetHeadMaterial->GetVectorParameterValue(FHashedMaterialParameterInfo("SkinColor"), OutValue);
									// Material[0]->SetVectorParameterValue("SkinColor", OutValue);
									// SetSkinColor(OutValue);
									for (auto It : Material) {
										if (It) {
											It->SetVectorParameterValue(FName(TEXT("SkinColor")), OutValue);
										}
									}
								}
							}
						}
					}
					return true;
					/*
				case ECostumePart::FaceSkin:
					if (Row->Tex1.IsPending()) Row->Tex1.LoadSynchronous();
					Material[1]->SetTextureParameterValue("Defuse", Row->Tex1.Get());
					return true;

				case ECostumePart::EyeBrow:
					if (Row->Tex1.IsPending()) Row->Tex1.LoadSynchronous();
					Material[1]->SetTextureParameterValue("EyeBrowType", Row->Tex1.Get());
					return true;

				case ECostumePart::Lip:
					if (Row->Tex1.IsPending()) Row->Tex1.LoadSynchronous();
					Material[1]->SetTextureParameterValue("LipColor", Row->Tex1.Get());
					return true;
					*/
				}
			}
		}
	}
	return false;
}

void ARMGamePlayerCharacter::SetMorpherTargetValue(EMorphTarget Target, float Value)
{
	FPlayerCharacter& Me = GetMyPlayer();
	URMPlayerAnimInstance *AnimInst = GetAnimInst();
	if (AnimInst) {
		Me.MorphTargetValue[(int8)Target].Value = Value;
		AnimInst->SetMorphTarget(Me.MorphTargetValue[(int8)Target].Name, Value);
	}
}
//USkeletalMesh* NewMesh, UMaterialInstance* NewMaterial
void ARMGamePlayerCharacter::ChangeSkeletalMesh(FName Tag, USkeletalMesh* NewMesh, UMaterialInstance* NewMaterial)
{
	if (NewMesh == nullptr) {
		return;
	}
	TArray<UActorComponent*> Skel;//= GetComponentsByClass(USkeletalMeshComponent::StaticClass());
	this->GetComponents(USkeletalMeshComponent::StaticClass(), Skel);
	for (auto It : Skel) {
		USkeletalMeshComponent* S = Cast<USkeletalMeshComponent>(It);
		if (S) {
			if (S->ComponentHasTag(Tag)) {
				S->SetSkeletalMesh(NewMesh);
				if (NewMaterial) {
					S->SetMaterial(0, NewMaterial);
				if (S->ComponentHasTag(FName(TEXT("Head"))))
					Material[1] = S->CreateDynamicMaterialInstance(0);
				}
			}
		}
	}
}

void ARMGamePlayerCharacter::UpdateCustomLOD()
{
	if (!CustomLOD) return;
	FVector P1 = UGameplayStatics::GetPlayerCameraManager(GetWorld(),0)->GetCameraLocation();
	FVector P2 = GetActorLocation();
	float Distance = (P1 - P2).Size();

	CurrentCustomLOD = 0;
	for (int32 I = 0; I < CustomLODDistanceRange.Num(); I++) {
		if (Distance > CustomLODDistanceRange[I]) {
			++CurrentCustomLOD;
		}
		else break;
	}
	CurrentCustomLOD = FMath::Clamp<float>(CurrentCustomLOD,CustomLODLevelRange.X, CustomLODLevelRange.Y);
	
	USkeletalMeshComponent *SK = GetMesh();
	if (SK) {
		TArray<UActorComponent*> Skel = GetComponentsByClass(USkeletalMeshComponent::StaticClass());
		for (auto It : Skel) {
			USkeletalMeshComponent* S = Cast<USkeletalMeshComponent>(It);
			if (S) {
				S->SetForcedLOD(CurrentCustomLOD);
			}
		}
	}
}