#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <list>

//���������� ������ ���� jpg ������ � ������� ����������.
void GetAllTextures(std::list<std::string>& ls_str_filename);

//�������� .jpg �������
GLuint loadJPG(const char * imagepath);

#endif