#pragma once

//-----------------------------------------------------------------------------
class ShaderHandler
{
public:
	virtual ~ShaderHandler() {}
	virtual void OnInitOnce() {};
	virtual void OnInit() = 0;
	virtual void OnReset() = 0;
	virtual void OnReload() = 0;
	virtual void OnRelease() = 0;
};
