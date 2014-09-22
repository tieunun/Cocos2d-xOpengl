#include "HelloWorldScene.h"

USING_NS_CC;


HelloWorld::~HelloWorld()
{
	glDeleteProgram(shader);
	glDeleteBuffers(1,&vertexObj);
	glDeleteBuffers(1,&indexObj);
	glDeleteTextures(1,&texture);
	glDeleteTextures(1,&uvObj);
}


Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label


	//OpenGL 設定を表示.
#if !WIN32
	std::string glVersion = "GL_VERSION:";
	glVersion += (const char*)glGetString(GL_VERSION);
	std::string glVendor = "GL_VENDOR:";
	glVendor += (const char*)glGetString(GL_VENDOR);
	std::string glRenderer = "GL_RENDERER:";
	glRenderer += (const char*)glGetString(GL_RENDERER);

	std::string glInfo = glVersion + "\n" + glVendor + "\n" + glRenderer;
#else
	std::string glVersion = "GL_VERSION:";
	glVersion += (const char*)glGetString(GL_VERSION);
	std::string glShadingLanguageVersion = "GL_SHADING_LANGUAGE_VERSION:";
	glShadingLanguageVersion += (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
	std::string glVendor = "GL_VENDOR:";
	glVendor += (const char*)glGetString(GL_VENDOR);
	std::string glRenderer = "GL_RENDERER:";
	glRenderer += (const char*)glGetString(GL_RENDERER);
	std::string glInfo = glVersion + "\n" + glShadingLanguageVersion + "\n" + glVendor + "\n" + glRenderer;
#endif

	auto label = LabelTTF::create(glInfo.c_str(), "Arial", visibleSize.height*0.04,Size::ZERO,cocos2d::TextHAlignment::LEFT);

	label->setAnchorPoint(cocos2d::Vec2(0,1));
	// position the label on the center of the screen
	label->setPosition(Vec2(origin.x + 0/*visibleSize.width/2*/,
		origin.y + visibleSize.height - /*label->getContentSize().height*/0));

	// add the label as a child to this layer
	this->addChild(label, 1);  

#if 0
	// add "HelloWorld" splash screen"
	auto sprite = Sprite::create("HelloWorld.png");

	// position the sprite on the center of the screen
	sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

	// add the sprite as a child to this layer
	this->addChild(sprite, 0);  
#endif // 0

//	KJH::VSCreateShader("shader.vsh");

	//シェーダーのコンパイル.
#if 1
	GLuint vs = KJH::VSCreateShader("shader.vsh");

	GLuint fs = KJH::FSCreateShader("shader.fsh");

	shader = glCreateProgram();
	glAttachShader(shader,vs);
	glAttachShader(shader,fs);
	glLinkProgram(shader);
	GLint linked;
	glGetProgramiv(shader,GL_LINK_STATUS,&linked);
	if(linked == GL_FALSE){
		assert(0);
	}
	glDeleteShader(vs);
	glDeleteShader(fs);



	cocos2d::Vec3 triangleVertex[] = {
		Vec3(-0.5,0.5,0),
		Vec3(-0.5,-0.5,0),
		Vec3(0.5,0.5,0),
		Vec3(0.5,-0.5,0),
	};
	unsigned short triangleIndex[] ={
		0,1,2,
		2,1,3
	};


	//バッファ作成.
	//unsigned int vertexObj;
	glGenBuffers(1,&vertexObj);
	glBindBuffer(GL_ARRAY_BUFFER,vertexObj);
	glBufferData(GL_ARRAY_BUFFER,sizeof(triangleVertex[0])*4,triangleVertex,GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER,0);


	//unsigned int indexObj;
	glGenBuffers(1,&indexObj);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexObj );
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(triangleIndex[0])*6,triangleIndex,GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);


#endif // 0


	unsigned char textureByte[][4] = {
		{255,0,0,255},{255,255,0,255},
		{0,255,0,255},{0,0,255,255},
	};

	//テクスチャ生成.
	glGenTextures(1,&texture);
	

	glBindTexture(GL_TEXTURE_2D,texture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,2,2,0,GL_RGBA,GL_UNSIGNED_BYTE,textureByte);
	
	GLenum error = glGetError();
    

	float triangleUV[4][2] = {
		{0, 0},
		{0, 1},
		{1, 0},
		{1, 1},
	};
	//UV生成.
	glGenBuffers(1,&uvObj);
	glBindBuffer(GL_ARRAY_BUFFER,uvObj);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*2*4, triangleUV, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)

	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
    return;
#endif

    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}


//-------------------------------------------------------------------------------------------------------------------------------------
//! @brief シーン描画関数.
//-------------------------------------------------------------------------------------------------------------------------------------
void HelloWorld::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags)
{
#if 1
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	//描画開始.
	int nowPG;
	glGetIntegerv(GL_CURRENT_PROGRAM,&nowPG);
	glUseProgram(shader);

	//アトリビュートに頂点バッファをバインド.
	unsigned int attr = glGetAttribLocation(shader,"vertex");
	glBindBuffer(GL_ARRAY_BUFFER,vertexObj);
	glVertexAttribPointer(attr,3,GL_FLOAT,GL_FALSE,0,NULL);
	glEnableVertexAttribArray(attr);
	//UVをバインド.
	attr = glGetAttribLocation(shader, "uv");
	glBindBuffer(GL_ARRAY_BUFFER, uvObj);
	glVertexAttribPointer(attr, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glEnableVertexAttribArray(attr);
	//テクスチャ設定.
	glBindTexture(GL_TEXTURE_2D, texture);

	////インデックスバッファ.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexObj);
	////描画命令.
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);

	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);


	////終わったら元に戻す.
	glUseProgram(nowPG);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);  
#endif // 0

}