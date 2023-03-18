#pragma once

#include "IMemoryInterface.h"
#include "Overlay.h"

class TestOverlay : public Overlay
{
public:
	TestOverlay(IMemoryInterface* pMemInterface, const char* title, int updateRate);
	TestOverlay(const TestOverlay& overlay) = delete;
	void UpdateImGui() override;
	void DrawImGui() override;
	bool ShouldShowMenu() override;
	void OnFocusFound() override;
	void OnFocusLost() override;
private:
	IMemoryInterface* pMemInterface;
};

