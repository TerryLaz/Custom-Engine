// ELEFTERIOS LAZARIDIS
#pragma once
#include <queue>

class Keyboard
{
public:
	class KeyboardEvent
	{
	public:
		enum EventType
		{
			Press,
			Release,
			Invalid
		};
	private:
		EventType type;
		unsigned char keycode;
	public:
		KeyboardEvent();
		KeyboardEvent(const EventType type, const unsigned char keycode);

		bool IsPress() const;
		bool IsRelease() const;
		bool IsValid() const;
		unsigned char GetKeyCode() const;
	};

private:
	std::queue<KeyboardEvent> keyBuffer;
	std::queue<unsigned char> charBuffer;
	bool autoreapeatEnabled = false;
	bool keyStates[256];

public:
	Keyboard();

	bool KeyIsPressed(const unsigned char keycode) const;
	bool KeyBufferIsEmpty() const;	
	bool CharBufferIsEmpty() const;
	KeyboardEvent ReadKey();
	unsigned char ReadChar();

	void OnKeyPressed(const unsigned char keycode);
	void OnKeyReleased(const unsigned char keycode);
	void OnChar(const unsigned char character);

	void EnableAutorepeat();
	void DisableAutorepeat();
	bool AutorepeatIsEnabled() const;
};