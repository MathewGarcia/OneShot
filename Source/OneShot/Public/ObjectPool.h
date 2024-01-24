#pragma once
#include "ObjectPool.h"

template <typename T>

class TObjectPool {
public:
	TObjectPool(){}
	~TObjectPool() { ClearPool(); }

	//get the array of class to spawn, amount to spawn and the world
	void InitPool(TArray<TSubclassOf<T>>& ClassesToSpawn, int32 AmountToSpawn, UWorld* CurrentWorldContext) {
		WorldContext = CurrentWorldContext;
		//for every object in classes to spawn
		for (const TSubclassOf<T>& ClassToSpawn : ClassesToSpawn) {
			//create the amount to spawn
			for (int i = 0; i < AmountToSpawn; i++) {
				//Create a new object and spawn it
				T* NewObject = WorldContext->SpawnActor<T>(ClassToSpawn, FVector(0,0,-1000), FRotator::ZeroRotator);
				if (NewObject) {
					//set the hide and disable colliison
					NewObject->SetActorEnableCollision(false);
					NewObject->SetActorHiddenInGame(true);
					NewObject->SetActivate(false);
					//add to root so we dont get destroyed by garbage collector.
					//NewObject->AddToRoot();
					//add it to the pool
					Pool.Add(NewObject);
				}
			}
		}
	}

	T* GetPooledObject(UClass*ClassToGet) {
		for (T* PooledObject : Pool) {
		
				if (PooledObject && PooledObject->IsA(ClassToGet) && !PooledObject->IsActive()) {
					PooledObject->SetActorHiddenInGame(false);
					PooledObject->SetActorEnableCollision(true);
					PooledObject->SetActivate(true);
					return PooledObject;
				}
			
		}
		return nullptr;
	}

	void ReturnPooledObject(T* Object) {
		if (!Object) return;
		Object->SetActorLocation(FVector::ZeroVector);
		Object->SetActorRotation(FRotator::ZeroRotator);
		Object->SetActorEnableCollision(false);
		Object->SetActorHiddenInGame(true);
		Object->SetActivate(false);
	}

	void ClearPool() {
		//for every pooled object in the pool
		for (T* PooledObject : Pool) {
			if (PooledObject) {
				//allow it to be garbage collected
				//PooledObject->RemoveFromRoot();
			}
		}
		//empty the pool
		Pool.Empty();
	}

	TArray<T*> GetAllPooledObjects() {
		return Pool;
	}
private:
	TArray<T*> Pool;
	UWorld* WorldContext;
};