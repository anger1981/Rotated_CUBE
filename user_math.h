#include <math.h>

//��������� z �� ������ ������� � xy
	float z_on_lxy(float l, float x, float y)
	{
		l = l * l;
		x = x * x;
		y = y * y;
		return (l > x + y) ? sqrt(l - x - y) : 0;
	}
	