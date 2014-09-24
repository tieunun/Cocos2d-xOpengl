#include "KJHMesh.h"

namespace KJH
{

	Mesh::Mesh()
		: m_vertex()
		, m_index()
		, m_material()
		, m_bone()
		, m_vertexNum(0)
		, m_vertexBuffer(0)
		, m_indexBuffer(0)
	{

	}

	Mesh::~Mesh()
	{
		if(m_vertexBuffer){
			glDeleteBuffers(1,&m_vertexBuffer);
		}
		if(m_indexBuffer){
			glDeleteBuffers(1,&m_indexBuffer);
		}
	}

	GLuint Mesh::Load(const char* filepath)
	{
		auto fileUtils = cocos2d::FileUtils::sharedFileUtils();
		std::string fullpath = fileUtils->fullPathForFilename(filepath);

		ssize_t filesize;
		unsigned char* buffer = fileUtils->getFileData(fullpath,"rb",&filesize);
		if(filesize == 0)
		{
			assert(0);
			return -1;
		}

		struct PmdHeader
		{
			char magic[4];//"Pmd"	//�{����char[3]�����p�b�L���O����邽��.
			float version;
			char model_name[20];
			char comment[256];
		};

		unsigned int offset = 0;

		PmdHeader header = PmdHeader();
		//PMD�̃w�b�_�[�t�@�C����ǂݍ���-------------------------------------------------------------.
		memcpy(header.magic,buffer,sizeof(header.magic));
		header.magic[3] = '\0';
		offset = 3;
		memcpy(&header.version,buffer + offset,sizeof(header) - sizeof(header.magic));
		offset = sizeof(header) - 1;
		
#if 1
		//���_���ǂ݂���------------------------------------------------------------------------------
		GLuint vertexNum;
		memcpy(&vertexNum,buffer + offset,sizeof(vertexNum));
		offset +=sizeof(vertexNum);

		//���_���
		struct PmdVertex
		{
			Float3 pos; // x, y, z // ���W
			Float3 normal; // nx, ny, nz // �@���x�N�g��
			Float2 uv; // u, v // UV���W // MMD�͒��_UV
			GLushort bone_num[2]; // �{�[���ԍ�1�A�ԍ�2 // ���f���ό`(���_�ړ�)���ɉe��
			GLubyte bone_weight[2]; // �{�[��1�ɗ^����e���x // min:0 max:100 // �{�[��2�ւ̉e���x�́A(100 - bone_weight) ���Ƃ��Ƃ�byte
			//GLubyte edge_flag; // 0:�ʏ�A1:�G�b�W���� // �G�b�W(�֊s)���L���̏ꍇ ���Ƃ��Ƃ�byte
		};
		m_vertex.resize(vertexNum);
		//���_�ǂݍ���.
		for(GLuint i = 0; i < vertexNum; ++i)
		{
			PmdVertex vertPmd = {};
			memcpy(&vertPmd,buffer + offset, 38);

			m_vertex[i].pos = vertPmd.pos;
			m_vertex[i].normal = vertPmd.normal;
			m_vertex[i].uv = vertPmd.uv;
			offset += 38;
		}
		//�C���f�b�N�X�ǂݍ���.
		GLuint indexNum = 0;
		memcpy(&indexNum,buffer + offset,sizeof(indexNum));
		offset += sizeof(indexNum);

		//�C���f�b�N�X�ǂݍ���.
		m_index.resize(indexNum);

		memcpy(&m_index[0],buffer + offset, sizeof(m_index[0]) * indexNum);
		offset += sizeof(m_index[0]) * indexNum;
#endif // 0

		struct PmdMaterial
		{
			KJH::Float3 diffuse; // dr, dg, db // �����F
			float alpha; // �����F�̕s�����x
			float specularity;
			KJH::Float3 specular_color; // sr, sg, sb // ����F
			KJH::Float3 mirror_color; // mr, mg, mb // ���F(ambient)
			GLbyte toon_index; // toon??.bmp // 0.bmp:0xFF, 1(01).bmp:0x00 �E�E�E 10.bmp:0x09
			GLbyte edge_flag; // �֊s�A�e
			GLuint face_vert_count; // �ʒ��_�� // �ʐ��ł͂���܂���B���̍ގ��Ŏg���A�ʒ��_���X�g�̃f�[�^���ł��B
			char texture_file_name[20]; // �e�N�X�`���t�@�C�����܂��̓X�t�B�A�t�@�C���� // 20�o�C�g���肬��܂Ŏg����(�I�[��0x00�͖����Ă�����)
		};

		GLuint materialNum = 0;
		memcpy(&materialNum,buffer + offset,sizeof(materialNum));
		offset += sizeof(materialNum);
		std::vector<PmdMaterial> pmdMat;
		pmdMat.reserve(materialNum);
		//�}�e���A�������.
		for(int i = 0; i < materialNum; ++i)
		{
			PmdMaterial material = {};
			const int AlientSize = 
				sizeof(material.diffuse) + 
				sizeof(material.alpha) + 
				sizeof(material.specularity) +
				sizeof(material.specular_color) +
				sizeof(material.mirror_color);
			memcpy(&material,buffer+offset,AlientSize);
			offset += AlientSize;
			memcpy(&material.toon_index,buffer+offset,sizeof(material.toon_index));
			offset += sizeof(material.toon_index);
			memcpy(&material.edge_flag,buffer+offset,sizeof(material.edge_flag));
			offset += sizeof(material.edge_flag);
			memcpy(&material.face_vert_count,buffer+offset,sizeof(material.face_vert_count));
			offset += sizeof(material.face_vert_count);
			memcpy(&material.texture_file_name,buffer+offset,sizeof(material.texture_file_name));
			offset += sizeof(material.texture_file_name);
			pmdMat.push_back(material);
		}

		struct PmdBone
		{
			char bone_name[20]; // �{�[����
			GLushort parent_bone_index; // �e�{�[���ԍ�(�Ȃ��ꍇ��0xFFFF)
			GLushort tail_pos_bone_index; // tail�ʒu�̃{�[���ԍ�(�`�F�[�����[�̏ꍇ��0xFFFF 0 ���⑫2) // �e�F�q��1�F���Ȃ̂ŁA��Ɉʒu���ߗp
			GLbyte bone_type; // �{�[���̎��
			GLushort ik_parent_bone_index; // IK�{�[���ԍ�(�e��IK�{�[���B�Ȃ��ꍇ��0)
			float bone_head_pos[3]; // x, y, z // �{�[���̃w�b�h�̈ʒu
		};
		//�{�[��
		GLushort boneCnt = 0;
		memcpy(&boneCnt,buffer+offset,sizeof(boneCnt));
		offset += sizeof(boneCnt);
		std::vector<PmdBone> pmdBone;
		pmdBone.reserve(boneCnt);
		for(int i = 0; i < boneCnt; ++i)
		{
			PmdBone bone = {};
			memcpy(&bone.bone_name,buffer+offset,sizeof(bone.bone_name));
			offset += sizeof(bone.bone_name);
			memcpy(&bone.parent_bone_index,buffer+offset,sizeof(bone.parent_bone_index));
			offset += sizeof(bone.parent_bone_index);
			memcpy(&bone.tail_pos_bone_index,buffer+offset,sizeof(bone.tail_pos_bone_index));
			offset += sizeof(bone.tail_pos_bone_index);
			memcpy(&bone.bone_type,buffer+offset,sizeof(bone.bone_type));
			offset += sizeof(bone.bone_type);
			memcpy(&bone.ik_parent_bone_index,buffer+offset,sizeof(bone.ik_parent_bone_index));
			offset += sizeof(bone.ik_parent_bone_index);
			memcpy(&bone.bone_head_pos,buffer+offset,sizeof(bone.bone_head_pos));
			offset += sizeof(bone.bone_head_pos);
			pmdBone.push_back(bone);
		}
		
		if(buffer)
		{
			delete[] buffer;
			buffer = 0;
		}

		//�}�e���A���i�[.
		m_material.resize(materialNum);
		for(int i = 0; i < materialNum; ++i)
		{
			m_material[i].diffuse = pmdMat[i].diffuse;
			m_material[i].ambient = pmdMat[i].mirror_color;
			m_material[i].alpha = pmdMat[i].alpha;
			m_material[i].indexNum = pmdMat[i].face_vert_count;

			std::string path = pmdMat[i].texture_file_name;
			if(path.empty()) continue;
			KJH::Texture2D* pTex = KJH::Texture2D::Create(path);
			m_material[i].tex = pTex;
		}
		//�{�[���i�[.
		m_bone.resize(boneCnt);

		//�\�z.
		glGenBuffers(1,&m_vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER,m_vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER,vertexNum * sizeof(m_vertex[0]),&m_vertex[0],GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER,0);

		//�C���f�b�N�X.
		glGenBuffers(1,&m_indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(m_index[0])*indexNum,&m_index[0],GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
		return 0;
	}

#define BUFFER_OFFSET(bytes) ((GLubyte *)NULL + (bytes))

	void Mesh::Draw(unsigned int shader)
	{
		//�A�g���r���[�g�ɒ��_�o�b�t�@���o�C���h.
		//�ʒu.
		glBindBuffer(GL_ARRAY_BUFFER,m_vertexBuffer);
		unsigned int attr = glGetAttribLocation(shader,"in_pos");
		glVertexAttribPointer(attr,3,GL_FLOAT,GL_FALSE,sizeof(m_vertex[0]),NULL);
		glEnableVertexAttribArray(attr);
		//�@��.
		attr = glGetAttribLocation(shader,"in_normal");
		glVertexAttribPointer(attr,3,GL_FLOAT,GL_FALSE,sizeof(m_vertex[0]),(GLuint*)sizeof(m_vertex[0].pos));
		glEnableVertexAttribArray(attr);
		//UV.
		attr = glGetAttribLocation(shader,"in_uv");
		glVertexAttribPointer(attr,2,GL_FLOAT,GL_FALSE,sizeof(m_vertex[0]),(GLuint*)(sizeof(m_vertex[0].pos)+sizeof(m_vertex[0].normal)));
		glEnableVertexAttribArray(attr);
		auto error = glGetError();
		assert(error == 0);

		//�C���f�b�N�X�o�b�t�@.
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
		GLuint offset = 0;
		//�}�e���A�����[�v.
		for(ssize_t i = 0; i < m_material.size(); ++i)
		{
			glUniform3fv(glGetUniformLocation(shader, "diffuse"), 1,(float*) &(m_material[i].diffuse));
			glUniform3fv(glGetUniformLocation(shader, "ambient"), 1,(float*) &(m_material[i].ambient));
			error = glGetError();
			static KJH::Texture2D* pWhite = KJH::Texture2D::CreateWhite();

			if(m_material[i].tex){
				glBindTexture(GL_TEXTURE_2D,m_material[i].tex->GetTex());
			}
			else{
				glBindTexture(GL_TEXTURE_2D,pWhite->GetTex());
			}
			//�`�施��.
			glDrawElements(GL_TRIANGLES,m_material[i].indexNum, GL_UNSIGNED_SHORT, BUFFER_OFFSET(offset));
			offset += sizeof(m_index[0]) * m_material[i].indexNum;
			error = glGetError();
		}
		glBindBuffer(GL_ARRAY_BUFFER,0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
	}
}