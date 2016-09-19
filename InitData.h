// Include GLEW
#include <GL/glew.h>

//���������� �� ������ z
const GLfloat Distance = 5.0f;

//������� ������ � ����������
const GLfloat Xmin = 0.75f;
const GLfloat Xmax = 0.95f;
const GLfloat Ymin = -0.95f;
const GLfloat Ymax = 0.95f;

//������ ������� �� �������
const GLfloat Padding = 0.03f;

const GLfloat TXmin = Xmin + Padding;
const GLfloat TXmax = Xmax - Padding;
const GLfloat TYmin = Ymin + Padding;
const GLfloat TYmax = Ymax - Padding;

//������ �����, z � ������ �� z ������������ ��������
const GLfloat LenMovTex = 0.8f;
const GLfloat Z_MovTex = 0.0f;
const GLfloat Z_lay_MovTex = 1.3f;

//���� ����
const GLfloat BGred = 0.9f;
const GLfloat BGgreen = 0.9f;
const GLfloat BGblue = 0.9f;

//������ ������������� VBO.
template <typename T_in, typename T_out>
T_out& VBO_init(const T_in* in_array, int len)
{
	T_out out_buf;
	glGenBuffers(1, &out_buf);
	glBindBuffer(GL_ARRAY_BUFFER, out_buf);
	glBufferData(GL_ARRAY_BUFFER, len, in_array, GL_STATIC_DRAW);
	return out_buf;
}

//������� ��� ����� ������
static const GLfloat SB_vertex[] = {
	Xmin, Ymin, 0.0f,
	Xmin, Ymax, 0.0f,
	Xmax, Ymax, 0.0f,
	Xmax, Ymin, 0.0f,
	Xmin, Ymin, 0.0f,
};

static const GLfloat TX_vertex_0[] = {
	TXmin, 0, 0.0f,
	TXmin, 0, 0.0f,
	TXmax, 0, 0.0f,
	TXmax, 0, 0.0f,
	TXmax, 0, 0.0f,
	TXmin, 0, 0.0f
};



//������ ������ ����
static const GLfloat V_CUBE[6][18] = {
	{
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,
	},
	{
		-1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
	},
	{
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
	},
	{
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
	},
	{
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
	},
	{
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
	}
};

// ������ ������ ����
static const GLfloat g_color_buffer_data[6][18] = {
	{
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
	},
	{
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
	},
	{
		1.0f, 0.5f, 0.0f,
		1.0f, 0.5f, 0.0f,
		1.0f, 0.5f, 0.0f,
		1.0f, 0.5f, 0.0f,
		1.0f, 0.5f, 0.0f,
		1.0f, 0.5f, 0.0f,
	},
	{
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
	},
	{
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
	},
	{
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
	}
};



//������ ������ ������������ ��������
static const GLfloat V_moved_texture[18] = {
	-LenMovTex, -LenMovTex, Z_MovTex,
	-LenMovTex, LenMovTex, Z_MovTex,
	LenMovTex, -LenMovTex, Z_MovTex,
	LenMovTex, -LenMovTex, Z_MovTex,
	LenMovTex, LenMovTex, Z_MovTex,
	-LenMovTex, LenMovTex, Z_MovTex,
};

//��������� UV ��� 2-� �������������. ����� - ���������� �� 90 Z
static const GLfloat g_uv_buffer_data[] = {
	0.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 0.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f
};
//NULL UV
static const GLfloat g_uv_NULL[] = {
	1000.0f, 1000.0f,
	1000.0f, 1000.0f,
	1000.0f, 1000.0f,
	1000.0f, 1000.0f,
	1000.0f, 1000.0f,
	1000.0f, 1000.0f,
};