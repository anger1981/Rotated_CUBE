// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3.lib")

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "shader.hpp"
#include "texture.hpp"
#include "controls.hpp"
#include "InitData.h"



int main(void)
{
	// Инициализация GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 8);

	// Создаем окно
	window = glfwCreateWindow(1024, 768, "Colored Cube", NULL, NULL);
	if (window == NULL){
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Инициализация GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	//Задаем фон
	glClearColor(BGred, BGgreen, BGblue, 0.0f);

	// Делаем доступной глубину и прозрачность
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	//Не прорисовываем скрытые элементы
	glDepthFunc(GL_LESS);

	// Подключаем шэйдеры куба
	GLuint CUBE_programID = LoadShaders("CUBE.vertexshader", "CUBE.fragmentshader");
	// Подключаем шэйдеры границы панели загруженных текстур
	GLuint SideBar_programID = LoadShaders("SideBarVertexShader.vertexshader", "SideBarFragmentShader.fragmentshader");
	// Подключаем шэйдеры загруженных текстур
	GLuint SB_Texture_programID = LoadShaders("SB_TransformVertexShader.vertexshader", "SB_TextureFragmentShader.fragmentshader");
	// Подключаем шэйдеры перемещаемой текстуры
	GLuint MovedTexture_programID = LoadShaders("MovedTexture.vertexshader", "MovedTexture.fragmentshader");

	//Получаем список всех файлов текстур в директории программы
	std::list<std::string> listFileTexture;
	GetAllTextures(listFileTexture);
	
	//Массив загруженных текстур
	GLuint* Texture = new GLuint[listFileTexture.size()];

	//Массив вершин треугольников для наложения текстур и отображения их на панели
	GLuint* SBTvertex = new GLuint[listFileTexture.size()];
	GLfloat** TX_vertex_buffer_data_ = new GLfloat*[listFileTexture.size()];

	//Массив цветов заполнения треугольников для получения индекса текстуры по выбранному курсором цвету.
	GLuint* SBTcolorbuffer = new GLuint[listFileTexture.size()];
	GLfloat** TX_color_buffer_data_ = new GLfloat*[listFileTexture.size()];
	
	//Инициализация массивов вершин треугольников панели для наложения на них загруженных текстур
	//и массивов цветов для получения индекса текстуры на панели по цвету r-канала
	for (std::list<std::string>::iterator itr = listFileTexture.begin(); itr != listFileTexture.end(); ++itr)
	{	
		static int i = 0;
		Texture[i] = loadJPG((*itr).c_str());
		TX_vertex_buffer_data_[i] = new GLfloat[18];
		TX_color_buffer_data_[i] = new GLfloat[18];
		for (int j = 0; j < 18; j++)
		{
			switch (j)
			{
			case 1: TX_vertex_buffer_data_[i][j] = TYmax - ((TXmax - TXmin + Padding)*i + TXmax - TXmin) * (4.0f / 3.0f);
				break;
			case 4: TX_vertex_buffer_data_[i][j] = TYmax - ((TXmax - TXmin + Padding)*i) * (4.0f / 3.0f);
				break;
			case 7: TX_vertex_buffer_data_[i][j] = TYmax - ((TXmax - TXmin + Padding)*i + TXmax - TXmin) * (4.0f / 3.0f);
				break;
			case 10: TX_vertex_buffer_data_[i][j] = TYmax - ((TXmax - TXmin + Padding)*i + TXmax - TXmin) * (4.0f / 3.0f);
				break;
			case 13: TX_vertex_buffer_data_[i][j] = TYmax - ((TXmax - TXmin + Padding)*i) * (4.0f / 3.0f);
				break;
			case 16: TX_vertex_buffer_data_[i][j] = TYmax - ((TXmax - TXmin + Padding)*i) * (4.0f / 3.0f);
				break;
			default:
				TX_vertex_buffer_data_[i][j] = TX_vertex_0[j];
				break;
			}

			((j + 6) % 3 == 0) ? TX_color_buffer_data_[i][j] = (GLfloat)(((GLfloat)i + 1.0f) / 255) : TX_color_buffer_data_[i][j] = 0.0f;
			
		}
		SBTvertex[i] = VBO_init<GLfloat, GLuint>(&TX_vertex_buffer_data_[i][0], sizeof(TX_vertex_0));//sizeof(TX_vertex_buffer_data_0)
		SBTcolorbuffer[i] = VBO_init<GLfloat, GLuint>(&TX_color_buffer_data_[i][0], sizeof(TX_vertex_0));

		i++;
	}

	// Получение хэндлов текстур в соответствующих программах шейдеров
	GLuint Loaded_TextureID = glGetUniformLocation(SB_Texture_programID, "LoadedTextures");
	fprintf(stderr, " Loaded_TextureID = %i \n", Loaded_TextureID);
	GLuint Added_TextureID = glGetUniformLocation(CUBE_programID, "AddedTextures");
	fprintf(stderr, " Added_TextureID = %i \n", Added_TextureID);
	GLuint Moved_TextureID = glGetUniformLocation(MovedTexture_programID, "MovedTextures");
	fprintf(stderr, " Moved_TextureID = %i \n", Moved_TextureID);


	// Получение хэндлов MVP-матриц
	GLuint MatrixID = glGetUniformLocation(CUBE_programID, "MVP");
	GLuint MovTex_MatrixID = glGetUniformLocation(MovedTexture_programID, "MVP");

	//Инициализация VBO
	GLuint SB_vertexbuffer = VBO_init<GLfloat, GLuint>(&SB_vertex[0], sizeof(SB_vertex));
	GLuint MovTex_vertexbuffer = VBO_init<GLfloat, GLuint>(&V_moved_texture[0], sizeof(V_moved_texture));
	GLuint SBT_UV_buffer = VBO_init<GLfloat, GLuint>(&g_uv_buffer_data[0], sizeof(g_uv_buffer_data));
	GLuint SBT_UV_NULL = VBO_init<GLfloat, GLuint>(&g_uv_NULL[0], sizeof(g_uv_NULL));

	GLuint* vertexbuffer = new GLuint[6];
	GLuint* colorbuffer = new GLuint[6];

	for (int k = 0; k < 6; k++)
	{
		vertexbuffer[k] = VBO_init<GLfloat, GLuint>(&V_CUBE[k][0], sizeof(V_CUBE[k]));
		colorbuffer[k] = VBO_init<GLfloat, GLuint>(&g_color_buffer_data[k][0], sizeof(g_color_buffer_data[k]));
	}

	Controls* ctrl = new Controls(window);


	do{

		ctrl->ComputeRotate();
		glm::mat4 ProjectionMatrix = ctrl->getProjectionMatrix();
		glm::mat4 ViewMatrix = ctrl->getViewMatrix();
		glm::mat4 RotateMatrix = ctrl->getRotateMatrix();
		glm::mat4 ModelMatrix = ctrl->getModelMatrix();
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix * RotateMatrix;//

#pragma region Draw colored cube

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Use our shader
		glUseProgram(CUBE_programID);

		glActiveTexture(GL_TEXTURE0);

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		for (int k = 0; k < 6; k++)
		{

			// 1rst attribute buffer : vertices
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[k]);
			glVertexAttribPointer(
				0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
				);

			// 2nd attribute buffer : colors
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, colorbuffer[k]);
			glVertexAttribPointer(
				1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
				3,                                // size
				GL_FLOAT,                         // type
				GL_FALSE,                         // normalized?
				0,                                // stride
				(void*)0                          // array buffer offset
				);

			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, SBT_UV_NULL);
			glVertexAttribPointer(
				2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
				2,                                // size
				GL_FLOAT,                         // type
				GL_FALSE,                         // normalized?
				0,                                // stride
				(void*)0                          // array buffer offset
				);

			// Draw the triangle !
			glDrawArrays(GL_TRIANGLES, 0, 6); // 12*3 indices starting at 0 -> 12 triangles

			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);
		}
#pragma endregion

#pragma region DrawColorToTextureInSideBar
		// Use our shader
		glUseProgram(SB_Texture_programID);

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		//glUniformMatrix4fv(SBT_MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);

		for (int i = 0; i < listFileTexture.size(); i++)
		{
			glBindTexture(GL_TEXTURE_2D, Texture[i]);
			// Set our "myTextureSampler" sampler to user Texture Unit 0
			glUniform1i(Loaded_TextureID, 0);

			// 1rst attribute buffer : vertices
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, SBTvertex[i]);
			glVertexAttribPointer(
				0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
				);

			// 2nd attribute buffer : colors
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, SBTcolorbuffer[i]);
			glVertexAttribPointer(
				1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
				3,                                // size
				GL_FLOAT,                         // type
				GL_FALSE,                         // normalized?
				0,                                // stride
				(void*)0                          // array buffer offset
				);

			// 3 attribute buffer : UVs
			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, SBT_UV_NULL);
			glVertexAttribPointer(
				2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
				2,                                // size : U+V => 2
				GL_FLOAT,                         // type
				GL_FALSE,                         // normalized?
				0,                                // stride
				(void*)0                          // array buffer offset
				);

			// Draw the triangle !
			glDrawArrays(GL_TRIANGLES, 0, 2 * 3); // 12*3 indices starting at 0 -> 12 triangles

			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);
		}
#pragma endregion

		ctrl->setPicked_Texture();//////////////////////////////////////////  pick texture **************************************
		ctrl->setTextureOnCube();
		//int Picked_Side = ctrl->getPicked_Side();

		glm::mat4 RotFrontToColorSide = ctrl->getRotateFrontToColorSide();

		//Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


#pragma region Draw colored-textured cube
		// Use our shader
		glUseProgram(CUBE_programID);

		glActiveTexture(GL_TEXTURE0);

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		for (int k = 0; k < 6; k++)
		{
			int TextureID = (*ctrl->getColorTexture(g_color_buffer_data[k][0], g_color_buffer_data[k][1], g_color_buffer_data[k][2])).TextureID;
			if (TextureID > 0)
				glBindTexture(GL_TEXTURE_2D, Texture[TextureID - 1]);
			glUniform1i(Added_TextureID, 0);

			// 1rst attribute buffer : vertices
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[k]);
			glVertexAttribPointer(
				0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
				);

			// 2nd attribute buffer : colors
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, colorbuffer[k]);
			glVertexAttribPointer(
				1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
				3,                                // size
				GL_FLOAT,                         // type
				GL_FALSE,                         // normalized?
				0,                                // stride
				(void*)0                          // array buffer offset
				);




			glEnableVertexAttribArray(2);
			if (TextureID > 0)
			{
				glBindBuffer(GL_ARRAY_BUFFER, SBT_UV_buffer);
				glVertexAttribPointer(
					2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
					2,                                // size
					GL_FLOAT,                         // type
					GL_FALSE,                         // normalized?
					0,                                // stride
					(void*)0                          // array buffer offset
					);
			}
			else
			{
				//glBindTexture(GL_TEXTURE_2D, Texture[0]);
				///glUniform1i(Added_TextureID, 0);

				glBindBuffer(GL_ARRAY_BUFFER, SBT_UV_NULL);
				glVertexAttribPointer(
					2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
					2,                                // size
					GL_FLOAT,                         // type
					GL_FALSE,                         // normalized?
					0,                                // stride
					(void*)0                          // array buffer offset
					);
			}
			// Draw the triangle !
			glDrawArrays(GL_TRIANGLES, 0, 6); // 12*3 indices starting at 0 -> 12 triangles

			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);
		}

#pragma endregion

#pragma region Draw Moved Texture
		// Use our shader
		glUseProgram(MovedTexture_programID);

		glActiveTexture(GL_TEXTURE0);

		glm::mat4 transMVP;

			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

			glm::vec3 trans = ctrl->Trans_Moved_Texture();

			transMVP = glm::translate(MVP, trans);

			if (RotFrontToColorSide[0][0] != 1000.0f) transMVP = transMVP * RotateMatrix * RotFrontToColorSide;


		glUniformMatrix4fv(MovTex_MatrixID, 1, GL_FALSE, &transMVP[0][0]);

		int TextureID = ctrl->getPicked_Texture();
		if (TextureID > 0)
		{
			glBindTexture(GL_TEXTURE_2D, Texture[TextureID - 1]);
			glUniform1i(Moved_TextureID, 0);

			// 1rst attribute buffer : vertices
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, MovTex_vertexbuffer);  // Picked_Side == -1 ? MovTex_vertexbuffer : vertexbuffer[Picked_Side]);
			glVertexAttribPointer(
				0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
				);

			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, SBT_UV_buffer);
			glVertexAttribPointer(
				1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
				2,                                // size
				GL_FLOAT,                         // type
				GL_FALSE,                         // normalized?
				0,                                // stride
				(void*)0                          // array buffer offset
				);

			// Draw the triangle !
			glDrawArrays(GL_TRIANGLES, 0, 6); // 12*3 indices starting at 0 -> 12 triangles

			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
		}		

#pragma endregion

#pragma region Draw SideBar border
		glUseProgram(SideBar_programID);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, SB_vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);

		// Draw the triangle !
		glDrawArrays(GL_LINE_STRIP, 0, 5); // 3 indices starting at 0 -> 1 triangle

		glDisableVertexAttribArray(0);
#pragma endregion


#pragma region DrawTextureInSideBar
		// Use our shader
		glUseProgram(SB_Texture_programID);

		glActiveTexture(GL_TEXTURE0);

		for (int i = 0; i < listFileTexture.size(); i++)
		{
			glBindTexture(GL_TEXTURE_2D, Texture[i]);
			// Set our "myTextureSampler" sampler to user Texture Unit 0
			glUniform1i(Loaded_TextureID, 0);

			// 1rst attribute buffer : vertices
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, SBTvertex[i]);
			glVertexAttribPointer(
				0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
				);

			// 2nd attribute buffer : colors
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, SBTcolorbuffer[i]);
			glVertexAttribPointer(
				1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
				3,                                // size
				GL_FLOAT,                         // type
				GL_FALSE,                         // normalized?
				0,                                // stride
				(void*)0                          // array buffer offset
				);

			// 2nd attribute buffer : UVs
			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, SBT_UV_buffer);
			glVertexAttribPointer(
				2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
				2,                                // size : U+V => 2
				GL_FLOAT,                         // type
				GL_FALSE,                         // normalized?
				0,                                // stride
				(void*)0                          // array buffer offset
				);

			// Draw the triangle !
			glDrawArrays(GL_TRIANGLES, 0, 2 * 3); // 12*3 indices starting at 0 -> 12 triangles

			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);
		}
#pragma endregion

		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
	glfwWindowShouldClose(window) == 0);

	delete ctrl;

	// Cleanup VBO and shader
	for (int i = 0; i < 6; i++)
	{
		glDeleteBuffers(1, &vertexbuffer[i]);
		glDeleteBuffers(1, &colorbuffer[i]);
	}
	glDeleteBuffers(1, &SB_vertexbuffer);
	for (int i = 0; i < listFileTexture.size(); i++)
	{
		glDeleteBuffers(1, &SBTvertex[i]);
	}
	glDeleteBuffers(1, &SBT_UV_buffer);
	glDeleteProgram(CUBE_programID);
	glDeleteProgram(SideBar_programID);
	glDeleteProgram(SB_Texture_programID);
	glDeleteProgram(MovedTexture_programID);

	//glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}