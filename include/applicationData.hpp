#pragma once

// Includes from project
#include "../src/view/OpenGl/OpenGl3DWidget.hpp"
#include "../src/view/OpenGl/object3D.hpp" 

// Includes from STL
#include <memory>


class ApplicationData
{
public:
	Object3D m_monkey3D;

public:
	ApplicationData();
	~ApplicationData() {};
};