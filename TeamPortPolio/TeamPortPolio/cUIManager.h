#pragma once

#include "Singleton.h"
#include "cUIImage.h"
#include "cUIText.h"
#include "cUIMiniMap.h"
#include "cUIButton.h"
#include "cUITab.h"
#include "cUISlot.h"

#define UI cUIManager::Instance()

class cUIManager : public Singleton<cUIManager>
{
	vector<cUIImage*>			m_vecImage;
	vector<cUIText*>			m_vecText;
	vector<cUIButton*>			m_vecShownBtn;
	vector<cUIButton*>			m_vecEventBtn;
	vector<cUITab*>				m_vecTab;
	cUIMiniMap*					m_pMiniMap;

	void Setup_TitleScene();
	void Setup_TownScene();
	void Setup_LoginScene();
	void Setup_SelectScene();

public:
	void Setup();
	void Release();
	void Update(float deltaTime);
	void Render(LPD3DXSPRITE pSprite);
	void Change(int sceneID);
	void PressKey();
	void SetEvent(int order);
	void GetEvent(OUT int& minimapIndex, OUT int& buttonIndex, OUT int& eventID, OUT int& itemID);


};

