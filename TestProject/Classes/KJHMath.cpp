#include "KJHMath.h"
#include <math.h>


namespace KJH
{
	float Float3::Dot(const Float3& lhs,const Float3& rhs)
	{
		return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
	}
	Float3 Float3::Cross(const Float3& lhs,const Float3& rhs)
	{
		float x = lhs.y * rhs.z - lhs.z * rhs.y;
		float y = lhs.z * rhs.x - lhs.x * rhs.z;
		float z = lhs.x * rhs.y - lhs.y * rhs.x;
		return Float3(x,y,z);
	}
	float Float3::LengthSq(const Float3& f)
	{
		return f.x * f.x + f.y * f.y + f.z * f.z;
	}

	float Float3::Length(const Float3& f)
	{
		return sqrtf( LengthSq(f) );
	}
	 Float3 Float3::Normalize(const Float3& f)
	 {
		 float inv = 1.0f / Length(f);
		 return Float3(f.x * inv,f.y * inv,f.z * inv);
	 }


	Mat Mat::LookAtRH(const Float3& eye,const Float3& target,const Float3& upper)
	{
		Float3 zaxis = Float3::Normalize(eye - target);
		Float3 xaxis = Float3::Normalize(Float3::Cross(upper, zaxis));
		Float3 yaxis = Float3::Cross(zaxis, xaxis);

		return Mat(xaxis.x,yaxis.x,zaxis.x,0,
		           xaxis.y,yaxis.y,zaxis.y,0,
				   xaxis.z,yaxis.z,zaxis.z,0,
				   -Float3::Dot(xaxis,eye),-Float3::Dot(yaxis, eye),-Float3::Dot(zaxis, eye),1);
		
	}


	Mat Mat::PerspectiveFovRH(float fieldOfViewY,float aspectRatio,float znearPlane,float zfarPlane)
	{
		float h = Cot(fieldOfViewY * 0.5f );
		float zNearMinuszFarInv = 1.0f / (znearPlane - zfarPlane);
		
		return Mat(
			h / aspectRatio,0,0,0,
			0              ,h,0,0,
			0              ,0,(zfarPlane)*zNearMinuszFarInv,-1,
			0,0,(zfarPlane*znearPlane)*zNearMinuszFarInv,0);
	}

	Mat Mat::RotateY(float rad)
	{
		float c = cosf(rad);
		float s = sinf(rad);
		return Mat(
			c,0,-s,0,
			0,1,0,0,
			s,0,c,0,
			0,0,0,1
			);
	}




	Quaternion::Quaternion(float ix,float iy,float iz,float iw)
		: v(KJH::Float3(ix,iy,iz))
		, w(iw)
	{
	}
	
	Quaternion::Quaternion(const Float3& obj,float iw)
		: v(obj)
		, w(iw)
	{
	}
	//掛け算代入オペレータ.
	Quaternion& Quaternion::operator*=(const Quaternion& rhs)
	{
		*this = (*this) * rhs;
		return *this;
	}
	//掛け算オペレータ.
	Quaternion Quaternion::operator*(const Quaternion& rhs)const
	{
		return Quaternion(rhs.v*w + v*rhs.w + Float3::Cross(v,rhs.v), 
			w*rhs.w - Float3::Dot(v,rhs.v));
	}
	//共益を返す.
	Quaternion Quaternion::Conjugate(const Quaternion& q)
	{
		return Quaternion(-q.v.x,-q.v.y,-q.v.z,q.w);
	}
	//回転軸と回転角度を元にクォータニオン生成.
	Quaternion Quaternion::RotAngle(const Float3& axis,float radian)
	{
		float sinHalf = sin(radian*0.5f);
		float cosHalf = cos(radian*0.5f);
		return Quaternion(axis * sinHalf,cosHalf);
	}
	Float3 Quaternion::MulVec(const Float3& v)const
	{
		Quaternion res = Quaternion::Conjugate(*this) * Quaternion(v,0);
		res = res * (*this);
		return res.v;
	}

	/*Float3 operator*(const Quaternion& q,const Float3& v)
	{
		Quaternion res = Quaternion::Conjugate(q)  * Quaternion(v,0) * q;
		return res.v;
	}
	Float3 operator*(const Float3& v,const Quaternion& q)
	{
		return q * v;
	}*/
};
