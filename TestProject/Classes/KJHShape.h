#ifndef __KJHSHAPE_H_
#define __KJHSHAPE_H_

#include "cocos2d.h"
#include "KJHMath.h"
#include "KJHTexture.h"

namespace KJH
{

	class LineBatch
	{
	public:
		static const int LINE_MAX = 1024;
	public:
		void PushLine(const Float3& beg,const Float3& end,const Float4& color);
		void Flush();
	protected:
		LineBatch();
		struct Line
		{
			Float3 beg,end;
			Float4 color;
		};
		Line m_lineArray[LINE_MAX];
		GLushort m_lineIndex[LINE_MAX * 2];
		int m_lineNum;
		GLuint m_buffers[2];	//頂点バッファとインデックスバッファ.
	};

	class ShapeBatch
	{

	};
}

#endif // __KJHSHAPE_H_