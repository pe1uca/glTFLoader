#include "matrices.h"
#include <cmath>
#include <cfloat>

#define CMP(x, y) \
		(fabsf((x)-(y)) <= FLT_EPSILON *	\
			fmaxf(1.0f,						\
			fmaxf(fabsf(x), fabsf(y)))		\
		)

void Transpose(const float *srcMat, float *dstMat, int srcRows, int srcCols)
{
	for (int i = 0; i < srcRows * srcCols; i++)
	{
		dstMat[i] = srcMat[srcCols * (i % srcRows) + (i / srcRows)];
	}
}

mat2 Transpose(const mat2& matrix)
{
	mat2 result;
	Transpose(matrix.asArray, result.asArray, 2, 2);
	return result;
}
mat3 Transpose(const mat3& matrix)
{
	mat3 result;
	Transpose(matrix.asArray, result.asArray, 3, 3);
	return result;
}
mat4 Transpose(const mat4& matrix)
{
	mat4 result;
	Transpose(matrix.asArray, result.asArray, 4, 4);
	return result;
}

mat2 operator*(const mat2& matrix, float scalar)
{
	mat2 result;
	for (int i = 0; i < 4; ++i)
	{
		result.asArray[i] = matrix.asArray[i] * scalar;
	}
	return result;
}
mat3 operator*(const mat3& matrix, float scalar)
{
	mat3 result;
	for (int i = 0; i < 9; ++i)
	{
		result.asArray[i] = matrix.asArray[i] * scalar;
	}
	return result;
}
mat4 operator*(const mat4& matrix, float scalar)
{
	mat4 result;
	for (int i = 0; i < 16; ++i)
	{
		result.asArray[i] = matrix.asArray[i] * scalar;
	}
	return result;
}

bool Multiply(float* out, const float* matA, int aRows, int aCols, const float* matB, int bRows, int bCols)
{
	if (aCols != bRows) {
		return false;
	}
	for (int i = 0; i < aRows; ++i)
	{
		for (int j = 0; j < bCols; ++j)
		{
			out[bCols * i + j] = 0.0f;
			for (int k = 0; k < bRows; ++k)
			{
				out[bCols * i + j] += matA[aCols * i + k] * matB[bCols * k + j];
			}
		}
	}
	return true;
}
mat2 operator*(const mat2& matA, const mat2& matB)
{
	mat2 res;
	Multiply(res.asArray, matA.asArray, 2, 2, matB.asArray, 2, 2);
	return res;
}
mat3 operator*(const mat3& matA, const mat3& matB)
{
	mat3 res;
	Multiply(res.asArray, matA.asArray, 3, 3, matB.asArray, 3, 3);
	return res;
}
mat4 operator*(const mat4& matA, const mat4& matB)
{
	mat4 res;
	Multiply(res.asArray, matA.asArray, 4, 4, matB.asArray, 4, 4);
	return res;
}

float Determinant(const mat2& matrix)
{
	return matrix._11 * matrix._22 -
			matrix._12 * matrix._21;
}

mat2 Cut(const mat3& mat, int row, int col)
{
	mat2 result;
	int index = 0;

	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			if (i == row || j == col)
				continue;
			result.asArray[++index] = mat.asArray[3 * i + j];
		}
	}
	return result;
}
mat2 Minor(const mat2& mat)
{
	return mat2(
		mat._22, mat._21,
		mat._12, mat._11);
}
mat3 Minor(const mat3& mat)
{
	mat3 result;
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			result[i][j] = Determinant(Cut(mat, i, j));
		}
	}
	return result;
}

void Cofactor(float* out, const float* minor, int rows, int cols)
{
	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < cols; ++j)
		{
			out[cols * j + i] = minor[cols * j + i] * powf(-1.0f, i + j);
		}
	}
}
mat2 Cofactor(const mat2& mat)
{
	mat2 result;
	Cofactor(result.asArray, Minor(mat).asArray, 2, 2);
	return result;
}
mat3 Cofactor(const mat3& mat)
{
	mat3 result;
	Cofactor(result.asArray, Minor(mat).asArray, 3, 3);
	return result;
}

float Determinant(const mat3& mat)
{
	float result = 0.0f;
	mat3 cofactor = Cofactor(mat);
	for (int j = 0; j < 3; ++j)
	{
		result += mat.asArray[3 * 0 + j] * cofactor[0][j];
	}
	return result;
}

mat3 Cut(const mat4& mat, int row, int col)
{
	mat3 result;
	int index = 0;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			if (i == row || j == col)
				continue;
			result.asArray[++index] = mat.asArray[4 * i + j];
		}
	}
	return result;
}
mat4 Minor(const mat4& mat)
{
	mat4 result;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			result[i][j] = Determinant(Cut(mat, i, j));
		}
	}
	return result;
}
mat4 Cofactor(const mat4& mat)
{
	mat4 result;
	Cofactor(result.asArray, Minor(mat).asArray, 4, 4);
	return result;
}
float Determinant(const mat4& mat)
{
	float result = 0.0f;
	mat4 cofactor = Cofactor(mat);
	for (int j = 0; j < 4; ++j)
	{
		result += mat.asArray[4 * 0 + j] * cofactor[0][j];
	}
	return result;
}

mat2 Adjugate(const mat2& mat)
{
	return Transpose(Cofactor(mat));
}
mat3 Adjugate(const mat3& mat)
{
	return Transpose(Cofactor(mat));
}
mat4 Adjugate(const mat4& mat)
{
	return Transpose(Cofactor(mat));
}

mat2 Inverse(const mat2& mat)
{
	float det = Determinant(mat);
	if (CMP(det, 0.0f)) return mat2();
	return Adjugate(mat) * (1.0f / det);
}
mat3 Inverse(const mat3& mat)
{
	float det = Determinant(mat);
	if (CMP(det, 0.0f)) return mat3();
	return Adjugate(mat) * (1.0f / det);
}
mat4 Inverse(const mat4& mat)
{
	/*float det = Determinant(mat);
	if (CMP(det, 0.0f)) return mat4();
	return Adjugate(mat) * (1.0f / det);*/
	float det
		= mat._11 * mat._22 * mat._33 * mat._44 + mat._11 * mat._23 * mat._34 * mat._42 + mat._11 * mat._24 * mat._32 * mat._43
		+ mat._12 * mat._21 * mat._34 * mat._43 + mat._12 * mat._23 * mat._31 * mat._44 + mat._12 * mat._24 * mat._33 * mat._41
		+ mat._13 * mat._21 * mat._32 * mat._44 + mat._13 * mat._22 * mat._34 * mat._41 + mat._13 * mat._24 * mat._31 * mat._42
		+ mat._14 * mat._21 * mat._33 * mat._42 + mat._14 * mat._22 * mat._31 * mat._43 + mat._14 * mat._23 * mat._32 * mat._41
		- mat._11 * mat._22 * mat._34 * mat._43 - mat._11 * mat._23 * mat._32 * mat._44 - mat._11 * mat._24 * mat._33 * mat._42
		- mat._12 * mat._21 * mat._33 * mat._44 - mat._12 * mat._23 * mat._34 * mat._41 - mat._12 * mat._24 * mat._31 * mat._43
		- mat._13 * mat._21 * mat._34 * mat._42 - mat._13 * mat._22 * mat._31 * mat._44 - mat._13 * mat._24 * mat._32 * mat._41
		- mat._14 * mat._21 * mat._32 * mat._43 - mat._14 * mat._22 * mat._33 * mat._41 - mat._14 * mat._23 * mat._31 * mat._42;

	if (CMP(det, 0.0f)) {
		return mat4();
	}
	float i_det = 1.0f / det;

	mat4 result;
	result._11 = (mat._22 * mat._33 * mat._44 + mat._23 * mat._34 * mat._42 + mat._24 * mat._32 * mat._43 - mat._22 * mat._34 * mat._43 - mat._23 * mat._32 * mat._44 - mat._24 * mat._33 * mat._42) * i_det;
	result._12 = (mat._12 * mat._34 * mat._43 + mat._13 * mat._32 * mat._44 + mat._14 * mat._33 * mat._42 - mat._12 * mat._33 * mat._44 - mat._13 * mat._34 * mat._42 - mat._14 * mat._32 * mat._43) * i_det;
	result._13 = (mat._12 * mat._23 * mat._44 + mat._13 * mat._24 * mat._42 + mat._14 * mat._22 * mat._43 - mat._12 * mat._24 * mat._43 - mat._13 * mat._22 * mat._44 - mat._14 * mat._23 * mat._42) * i_det;
	result._14 = (mat._12 * mat._24 * mat._33 + mat._13 * mat._22 * mat._34 + mat._14 * mat._23 * mat._32 - mat._12 * mat._23 * mat._34 - mat._13 * mat._24 * mat._32 - mat._14 * mat._22 * mat._33) * i_det;
	result._21 = (mat._21 * mat._34 * mat._43 + mat._23 * mat._31 * mat._44 + mat._24 * mat._33 * mat._41 - mat._21 * mat._33 * mat._44 - mat._23 * mat._34 * mat._41 - mat._24 * mat._31 * mat._43) * i_det;
	result._22 = (mat._11 * mat._33 * mat._44 + mat._13 * mat._34 * mat._41 + mat._14 * mat._31 * mat._43 - mat._11 * mat._34 * mat._43 - mat._13 * mat._31 * mat._44 - mat._14 * mat._33 * mat._41) * i_det;
	result._23 = (mat._11 * mat._24 * mat._43 + mat._13 * mat._21 * mat._44 + mat._14 * mat._23 * mat._41 - mat._11 * mat._23 * mat._44 - mat._13 * mat._24 * mat._41 - mat._14 * mat._21 * mat._43) * i_det;
	result._24 = (mat._11 * mat._23 * mat._34 + mat._13 * mat._24 * mat._31 + mat._14 * mat._21 * mat._33 - mat._11 * mat._24 * mat._33 - mat._13 * mat._21 * mat._34 - mat._14 * mat._23 * mat._31) * i_det;
	result._31 = (mat._21 * mat._32 * mat._44 + mat._22 * mat._34 * mat._41 + mat._24 * mat._31 * mat._42 - mat._21 * mat._34 * mat._42 - mat._22 * mat._31 * mat._44 - mat._24 * mat._32 * mat._41) * i_det;
	result._32 = (mat._11 * mat._34 * mat._42 + mat._12 * mat._31 * mat._44 + mat._14 * mat._32 * mat._41 - mat._11 * mat._32 * mat._44 - mat._12 * mat._34 * mat._41 - mat._14 * mat._31 * mat._42) * i_det;
	result._33 = (mat._11 * mat._22 * mat._44 + mat._12 * mat._24 * mat._41 + mat._14 * mat._21 * mat._42 - mat._11 * mat._24 * mat._42 - mat._12 * mat._21 * mat._44 - mat._14 * mat._22 * mat._41) * i_det;
	result._34 = (mat._11 * mat._24 * mat._32 + mat._12 * mat._21 * mat._34 + mat._14 * mat._22 * mat._31 - mat._11 * mat._22 * mat._34 - mat._12 * mat._24 * mat._31 - mat._14 * mat._21 * mat._32) * i_det;
	result._41 = (mat._21 * mat._33 * mat._42 + mat._22 * mat._31 * mat._43 + mat._23 * mat._32 * mat._41 - mat._21 * mat._32 * mat._43 - mat._22 * mat._33 * mat._41 - mat._23 * mat._31 * mat._42) * i_det;
	result._42 = (mat._11 * mat._32 * mat._43 + mat._12 * mat._33 * mat._41 + mat._13 * mat._31 * mat._42 - mat._11 * mat._33 * mat._42 - mat._12 * mat._31 * mat._43 - mat._13 * mat._32 * mat._41) * i_det;
	result._43 = (mat._11 * mat._23 * mat._42 + mat._12 * mat._21 * mat._43 + mat._13 * mat._22 * mat._41 - mat._11 * mat._22 * mat._43 - mat._12 * mat._23 * mat._41 - mat._13 * mat._21 * mat._42) * i_det;
	result._44 = (mat._11 * mat._22 * mat._33 + mat._12 * mat._23 * mat._31 + mat._13 * mat._21 * mat._32 - mat._11 * mat._23 * mat._32 - mat._12 * mat._21 * mat._33 - mat._13 * mat._22 * mat._31) * i_det;

	return result;
}