#include "MainScene.h"

USING_NS_CC;


MainScene::MainScene()
	: mesh()
	, status()
	, shader(0)
{

}


MainScene::~MainScene()
{
	glDeleteProgram(shader);
	CCLOG("========Scene End========");
}


Scene* MainScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = MainScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool MainScene::init()
{
	CCLOG("--Load Begin--");
    //////////////////////////////
    // 1. super init first
	if( !Layer::init() )
	{
		return false;
	}
	glClearColor(1.0,0.7,0.7,1.0);
	CCLOG("--Check1--");

	status.Init();
	status.Log();



    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(MainScene::menuCloseCallback, this));
    
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

	CCLOG("--Check2--");

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
	CCLOG("--Check3--");
	auto label = LabelTTF::create(glInfo.c_str(), "Arial", visibleSize.height*0.04,Size::ZERO,cocos2d::TextHAlignment::LEFT);

	label->setAnchorPoint(cocos2d::Vec2(0,1));
	// position the label on the center of the screen
	label->setPosition(Vec2(origin.x + 0/*visibleSize.width/2*/,
		origin.y + visibleSize.height - /*label->getContentSize().height*/0));

	// add the label as a child to this layer
	this->addChild(label, 1);  

	//シェーダーのコンパイル.
	CCLOG("--Check VS--");
	GLuint vs = KJH::VSCreateShader("shader.vsh");
	CCLOG("--Check PS--");
	GLuint fs = KJH::FSCreateShader("shader.fsh");
	CCLOG("--Check Shader Link--");
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
	CCLOG("--Check5--");

	
	GLenum error = glGetError();
    

	mesh.Load("miku.pmd");
	CCLOG("--Load End--");




    return true;
}


void MainScene::menuCloseCallback(Ref* pSender)
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
void MainScene::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags)
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

	static float d = 0;
	d += 0.01f;
	KJH::Mat world = KJH::Mat::RotateY(d);
	KJH::Mat view = KJH::Mat::LookAtRH(KJH::Float3(30,30,30),KJH::Float3(0,10,0),KJH::Float3(0,1,0));
	KJH::Mat proj = KJH::Mat::PerspectiveFovRH(KJH::ToRadian(45),Director::getInstance()->getVisibleSize().width / Director::getInstance()->getVisibleSize().height,1,100);
	
	//world = KJH::Mat::Scale(0.05f);
	view = KJH::Mat::Transpose(view);
	proj = KJH::Mat::Transpose(proj);
	world = KJH::Mat::Transpose(world);
	//ビュー行列とプロジェクション行列を適用させる.
	unsigned int attr = glGetUniformLocation(shader, "view");
	glUniformMatrix4fv(attr, 1, GL_FALSE, (float*) &view);
	attr = glGetUniformLocation(shader, "proj");
	glUniformMatrix4fv(attr, 1, GL_FALSE, (float*) &proj);
	attr = glGetUniformLocation(shader, "world");
	glUniformMatrix4fv(attr, 1, GL_FALSE, (float*) &world);
	
	auto error = glGetError();
	CCASSERT(error == 0,"Uniform");

	mesh.Draw(shader);
	error = glGetError();
	if(error != 0){
		CCLOG("Draw Error!");
		assert(0);
	}
	////終わったら元に戻す.
	glUseProgram(nowPG);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

#endif // 0

}