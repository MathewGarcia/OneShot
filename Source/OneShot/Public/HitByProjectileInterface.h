// HitByProjectileInterface.h

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Projectile.h"
#include "HitByProjectileInterface.generated.h"

UINTERFACE(MinimalAPI)
class UHitByProjectileInterface : public UInterface
{
    GENERATED_BODY()
};

class ONESHOT_API IHitByProjectileInterface : public IInterface
{
    GENERATED_IINTERFACE_BODY()

public:
    virtual void HitByProjectile(float DamageAmount,AActor*Shooter,FDamageEvent&DamageEvent) PURE_VIRTUAL(IHitByProjectileInterface::HitByProjectile, );
};
