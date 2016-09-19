#include <list>
#include <algorithm>

#ifndef CONTROLS_HPP
#define CONTROLS_HPP

//Структура для связи цвета грани куба с текстурой
struct color_texture
{
	GLfloat red;
	GLfloat green;
	GLfloat blue;
	int TextureID;
};

//Структура для связи цвета грани куба с матрицей вращения фронтальной грани для перехода ее в положение гарани данного цвета
struct color_rotate_front
{
	GLfloat red;
	GLfloat green;
	GLfloat blue;
	glm::mat4 rotate_front;
};

//Класс управления вращением и перетаскиванием
class Controls
{
private:
	GLFWwindow* window;

	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;
	glm::mat4 ModelMatrix;
	glm::mat4 RotateMatrix;

	// Позиция камеры
	glm::vec3 position;
	GLfloat Distance_MovTex;

	// Угол обзора камеры
	float initialFoV = 3.14f / 4;

	//Выделенная текстура
	int picked_texture;

	//Список цвет грани - текстура
	std::list<color_texture> ls_color_texture;

	//Матрица перемещения текстуры при перетягивании
	glm::mat4 TransMovedTexMatrix;

	//Список связей цвета грани с матрицей вращения фронтальной грани для перевода ее в положение грани данного цвета
	std::list<color_rotate_front> ls_color_rotate_front;

	//Векторы начала и конца вращения
	glm::vec3 vxy0, vxyz;

	//Вектор, вокруг которого происходит вращение и его начальное значение
	glm::vec3 vrot = glm::vec3(0, 0, 1);

	//Угол вращения и начальное значение.
	float RotateAngle = 0.0f;

	//Состояние кнопок мыши
	bool rmb_down, rmb_up, lmb_down, lmb_up;

	//Цвет пикселя под курсором
	unsigned char pixel_color[4];
	GLfloat red;
	GLfloat green;
	GLfloat blue;

	//Положение курсора
	double xpos;
	double ypos;

	//Начальная матрица вращения
	glm::mat4 RotateMatrix0;

	//Направление камеры
	glm::vec3 direction = glm::vec3(0, 0, -1);
	glm::vec3 up = glm::vec3(0, 1, 0);

	void UpdPos();

	void Rotate_CUBE();

public:
	Controls(GLFWwindow*);
	~Controls();

	glm::mat4 getViewMatrix(){
		return ViewMatrix;
	}
	glm::mat4 getProjectionMatrix(){
		return ProjectionMatrix;
	}
	glm::mat4 getRotateMatrix(){
		return RotateMatrix;
	}

	glm::mat4 getModelMatrix(){
		return ModelMatrix;
	}

	//Перевод 255 в 1.0
	void setRGB_on_PixelColor();

	void setPicked_Texture();
	int getPicked_Texture();

	//Инициируем список цвет грани - матрица вращения фронтальной грани в положение грани данного цвета (для красивого перемещения текстуры)
	void InitRotateFrontToColorSide();
	glm::mat4 getRotateFrontToColorSide();

	void setTextureOnCube();
	int getTextureOnCube();

	//Добавляем связку цвет грани - ИД текстуры
	void setColorTexture(color_texture col_tex){
		std::list<color_texture>::iterator itr = getColorTexture(col_tex.red, col_tex.green, col_tex.blue);
		if (itr != ls_color_texture.end()) ls_color_texture.erase(itr);
		ls_color_texture.push_back(col_tex);
	}
	//Ищем связку цвет грани - ИД текстуры
	std::list<color_texture>::iterator getColorTexture(GLfloat red, GLfloat green, GLfloat blue){
		for (std::list<color_texture>::iterator itr = ls_color_texture.begin(); itr != ls_color_texture.end(); ++itr)
			if ((*itr).red == red && (*itr).green == green && (*itr).blue == blue)
				return itr;			
		return ls_color_texture.end();
	}

	glm::vec3 Trans_Moved_Texture();

	void ComputeRotate();

};

#endif