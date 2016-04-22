#pragma once

namespace games
{
	using std::map;

	enum class GameStatus
	{
		NONE,
        GAME_STATUS,
        UI_PANEL_STATUS,
        UI_STATISTICS,
	};

    class IStateMachine;

	//
	// state machine manager
	// 
	class StateMachine
	{
	public:
		StateMachine();
		~StateMachine();

		static StateMachine &Instance();

		void Run();
		void NextState(GameStatus state);
		void RegisteStateMachine(GameStatus state, IStateMachine *machine);
	
	private:
        void StateMachineInit();
		void NextState();

	private:
		map<GameStatus, IStateMachine*> m_StateHash;
		GameStatus m_CurrentStatus;	// 当前状态
		GameStatus m_NextStatus;	// 下一个状态
		bool next = false;			// 表示用户调用了 NextState(GameStatus state) 函数
	};

}

