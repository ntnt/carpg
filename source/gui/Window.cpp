#include "Pch.h"
#include "Base.h"
#include "Window.h"
#include "MenuBar.h"
#include "ToolStrip.h"

using namespace gui;

Window::Window(bool fullscreen) : Container(true), menu(nullptr), toolstrip(nullptr), fullscreen(fullscreen)
{
	size = INT2(300, 200);
}

Window::~Window()
{
}

void Window::Draw(ControlDrawData*)
{
	GUI.DrawArea(body_rect, layout->window.background);

	if(!fullscreen)
	{
		GUI.DrawArea(header_rect, layout->window.header);
		if(!text.empty())
		{
			RECT r = header_rect.ToRect(layout->window.padding);
			GUI.DrawText(layout->window.font, text, DT_LEFT | DT_VCENTER, layout->window.font_color, r, &r);
		}
	}
	
	RECT r = body_rect.ToRect();
	ControlDrawData cdd = { &r };
	Container::Draw(&cdd);
}

void Window::Update(float dt)
{
	Container::Update(dt);

	if(mouse_focus && IsInside(GUI.cursor_pos))
		TakeFocus();
}

void Window::Event(GuiEvent e)
{
	switch(e)
	{
	case GuiEvent_Initialize:
		{
			if(fullscreen)
			{
				size = GUI.wnd_size;
				pos = INT2(0, 0);
			}
			body_rect = BOX2D(float(pos.x), float(pos.y), float(pos.x + size.x), float(pos.y + size.y));
			if(menu)
				menu->Initialize();
			if(toolstrip)
				toolstrip->Initialize();
			CalculateArea();
			for(Control* c : ctrls)
			{
				if(c == menu || c == toolstrip)
					continue;
				if(c->IsDocked())
				{
					c->pos = INT2(area.v1);
					c->global_pos = c->pos + global_pos;
					c->size = INT2(area.Size());
				}
				c->Initialize();
			}
		}
		break;
	case GuiEvent_WindowResize:
		if(fullscreen)
		{
			size = GUI.wnd_size;
			if(menu)
				menu->Event(GuiEvent_Resize);
			if(toolstrip)
				toolstrip->Event(GuiEvent_Resize);
			CalculateArea();
			for(Control* c : ctrls)
			{
				if(c == menu || c == toolstrip)
					continue;
				if(c->IsDocked())
				{
					c->pos = INT2(area.v1);
					c->global_pos = c->pos + global_pos;
					c->size = INT2(area.Size());
				}
			}
		}
		break;
	default:
		Container::Event(e);
		break;
	}
}

void Window::SetMenu(MenuBar* _menu)
{
	assert(_menu && !menu && !initialized);
	menu = _menu;
	Add(menu);
}

void Window::SetToolStrip(ToolStrip* _toolstrip)
{
	assert(_toolstrip && !toolstrip && !initialized);
	toolstrip = _toolstrip;
	Add(toolstrip);
}

void Window::CalculateArea()
{
	area.v1 = VEC2(0, 0);
	if(!fullscreen)
		area.v1.y += layout->window.header_height;
	if(menu)
		area.v1.y += menu->size.y;
	if(toolstrip)
		area.v1.y += toolstrip->size.y;
	area.v2 = size.ToVEC2();
}
