#include <list>
#include <algorithm>

#ifndef CONTROLS_HPP
#define CONTROLS_HPP

//��������� ��� ����� ����� ����� ���� � ���������
struct color_texture
{
	GLfloat red;
	GLfloat green;
	GLfloat blue;
	int TextureID;
};

//��������� ��� ����� ����� ����� ���� � �������� �������� ����������� ����� ��� �������� �� � ��������� ������ ������� �����
struct color_rotate_front
{
	GLfloat red;
	GLfloat green;
	GLfloat blue;
	glm::mat4 rotate_front;
};

//����� ���������� ��������� � ���������������
class Controls
{
private:
	GLFWwindow* window;

	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;
	glm::mat4 ModelMatrix;
	glm::mat4 RotateMatrix;

	// ������� ������
	glm::vec3 position;
	GLfloat Distance_MovTex;

	// ���� ������ ������
	float initialFoV = 3.14f / 4;

	//���������� ��������
	int picked_texture;

	//������ ���� ����� - ��������
	std::list<color_texture> ls_color_texture;

	//������� ����������� �������� ��� �������������
	glm::mat4 TransMovedTexMatrix;

	//������ ������ ����� ����� � �������� �������� ����������� ����� ��� �������� �� � ��������� ����� ������� �����
	std::list<color_rotate_front> ls_color_rotate_front;

	//������� ������ � ����� ��������
	glm::vec3 vxy0, vxyz;

	//������, ������ �������� ���������� �������� � ��� ��������� ��������
	glm::vec3 vrot = glm::vec3(0, 0, 1);

	//���� �������� � ��������� ��������.
	float RotateAngle = 0.0f;

	//��������� ������ ����
	bool rmb_down, rmb_up, lmb_down, lmb_up;

	//���� ������� ��� ��������
	unsigned char pixel_color[4];
	GLfloat red;
	GLfloat green;
	GLfloat blue;

	//��������� �������
	double xpos;
	double ypos;

	//��������� ������� ��������
	glm::mat4 RotateMatrix0;

	//����������� ������
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

	//������� 255 � 1.0
	void setRGB_on_PixelColor();

	void setPicked_Texture();
	int getPicked_Texture();

	//���������� ������ ���� ����� - ������� �������� ����������� ����� � ��������� ����� ������� ����� (��� ��������� ����������� ��������)
	void InitRotateFrontToColorSide();
	glm::mat4 getRotateFrontToColorSide();

	void setTextureOnCube();
	int getTextureOnCube();

	//��������� ������ ���� ����� - �� ��������
	void setColorTexture(color_texture col_tex){
		std::list<color_texture>::iterator itr = getColorTexture(col_tex.red, col_tex.green, col_tex.blue);
		if (itr != ls_color_texture.end()) ls_color_texture.erase(itr);
		ls_color_texture.push_back(col_tex);
	}
	//���� ������ ���� ����� - �� ��������
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