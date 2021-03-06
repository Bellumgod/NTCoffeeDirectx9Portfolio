#include "stdafx.h"
#include "Melee_State.h"


void Melee_Battle::OnBegin(cMeleeUnit * pUnit)
{
	//FindTarget(pUnit);
	pUnit->GetMesh()->SetAnimationIndexBlend(FG_BATTLERUN);
}

void Melee_Battle::OnUpdate(cMeleeUnit * pUnit, float deltaTime)
{
	if (BattleTarget == NULL)
	{
		Charge(pUnit);
	}
	else
	{
		D3DXVECTOR3 thisPos = pUnit->GetCharacterEntity()->Pos();

		D3DXVECTOR3 targetPos = BattleTarget->GetCharacterEntity()->Pos();

		float distance = MATH->Distance(thisPos, targetPos);

		if (distance > 1.0f)
		{
			pUnit->GetCharacterEntity()->Steering()->UnitArrive(targetPos);
			pUnit->GetCharacterEntity()->Steering()->ConstrainOverlap(OBJECT->GetEntities());

			FindNearTarget(pUnit);
			if (TIME->UpdateOneSecond())FindTarget(pUnit);
		}
		else
		{
			if (((cCharacter*)BattleTarget)->IsDeath() == true)
			{
				if (pUnit->GetMesh()->GetPassedTime() > pUnit->GetMesh()->GetCurrentAnim()->GetPeriod() - 0.3f)
				{
					pUnit->GetMesh()->SetAnimationIndexBlend(FG_BATTLEWALK);
				}
				BattleTarget = NULL;
				FindNearTarget(pUnit);
				if(TIME->UpdateOneSecond())FindTarget(pUnit);
			}
			else if (((cCharacter*)BattleTarget)->GetMesh()->GetIndex() != FG_DEATH)
			{
				BattleWithTarget(pUnit);
			}
		}
		pUnit->GetCharacterEntity()->Steering()->ConstrainOverlap(OBJECT->GetEntities());

	}
}

void Melee_Battle::OnEnd(cMeleeUnit * pUnit)
{
	pUnit->SetTargetObject(NULL);
}

void Melee_Battle::StateChanger(cMeleeUnit * pUnit)
{
	FG_STATE state;

}

//자신과 충돌한 적을 찾는 함수
void Melee_Battle::FindTarget(cMeleeUnit * pUnit)
{
	for (int i = 0; i < ((cLeader*)pUnit->GetTargetObject())->GetUnits().size(); i++)
	{
		if (((cLeader*)pUnit->GetTargetObject())->GetUnits()[i]->IsDeath() == false)
		{
			if (MATH->IsCollided(((cLeader*)pUnit->GetTargetObject())->GetUnits()[i]->GetArrangeSphere(), pUnit->GetArrangeSphere()))
			{
				BattleTarget = ((cLeader*)pUnit->GetTargetObject())->GetUnits()[i];
			}
		}
	}
}
//자신과 제일 가까운 적을 찾는 함수
void Melee_Battle::FindNearTarget(cMeleeUnit * pUnit)
{
	if (TIME->UpdateOneSecond())
	{
		D3DXVECTOR3 nextTargetPos, prevTargetPos;
		D3DXVECTOR3 pos = pUnit->GetCharacterEntity()->Pos();
		for (int i = 1; i < ((cLeader*)pUnit->GetTargetObject())->GetUnits().size(); i++)
		{
			if (((cLeader*)pUnit->GetTargetObject())->GetUnits()[i]->IsDeath() == false)
			{
				prevTargetPos = ((cLeader*)pUnit->GetTargetObject())->GetUnits()[i - 1]->GetCharacterEntity()->Pos();
				nextTargetPos = ((cLeader*)pUnit->GetTargetObject())->GetUnits()[i]->GetCharacterEntity()->Pos();

				if (MATH->Distance(pos, prevTargetPos) > MATH->Distance(pos, nextTargetPos))
				{
					BattleTarget = ((cLeader*)pUnit->GetTargetObject())->GetUnits()[i];
				}
				else
				{
					BattleTarget = ((cLeader*)pUnit->GetTargetObject())->GetUnits()[i - 1];
				}
			}
		}
	}
}

void Melee_Battle::Charge(cMeleeUnit * pUnit)
{
	D3DXVECTOR3 worldOffset = MATH->LocalToWorld(D3DXVECTOR3((pUnit->GetOffset().x + (pUnit->GetOffset().z*0.5F)), 0, 0), pUnit->GetLeader()->Forward());

	D3DXVECTOR3 thisPos = pUnit->GetCharacterEntity()->Pos();

	D3DXVECTOR3 targetPos = worldOffset + pUnit->GetTargetObject()->GetCharacterEntity()->Pos() - (pUnit->GetLeader()->Forward());

	float distance = MATH->Distance(thisPos, targetPos);

	if (distance > 1.0f)
	{
		pUnit->GetCharacterEntity()->Steering()->UnitArrive(targetPos);
		pUnit->GetCharacterEntity()->Steering()->ConstrainOverlap(OBJECT->GetEntities());
		FindTarget(pUnit);
	}
	else
	{
		FindNearTarget(pUnit);
	}
}

void Melee_Battle::Battle(cMeleeUnit * pUnit)
{
}

void Melee_Battle::BattleWithTarget(cMeleeUnit * pUnit)
{
	if (pUnit->GetMesh()->GetPassedTime() > pUnit->GetMesh()->GetCurrentAnim()->GetPeriod() - 0.3f)
	{
		D3DXVECTOR3 dir = BattleTarget->GetCharacterEntity()->Pos() - pUnit->GetCharacterEntity()->Pos();
		D3DXVec3Normalize(&dir, &dir);
		pUnit->GetCharacterEntity()->SetForward(dir);

		switch (pUnit->GetMesh()->GetIndex())
		{
		case FG_ATTACK1:
			pUnit->GetMesh()->SetAnimationIndexBlend(FG_ATTACK2);
			break;
		default:
			pUnit->GetMesh()->SetAnimationIndexBlend(FG_ATTACK1);
			break;
		}

		if (MATH->IsCollided(pUnit->GetAttackCollider(), ((cCharacter*)BattleTarget)->GetSphere()))
		{
			switch (pUnit->Fight(pUnit, (cCharacter*)BattleTarget))
			{
			case FIGHT_BLOCK:((cCharacter*)BattleTarget)->SetAnimBlock(); break;
			case FIGHT_HIT:((cCharacter*)BattleTarget)->SetAnimHit(); break;
			}
		}
	}
}

