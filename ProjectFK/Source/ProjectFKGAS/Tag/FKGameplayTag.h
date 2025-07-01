
#pragma once

#include "GameplayTagContainer.h"

#define FKTAG_CHARACTER_ISDEAD FGameplayTag::RequestGameplayTag(FName("Character.State.IsDead"))
#define FKTAG_CHARACTER_ISFALLING FGameplayTag::RequestGameplayTag(FName("Character.State.IsFalling"))
#define FKTAG_CHARACTER_ISINVINSIBLE FGameplayTag::RequestGameplayTag(FName("Character.State.IsInvinsible"))

#define GAMEPLAYCUE_CHARACTER_ATTACKHIT_KNIGHT FGameplayTag::RequestGameplayTag(FName("GameplayCue.Character.AttackHit.Knight"))