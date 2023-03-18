#pragma once

class Overlay
{
public:
	Overlay(const char* title, int updateRate);
	~Overlay();
	virtual void UpdateImGui() {
	}

	virtual void OnFocusFound() {
	}

	virtual void OnFocusLost() {
	}

	void Update();
	void Draw();
	bool IsRunning();
	virtual void DrawImGui() = 0;
	virtual bool ShouldShowMenu() = 0;

	bool isMenuOpen;
private:
	const char* title;
};
