// ELEFTERIOS LAZARIDIS
#include "Keyboard.h"

// Keyboard Event
Keyboard::KeyboardEvent::KeyboardEvent():
	type(EventType::Invalid), keycode(0)
{
}

Keyboard::KeyboardEvent::KeyboardEvent(const EventType type, const unsigned char keycode):
	type(type), keycode(keycode)
{
}

bool Keyboard::KeyboardEvent::IsPress() const
{
	return type == EventType::Press;
}

bool Keyboard::KeyboardEvent::IsRelease() const
{
	return type == EventType::Release;
}

bool Keyboard::KeyboardEvent::IsValid() const
{
	return type != EventType::Invalid;
}

unsigned char Keyboard::KeyboardEvent::GetKeyCode() const
{
	return keycode;
}

// Keyboard
Keyboard::Keyboard()
{
	for (int i = 0; i < 256; i++)
	{
		keyStates[i] = false;
	}
}

bool Keyboard::KeyIsPressed(const unsigned char keycode) const
{
	return keyStates[keycode];
}

bool Keyboard::KeyBufferIsEmpty() const
{
	return keyBuffer.empty();
}

bool Keyboard::CharBufferIsEmpty() const
{
	return charBuffer.empty();
}

Keyboard::KeyboardEvent Keyboard::ReadKey()
{
	if (keyBuffer.empty()) // if no keys to read
	{
		return Keyboard::KeyboardEvent(); // return Inavalid keyboard event (default constructor)
	}
	else
	{
		Keyboard::KeyboardEvent e = keyBuffer.front(); // get keyboard event from queue
		keyBuffer.pop(); // remove first key from queue
		return e; // return keyboard event
	}
}

unsigned char Keyboard::ReadChar()
{
	if (charBuffer.empty()) // if no chars to read
	{
		return 0; // return null char
	}
	else
	{
		unsigned char e = charBuffer.front(); // get first char from queue
		charBuffer.pop(); // remove first char from queue
		return e; // return char
	}
}

void Keyboard::OnKeyPressed(const unsigned char keycode)
{
	keyStates[keycode] = true;
	keyBuffer.push(Keyboard::KeyboardEvent(Keyboard::KeyboardEvent::EventType::Press, keycode));
}

void Keyboard::OnKeyReleased(const unsigned char keycode)
{
	keyStates[keycode] = false;
	keyBuffer.push(Keyboard::KeyboardEvent(Keyboard::KeyboardEvent::EventType::Release, keycode));
}

void Keyboard::OnChar(const unsigned char character)
{
	charBuffer.push(character);
}

void Keyboard::EnableAutorepeat()
{
	autoreapeatEnabled = true;
}

void Keyboard::DisableAutorepeat()
{
	autoreapeatEnabled = false;
}

bool Keyboard::AutorepeatIsEnabled() const
{
	return autoreapeatEnabled;
}