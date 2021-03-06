#pragma once

class GC
{
public:
	enum class eMultiplayerMode
	{
		HOST,
		JOIN
	};

	static bool GenerateLightData;
	static bool GenerateCubeMap;
	static int SHNodeSize;

	static bool EnableCheapAmbient;
	static bool EnableVSync;
	static bool UseLowModels;
	static bool EnableDynamicShadows;

	static int Difficulty;
	static float DamageMultiplier[3];
	static float SpawnIntervalMultiplier[3];

	static volatile bool PlayerAlive;

	static eMultiplayerMode MultiplayerMode;

	static bool ShouldRenderGUI;

	static float PlayerHeightWithLegs;
	static float PlayerRadius;

	static bool OptionsUseViberations;
	static bool OptionsUseShadows;
	static bool OptionsEnableOffline;

	static bool EnableCheat;
	static bool HasCheatFiles;

	static int TotalLevels;
	static int TotalNightmareLevels;

	static bool NightmareMode;
	static bool HasWonGame;
	static bool HasBeenInVictoryScreen;
	static bool HasBeenInVictoryScreenNightmare;

	static int Gold;

	static int CurrentActivePlayers;

	static bool FirstTimeScoreSubmit;

	static int DebugRenderTexture;

	static float ShadowBufferSize;

	static bool HasShownHowToUseHats;
};