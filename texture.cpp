#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#define STBI_NO_GIF 1
#define STB_IMAGE_IMPLEMENTATION 1

#include "stb/stb_image.h"

#include "texture.hpp"

void GetAllTextures(std::list<std::string>& ls_str_filename)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE h;
	h = FindFirstFile("*.jpg", &FindFileData);
	if (h != INVALID_HANDLE_VALUE)
	{
		do
		{
			ls_str_filename.push_back(std::string(FindFileData.cFileName));
		} while (FindNextFile(h, &FindFileData) != 0);
		FindClose(h);
	}
}

//переворот данных верх на низ в текстуре
void flipTexture(unsigned char *textureData, int width, int height, int n) {
	for (int h = 0; h < height / 2; ++h) {
		for (int w = 0; w < width; ++w) {
			for (int i = 0; i < n; ++i) {
				int offset1 = (h*width + w)*n + i;
				int offset2 = ((height - h - 1)*width + w)*n + i;
				unsigned char t = textureData[offset1];
				textureData[offset1] = textureData[offset2];
				textureData[offset2] = t;
			}
		}
	}
}

GLuint loadJPG(const char * imagepath){

	printf("Reading image %s\n", imagepath);

	int width, height, n; // ширина, высота, байт на пиксель

	// читаем файл, при необходимости конвертируем в 3 байта на пиксель
	unsigned char *textureData = stbi_load(imagepath, &width, &height, &n, 3);
	if (textureData == nullptr) {
		printf("loadTexture failed, fname =  %s\n", imagepath);
		return false;
	}

	flipTexture(textureData, width, height, n);
	// Генерируем текстуру
	GLuint textureID;
	glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);


	// ... Фильтрация для МипМэп
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Освобождаем память
	stbi_image_free(textureData);	

	return textureID;
}