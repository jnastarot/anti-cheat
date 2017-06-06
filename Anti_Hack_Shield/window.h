#pragma once
class window
{
	HWND wnd;

	ATOM window::WRegisterClass(std::string winclass);
public:
	window::window(std::string name,std::string winclass);
	window::~window();

	bool window::onMessage();
	HWND window::getSelfHandle();
};

