#pragma once

namespace games
{
    enum class MouseEvent;
    enum class KeywordEvent;


    class IStateMachine
    {
    public:
        virtual ~IStateMachine() = 0 {};

        // state machine logic handle.
        virtual void Run() = 0;

        // call before the state first run.
        virtual void PrevRunState() = 0;
        // call before the state is tranlsation.
        virtual void PrevNextState() = 0;
    };

	//
	// 状态机进一步封装，以适应当前的基础Framework
	//
	class BaseStateMachine : public IStateMachine
	{
	public:
		BaseStateMachine() { }
		virtual ~BaseStateMachine() = 0 {
			DestroyEventCallback();
			DestroyRenderCallback();
		};

		virtual void PrevRunState() override final;
		virtual void PrevNextState() override final;

		virtual void OnRender() = 0;
		virtual void OnRenderUI() = 0;
		virtual void OnMouseEvent(MouseEvent event, int x, int y) = 0;
		virtual void OnKeywordEvent(KeywordEvent event, unsigned int state) = 0;

	protected:
		virtual void RegisterEventCallback();
		virtual void DestroyEventCallback();

		virtual void RegisterRenderCallback();
		virtual void DestroyRenderCallback();

	};

}

