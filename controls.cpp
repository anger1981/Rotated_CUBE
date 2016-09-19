
#include <stdio.h>
#include <math.h>
#include "InitData.h"


#include <GLFW/glfw3.h>


#include "user_math.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "controls.hpp"


Controls::Controls(GLFWwindow* Window)
{
	window = Window;
	rmb_down = true;
	rmb_up = true;
	lmb_down = true;
	lmb_up = true;

	position = glm::vec3(0, 0, Distance);
	Distance_MovTex = Distance - Z_lay_MovTex - Z_MovTex;

	ProjectionMatrix = glm::perspective(initialFoV, 4.0f / 3.0f, 0.1f, 100.0f);
	ViewMatrix = glm::lookAt(position, glm::vec3(0, 0, 0), up);
	ModelMatrix = glm::mat4(1.0f);
	RotateMatrix0 = glm::rotate(glm::mat4(), RotateAngle, vrot);

	picked_texture = 0;

	InitRotateFrontToColorSide();
}

Controls::~Controls()
{
}

//Перевод 255 в 1.0
void Controls::setRGB_on_PixelColor()
{
	red = (GLfloat)(round(100.0f * (((GLfloat)pixel_color[0]) / 255.0f)) / 100.0f);
	green = (GLfloat)(round(100.0f * (((GLfloat)pixel_color[1]) / 255.0f)) / 100.0f);
	blue = (GLfloat)(round(100.0f * (((GLfloat)pixel_color[2]) / 255.0f)) / 100.0f);
}

//Инициируем список "цвет грани" - "матрица вращения фронтальной грани в положение грани данного цвета" (для красивого перемещения текстуры)
void Controls::InitRotateFrontToColorSide()
{

	ls_color_rotate_front.push_back({ g_color_buffer_data[0][0], g_color_buffer_data[0][1], g_color_buffer_data[0][2], glm::rotate(glm::mat4(), -3.14f / 2, glm::vec3(0, 1, 0)) });
	ls_color_rotate_front.push_back({ g_color_buffer_data[1][0], g_color_buffer_data[1][1], g_color_buffer_data[1][2], glm::rotate(glm::mat4(), 0.0f, glm::vec3(0, 1, 0)) });
	ls_color_rotate_front.push_back({ g_color_buffer_data[2][0], g_color_buffer_data[2][1], g_color_buffer_data[2][2], glm::rotate(glm::mat4(), 3.14f / 2, glm::vec3(0, 1, 0)) });
	ls_color_rotate_front.push_back({ g_color_buffer_data[3][0], g_color_buffer_data[3][1], g_color_buffer_data[3][2], glm::rotate(glm::mat4(), 3.14f, glm::vec3(0, 1, 0)) });
	ls_color_rotate_front.push_back({ g_color_buffer_data[4][0], g_color_buffer_data[4][1], g_color_buffer_data[4][2], glm::rotate(glm::mat4(), -3.14f / 2, glm::vec3(1, 0, 0)) });
	ls_color_rotate_front.push_back({ g_color_buffer_data[5][0], g_color_buffer_data[5][1], g_color_buffer_data[5][2], glm::rotate(glm::mat4(), 3.14f / 2, glm::vec3(1, 0, 0)) });
}

//Получаем матрицу вращения фронтальной грани в положение грани цвета пикселя под курсором (для красивого перемещения текстуры)
glm::mat4 Controls::getRotateFrontToColorSide()
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
	
		if (picked_texture != 0)
		{
			UpdPos();
			glReadPixels(xpos, 768 - ypos, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel_color);
			setRGB_on_PixelColor();

			for (std::list<color_rotate_front>::iterator itr = ls_color_rotate_front.begin(); itr != ls_color_rotate_front.end(); ++itr)
				if ((*itr).red == red && (*itr).green == green && (*itr).blue == blue)
				{
					return (*itr).rotate_front;
				}
		}
	}
	return glm::mat4(1000.0f); //Нормальный Rotate не может быть 1000. Аналог NULL
}

void Controls::ComputeRotate()
{
		UpdPos();
		Rotate_CUBE();

		if (RotateAngle > 0.0f)
			RotateMatrix = glm::rotate(glm::mat4(), RotateAngle, vrot) * RotateMatrix0;
		else RotateMatrix = RotateMatrix0;		
}

void Controls::Rotate_CUBE()
	{
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS){
			rmb_down = true;
			if (rmb_up)
			{
				vxy0 = glm::vec3((xpos - 1024 / 2), -(ypos - 768 / 2), 0);
			}
			else
			{
				vxyz = glm::vec3((xpos - 1024 / 2), -(ypos - 768 / 2), z_on_lxy(glm::length(vxy0), (xpos - 1024 / 2), (ypos - 768 / 2)));
				vrot = glm::cross(vxy0, vxyz);
				RotateAngle = glm::acos(glm::dot(vxy0, vxyz) / (glm::length(vxy0) *glm::length(vxyz)));
			}
			rmb_up = false;
		}

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE){
			rmb_up = true;
			rmb_down = false;
			RotateMatrix0 = RotateMatrix;
			RotateAngle = 0.0f;
			vrot = glm::vec3(0, 0, 1);
		}
	}

//Двигаем текстуру за курсором используя матрицу перемещения
glm::vec3 Controls::Trans_Moved_Texture()
{
	GLfloat X = (GLfloat)((2 * xpos / 1024) - 1) * Distance_MovTex * tan((initialFoV / 2)) * (4.0f / 3.0f);
	GLfloat Y = (GLfloat)(-((2 * ypos / 768) - 1)) * Distance_MovTex * tan(initialFoV / 2);
	return  glm::vec3(X, Y, Z_lay_MovTex);
}

//Выделяем текстуру либо на панели по цвету, либо на кубе
void Controls::setPicked_Texture()
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
		lmb_down = true;
		if (lmb_up)
		{
			glReadPixels(xpos, 768 - ypos, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel_color);
			setRGB_on_PixelColor();

			if (!(red == BGred && green == BGgreen && blue == BGblue))
			if ((xpos > (1024 / 2) * (1 + Xmin))) picked_texture = (int)pixel_color[0]; //Если курсор на панели - красная составляющая = ИД текстуры
			else //Иначе ищем на гранях куба
			{
				std::list<color_texture>::iterator itr = getColorTexture(red, green, blue);
				if (itr != ls_color_texture.end())
				{
					picked_texture = (*this->getColorTexture(red, green, blue)).TextureID;
					ls_color_texture.erase(itr);
				}
				
			}
		}
		lmb_up = false;
	}
}

int Controls::getPicked_Texture()
{
	return picked_texture;
}

//Пытаемся добавить текстуру в список цвет грани - ИД текстуры
void Controls::setTextureOnCube()
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE){
		lmb_up = true;
		if (lmb_down && picked_texture!=0)
		{
			UpdPos();
			glReadPixels(xpos, 768 - ypos, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel_color);
			setRGB_on_PixelColor();

			if ((xpos < (1024 / 2) * (1 + Xmin)) && !(red == BGred && green == BGgreen && blue == BGblue))
			{
				this->setColorTexture({ red, green, blue, picked_texture });
			}
			picked_texture = 0;

		}
		lmb_down = false;
	}
}

void Controls::UpdPos()
{
	glfwGetCursorPos(window, &xpos, &ypos);
}