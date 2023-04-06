#pragma once

class RenderPipeline
{
private:
	std::vector<ShaderProgram> programs;


public:
	void initShaders();
	void setAttributeLayouts();
	void addProgram(ShaderProgram aProgram);
	void updateMatrices(int* width, int* height);
};