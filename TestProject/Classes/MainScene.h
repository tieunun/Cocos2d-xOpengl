#ifndef __MAIN_SCENE_H__
#define __MAIN_SCENE_H__

#include "cocos2d.h"
#include "KJHUtility.h"
#include "KJHMesh.h"
#include "KJHMath.h"
#include "KJHGL.h"


class MainScene : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(MainScene);


	~MainScene();

	MainScene();

	//-------------------------------------------------------------------------------------------------------------------------------------
	//! @brief シーン描画関数.
	//-------------------------------------------------------------------------------------------------------------------------------------
	void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags);
protected:

	unsigned int shader;
	KJH::Mesh mesh;
	KJH::GLStatus status;
};

#endif // __MAIN_SCENE_H__
