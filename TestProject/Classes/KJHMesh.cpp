#include "KJHMesh.h"

namespace KJH
{
#define BONE_TEX_SIZE 32
	Mesh::Mesh()
		: m_vertex()
		, m_index()
		, m_material()
		, m_bone()
		, m_vertexNum(0)
		, m_vertexBuffer(0)
		, m_indexBuffer(0)
		, m_white(nullptr)
		, m_boneQuaTrans()
		, m_boneTex(0)
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

		for(int i = 0; i < m_material.size(); ++i)
		{
			CC_SAFE_DELETE(m_material[i].tex);
		}

		CC_SAFE_DELETE(m_white);
		glDeleteTextures(1,&m_boneTex);
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
			char magic[4];//"Pmd"	//本来はchar[3]だがパッキングされるため.
			float version;
			char model_name[20];
			char comment[256];
		};

		unsigned int offset = 0;

		PmdHeader header = PmdHeader();
		//PMDのヘッダーファイルを読み込む-------------------------------------------------------------.
		memcpy(header.magic,buffer,sizeof(header.magic));
		header.magic[3] = '\0';
		offset = 3;
		memcpy(&header.version,buffer + offset,sizeof(header) - sizeof(header.magic));
		offset = sizeof(header) - 1;
		
#if 1
		//頂点数読みこみ------------------------------------------------------------------------------
		GLuint vertexNum;
		memcpy(&vertexNum,buffer + offset,sizeof(vertexNum));
		offset +=sizeof(vertexNum);

		//頂点情報
		struct PmdVertex
		{
			Float3 pos; // x, y, z // 座標
			Float3 normal; // nx, ny, nz // 法線ベクトル
			Float2 uv; // u, v // UV座標 // MMDは頂点UV
			GLushort bone_num[2]; // ボーン番号1、番号2 // モデル変形(頂点移動)時に影響
			GLubyte bone_weight[2]; // ボーン1に与える影響度 // min:0 max:100 // ボーン2への影響度は、(100 - bone_weight) もともとはbyte
			//GLubyte edge_flag; // 0:通常、1:エッジ無効 // エッジ(輪郭)が有効の場合 もともとはbyte
		};
		m_vertex.resize(vertexNum);
		//頂点読み込み.
		for(GLuint i = 0; i < vertexNum; ++i)
		{
			PmdVertex vertPmd = {};
			memcpy(&vertPmd,buffer + offset, 38);

			m_vertex[i].pos = vertPmd.pos;
			/*m_vertex[i].pos.x *= -1;
			m_vertex[i].pos.z *= -1;*/
			m_vertex[i].normal = vertPmd.normal;
			/*m_vertex[i].normal.x *= -1;
			m_vertex[i].normal.z *= -1;*/
			m_vertex[i].uv = vertPmd.uv;
			/*m_vertex[i].uv.x = 1.0f - m_vertex[i].uv.x;
			m_vertex[i].uv.y = 1.0f - m_vertex[i].uv.y;*/
			m_vertex[i].boneIdx.x = static_cast<float>(vertPmd.bone_num[0]);
			m_vertex[i].boneIdx.y = static_cast<float>(vertPmd.bone_num[1]);
			m_vertex[i].weight.x = static_cast<float>(vertPmd.bone_weight[0] * 0.01f);
			m_vertex[i].weight.y = static_cast<float>((100 - vertPmd.bone_weight[0]) * 0.01f);
			offset += 38;
		}
		//インデックス読み込み.
		GLuint indexNum = 0;
		memcpy(&indexNum,buffer + offset,sizeof(indexNum));
		offset += sizeof(indexNum);

		//インデックス読み込み.
		m_index.resize(indexNum);

		memcpy(&m_index[0],buffer + offset, sizeof(m_index[0]) * indexNum);
		offset += sizeof(m_index[0]) * indexNum;
#endif // 0

		struct PmdMaterial
		{
			KJH::Float3 diffuse; // dr, dg, db // 減衰色
			float alpha; // 減衰色の不透明度
			float specularity;
			KJH::Float3 specular_color; // sr, sg, sb // 光沢色
			KJH::Float3 mirror_color; // mr, mg, mb // 環境色(ambient)
			GLbyte toon_index; // toon??.bmp // 0.bmp:0xFF, 1(01).bmp:0x00 ・・・ 10.bmp:0x09
			GLbyte edge_flag; // 輪郭、影
			GLuint face_vert_count; // 面頂点数 // 面数ではありません。この材質で使う、面頂点リストのデータ数です。
			char texture_file_name[20]; // テクスチャファイル名またはスフィアファイル名 // 20バイトぎりぎりまで使える(終端の0x00は無くても動く)
		};

		GLuint materialNum = 0;
		memcpy(&materialNum,buffer + offset,sizeof(materialNum));
		offset += sizeof(materialNum);
		std::vector<PmdMaterial> pmdMat;
		pmdMat.reserve(materialNum);
		//マテリアルを解析.
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
			char bone_name[20]; // ボーン名
			GLushort parent_bone_index; // 親ボーン番号(ない場合は0xFFFF)
			GLushort tail_pos_bone_index; // tail位置のボーン番号(チェーン末端の場合は0xFFFF 0 →補足2) // 親：子は1：多なので、主に位置決め用
			GLbyte bone_type; // ボーンの種類
			GLushort ik_parent_bone_index; // IKボーン番号(影響IKボーン。ない場合は0)
			float bone_head_pos[3]; // x, y, z // ボーンのヘッドの位置
		};
		//ボーン
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

		m_white = KJH::Texture2D::CreateWhite();
		//マテリアル格納.
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
		//ボーン格納.
		m_bone.resize(boneCnt);
		m_bone_rotate.resize(boneCnt);
		m_boneTranslate.resize(boneCnt);
		m_boneQuaTrans.resize(boneCnt);
		for(int i = 0; i < m_bone.size(); ++i)
		{
			if(pmdBone[i].parent_bone_index == 0xFFFF) m_bone[i].parent = NULL;
			else m_bone[i].parent = &m_bone[ pmdBone[i].parent_bone_index ];

			m_bone[i].offset.x = pmdBone[i].bone_head_pos[0];
			m_bone[i].offset.y = pmdBone[i].bone_head_pos[1];
			m_bone[i].offset.z = pmdBone[i].bone_head_pos[2];
			
		}

		CCLOG("pmd:%s",filepath);
		CCLOG("vertex:%d",vertexNum);
		CCLOG("index:%d",indexNum);
		CCLOG("polygon:%d",indexNum/3);
		CCLOG("material:%d",materialNum);
		CCLOG("bone:%d",boneCnt);

		//構築.
		glGenBuffers(1,&m_vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER,m_vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER,vertexNum * sizeof(m_vertex[0]),&m_vertex[0],GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER,0);

		//インデックス.
		glGenBuffers(1,&m_indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(m_index[0])*indexNum,&m_index[0],GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

		if(glGetError() != 0)
		{
			CCLOG("Bone Begin");
			assert(0);
		}

		auto a = GLEW_ARB_texture_float;

		glGenTextures(1,&m_boneTex);
		glBindTexture(GL_TEXTURE_2D,m_boneTex);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
		//Float4 temp[BONE_TEX_SIZE * BONE_TEX_SIZE];
		//glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,BONE_TEX_SIZE,BONE_TEX_SIZE,0,GL_RGBA,GL_FLOAT,&temp[0]);
		auto er = glGetError();
		if(er != 0)
		{
			CCLOG("%d",er);
			CCLOG("Bone Create Failed");
			assert(0);
		}


		return 0;
	}

#define BUFFER_OFFSET(bytes) ((GLubyte *)NULL + (bytes))


	Quaternion MeshBone::LocalRotate()
	{
		if(parent == nullptr) return rotate;
		return parent->LocalRotate() * rotate;
	}
	Float3 MeshBone::LocalTranslate()
	{
		Float3 trans = rotate.MulVec(-offset) + translate + offset;
		if(parent == nullptr) return trans;
		return parent->LocalTranslate() + trans;
	}

	void Mesh::Draw(unsigned int shader)
	{
		m_bone[1].translate.z = 15;
		//m_bone[1].rotate = Quaternion::RotAngle(Float3(-1,0,0),-1.57);
		//m_bone[17].rotate = Quaternion::RotAngle(Float3(0,0,1),-1.57*0.5);
		auto error = glGetError();
		CCASSERT(error == 0,"SkinMesh");
		for(int i = 0; i < m_bone.size(); ++i)
		{
			m_bone_rotate[i] = m_bone[i].LocalRotate();
			m_boneTranslate[i] = m_bone[i].LocalTranslate();

			m_boneQuaTrans[i].rotate = m_bone_rotate[i];
			m_boneQuaTrans[i].translate.x = m_boneTranslate[i].x;
			m_boneQuaTrans[i].translate.y = m_boneTranslate[i].y;
			m_boneQuaTrans[i].translate.z = m_boneTranslate[i].z;
		}

#if 1
		//ボーンをセットする.
		auto a = glGetUniformLocation(shader,"bone_quaternion");
		glUniform4fv(a, m_bone_rotate.size(),(float*)&(m_bone_rotate[0]));
		a = glGetUniformLocation(shader,"bone_translate");
		glUniform3fv(a, m_boneTranslate.size(),(float*)&(m_boneTranslate[0]));
		unsigned int attrOffset = 0;
		//位置.
		glBindBuffer(GL_ARRAY_BUFFER,m_vertexBuffer);
		unsigned int attr = glGetAttribLocation(shader,"in_pos");
		glVertexAttribPointer(attr,3,GL_FLOAT,GL_FALSE,sizeof(m_vertex[0]),(GLuint*)attrOffset);
		glEnableVertexAttribArray(attr);
		attrOffset += sizeof(m_vertex[0].pos);

		//法線.
		attr = glGetAttribLocation(shader,"in_normal");
		glVertexAttribPointer(attr,3,GL_FLOAT,GL_FALSE,sizeof(m_vertex[0]),(GLuint*)attrOffset);
		glEnableVertexAttribArray(attr);
		attrOffset += sizeof(m_vertex[0].normal);
		//UV.
		attr = glGetAttribLocation(shader,"in_uv");
		glVertexAttribPointer(attr,2,GL_FLOAT,GL_FALSE,sizeof(m_vertex[0]),(GLuint*)attrOffset);
		glEnableVertexAttribArray(attr);
		attrOffset += sizeof(m_vertex[0].uv);

		//ボーンインデックス.
		attr = glGetAttribLocation(shader,"in_bone");
		glVertexAttribPointer(attr,2,GL_FLOAT,GL_FALSE,sizeof(m_vertex[0]),(GLuint*)attrOffset);
		glEnableVertexAttribArray(attr);
		attrOffset += sizeof(m_vertex[0].boneIdx);
		//ボーンウェイト.
		attr = glGetAttribLocation(shader,"in_weight");
		glVertexAttribPointer(attr,2,GL_FLOAT,GL_FALSE,sizeof(m_vertex[0]),(GLuint*)attrOffset);
		glEnableVertexAttribArray(attr);
		attrOffset += sizeof(m_vertex[0].weight);
#endif
		error = glGetError();
		CCASSERT(error == 0,"Begin Attrib");
		//テクスチャ番号取得.
		auto boneID = glGetUniformLocation(shader,"bone_tex");
		auto tex = glGetUniformLocation(shader,"texture");
		//ピクセルを４バイトに.
		//glPixelStorei(GL_UNPACK_ALIGNMENT,4);
		


		//ボーンテクスチャをバインド.
		const GLint boneUnit = 1;
		glActiveTexture(GL_TEXTURE0 + boneUnit);
		glBindTexture(GL_TEXTURE_2D,m_boneTex);
		Float4 temp[BONE_TEX_SIZE * BONE_TEX_SIZE];		//コピー用バッファ.
		memset(temp,0,sizeof(temp));
		memcpy(temp,&m_boneQuaTrans[0],sizeof(m_boneQuaTrans[0]) * m_boneQuaTrans.size());
		//テクスチャに流し込む.
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA32F,BONE_TEX_SIZE,BONE_TEX_SIZE,0,GL_RGBA,GL_FLOAT,temp);
		glUniform1i(boneID,boneUnit);
		
		//通常状態に戻す.
		glActiveTexture(GL_TEXTURE0);
		error = glGetError();
		CCASSERT(error == 0,"AAAAAAAAAAAAAAa");


		//インデックスバッファ.
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
		GLuint offset = 0;
		//マテリアルループ.
		for(ssize_t i = 0; i < m_material.size(); ++i)
		{
			glUniform3fv(glGetUniformLocation(shader, "diffuse"), 1,(float*) &(m_material[i].diffuse));
			glUniform3fv(glGetUniformLocation(shader, "ambient"), 1,(float*) &(m_material[i].ambient));
			error = glGetError();

			auto texture = m_material[i].tex ? m_material[i].tex->GetTex() : m_white->GetTex();
			glBindTexture(GL_TEXTURE_2D,texture);
			//描画命令.
			glDrawElements(GL_TRIANGLES,m_material[i].indexNum, GL_UNSIGNED_SHORT, BUFFER_OFFSET(offset));
			offset += sizeof(m_index[0]) * m_material[i].indexNum;
			error = glGetError();
		}
		glBindBuffer(GL_ARRAY_BUFFER,0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
	}
}