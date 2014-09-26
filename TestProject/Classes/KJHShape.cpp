#include "KJHShape.h"

namespace KJH
{
	LineBatch::LineBatch()
		: m_lineNum(0)
	{
		for(int i = 0; i < LINE_MAX * 2; ++i)
		{
			m_lineIndex[i] = i;
		}

		glGenBuffers(2,m_buffers);//���_�o�b�t�@�ƃC���f�b�N�X�o�b�t�@.
		glBindBuffer(GL_ARRAY_BUFFER,m_buffers[0]);
	}
	void LineBatch::PushLine(const Float3& beg,const Float3& end,const Float4& color)
	{
		m_lineArray[m_lineNum].beg = beg;
		m_lineArray[m_lineNum].end = end;
		m_lineArray[m_lineNum].color = color;

		++m_lineNum;
	}
	//�o�^����Ă��郉�C������C�ɕ`��.
	void LineBatch::Flush()
	{

	}

}