// ELEFTERIOS LAZARIDIS
#pragma once
#include <queue>

struct MousePoint
{
	int x;
	int y;
};

class Mouse
{
public:
	class MouseEvent
	{
	public:
		enum EventType
		{
			LPress,
			LRelease,
			RPress,
			RRelease,
			MPress,
			MRelease,
			WheelUp,
			WheelDown,
			Move,
			RAW_MOVE,
			Invalid
		};
	private:
		EventType type;
		int x;
		int y;
	public:
		MouseEvent();
		MouseEvent(const EventType type, const int x, const int y);
		bool IsValid() const;
		EventType GetType() const;
		MousePoint GetPos() const;
		int GetPosX() const;
		int GetPosY() const;
	};

private:
	std::queue<MouseEvent> eventBuffer;
	bool leftIsDown = false;
	bool rightIsDown = false;
	bool mbuttonDown = false;
	int x = 0;
	int y = 0;
public:
	void OnLeftPressed(int x, int y);
	void OnLeftReleased(int x, int y);
	void OnRightPressed(int x, int y);
	void OnRightReleased(int x, int y);
	void OnMiddlePressed(int x, int y);
	void OnMiddleReleased(int x, int y);
	void OnWheelUp(int x, int y);
	void OnWheelDown(int x, int y);
	void OnMouseMove(int x, int y);
	void OnMouseMoveRaw(int x, int y);

	bool EventBufferIsEmpty();
	Mouse::MouseEvent ReadEvent();

	bool IsLeftDown() const;
	bool IsMiddleDown() const;
	bool IsRightDown() const;

	int GetPosX() const;
	int GetPosY() const;
	MousePoint GetPos() const;
};