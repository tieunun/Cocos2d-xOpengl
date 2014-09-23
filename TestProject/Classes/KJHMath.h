#ifndef __KJHMATH_H_
#define __KJHMATH_H_
#include <math.h>

namespace KJH
{
	inline float ToRadian(float degree){return degree * M_PI / 180.0f;}
	inline float Cot(float radian){return 1.0f / tanf(radian);}


	struct Float2
	{
		float x,y;
		Float2(float ix = 0.0f,float iy = 0.0f) : x(ix) , y(iy) {}
		Float2 operator+(const Float2& rhs)const{return Float2(x+rhs.x,y+rhs.y);}
		Float2 operator-(const Float2& rhs)const{return Float2(x-rhs.x,y-rhs.y);}
		Float2 operator*(const Float2& rhs)const{return Float2(x*rhs.x,y*rhs.y);}
		Float2 operator/(const Float2& rhs)const{return Float2(x/rhs.x,y/rhs.y);}
	};
	struct Float3
	{
		float x,y,z;
		Float3(float ix = 0.0f,float iy = 0.0f,float iz = 0.0f) : x(ix) , y(iy) , z(iz){}
		Float3 operator+(const Float3& rhs)const{return Float3(x+rhs.x,y+rhs.y,z+rhs.z);}
		Float3 operator-(const Float3& rhs)const{return Float3(x-rhs.x,y-rhs.y,z-rhs.z);}
		Float3 operator*(const Float3& rhs)const{return Float3(x*rhs.x,y*rhs.y,z*rhs.z);}
		Float3 operator/(const Float3& rhs)const{return Float3(x/rhs.x,y/rhs.y,z/rhs.z);}
		static Float3 Cross(const Float3& lhs,const Float3& rhs);
		static float Dot(const Float3& lhs,const Float3& rhs);
		static Float3 Normalize(const Float3& f);
		static float Length(const Float3& f);
		static float LengthSq(const Float3& f);
	};
	struct Float4
	{
		float x,y,z,w;
		Float4(float ix = 0.0f,float iy = 0.0f,float iz = 0.0f,float iw = 0.0f) : x(ix) , y(iy) , z(iz),w(iw){}
		Float4 operator+(const Float4& rhs)const{return Float4(x+rhs.x,y+rhs.y,z+rhs.z,w+rhs.w);}
		Float4 operator-(const Float4& rhs)const{return Float4(x-rhs.x,y-rhs.y,z-rhs.z,w-rhs.w);}
		Float4 operator*(const Float4& rhs)const{return Float4(x*rhs.x,y*rhs.y,z*rhs.z,w*rhs.w);}
		Float4 operator/(const Float4& rhs)const{return Float4(x/rhs.x,y/rhs.y,z/rhs.z,w/rhs.w);}
	};
	struct Mat
	{
		float m[4][4];
		Mat(float m11,float m12,float m13,float m14,
			float m21,float m22,float m23,float m24,
			float m31,float m32,float m33,float m34,
			float m41,float m42,float m43,float m44)
		{
			m[0][0] = m11; m[0][1] = m12; m[0][2] = m13; m[0][3] = m14;
			m[1][0] = m21; m[1][1] = m22; m[1][2] = m23; m[1][3] = m24;
			m[2][0] = m31; m[2][1] = m32; m[2][2] = m33; m[2][3] = m34;
			m[3][0] = m41; m[3][1] = m42; m[3][2] = m43; m[3][3] = m44;
		}
		Mat(){
			*this = Mat::Identity();
		}
		static Mat LookAtRH(const Float3& eye,const Float3& target,const Float3& upper);
		static Mat PerspectiveFovRH(float fieldOfViewY,float aspectRatio,float znearPlane,float zfarPlane);
		static Mat Identity(){return Mat(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);}
		static Mat Transpose(const Mat& m)
		{
			return Mat(m.m[0][0],m.m[1][0],m.m[2][0],m.m[3][0],
				       m.m[0][1],m.m[1][1],m.m[2][1],m.m[3][1],
					   m.m[0][2],m.m[1][2],m.m[2][2],m.m[3][2],
					   m.m[0][3],m.m[1][3],m.m[2][3],m.m[3][3]);
		}
		static Mat RotateY(float rad);
	};






}


#endif // __KJHMATH_H_