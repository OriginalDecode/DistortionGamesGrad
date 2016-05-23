#pragma once
#include "GameState.h"

class ScoreWidget;

struct Score;
struct ScoreInfo;

class ScoreState : public GameState
{
public:
	ScoreState(const CU::GrowingArray<const Score*>& someScores, const ScoreInfo& aScoreInfo);
	~ScoreState();

	void InitState(StateStackProxy* aStateStackProxy, CU::ControllerInput* aController, GUI::Cursor* aCursor) override;
	void EndState() override;
	const eStateStatus Update(const float& aDeltaTime) override;
	void Render() override;
	void ResumeState() override;
	void PauseState() override;
	void OnResize(int aWidth, int aHeight) override;

private:
	void operator=(const ScoreState&) = delete;
	GUI::GUIManager* myGUIManager;

	const CU::GrowingArray<const Score*>& myScores;
	const ScoreInfo& myScoreInfo;
	CU::GrowingArray<ScoreWidget*> myScoreWidgets;
};

