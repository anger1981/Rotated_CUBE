#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <list>

//Генерируем список всех jpg файлов в текущей директории.
void GetAllTextures(std::list<std::string>& ls_str_filename);

//Загрузка .jpg текстур
GLuint loadJPG(const char * imagepath);

#endif