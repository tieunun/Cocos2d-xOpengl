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
			m_vertex[i].normal = vertPmd.normal;
			m_vertex[i].uv = vertPmd.uv;
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
		return 0;
	}

#define BUFFER_OFFSET(bytes) ((GLubyte *)NULL + (bytes))

	void Mesh::Draw(unsigned int shader)
	{
		//アトリビュートに頂点バッファをバインド.
		//位置.
		glBindBuffer(GL_ARRAY_BUFFER,m_vertexBuffer);
		unsigned int attr = glGetAttribLocation(shader,"in_pos");
		glVertexAttribPointer(attr,3,GL_FLOAT,GL_FALSE,sizeof(m_vertex[0]),NULL);
		glEnableVertexAttribArray(attr);
		//法線.
		attr = glGetAttribLocation(shader,"in_normal");
		glVertexAttribPointer(attr,3,GL_FLOAT,GL_FALSE,sizeof(m_vertex[0]),(GLuint*)sizeof(m_vertex[0].pos));
		glEnableVertexAttribArray(attr);
		//UV.
		attr = glGetAttribLocation(shader,"in_uv");
		glVertexAttribPointer(attr,2,GL_FLOAT,GL_FALSE,sizeof(m_vertex[0]),(GLuint*)(sizeof(m_vertex[0].pos)+sizeof(m_vertex[0].normal)));
		glEnableVertexAttribArray(attr);
		auto error = glGetError();
		assert(error == 0);

		//インデックスバッファ.
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
		GLuint offset = 0;
		//マテリアルループ.
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
			//描画命令.
			glDrawElements(GL_TRIANGLES,m_material[i].indexNum, GL_UNSIGNED_SHORT, BUFFER_OFFSET(offset));
			offset += sizeof(m_index[0]) * m_material[i].indexNum;
			error = glGetError();
		}
		glBindBuffer(GL_ARRAY_BUFFER,0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
	}
}