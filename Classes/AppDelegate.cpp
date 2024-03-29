#include "AppDelegate.h"

#include <avalon/ads/Manager.h>
#include "scenes/SceneManager.h"
#include "scenes/IntroScene.h"
#include "SimpleAudioEngine.h"
#include "userstate.h"

using namespace cocos2d;
using namespace CocosDenshion;

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate() 
{
}

#pragma mark Lifecycle

bool AppDelegate::applicationDidFinishLaunching()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    //pDirector->setDisplayStats(true);

    CCEGLView* pEGLView = CCEGLView::sharedOpenGLView();
    pDirector->setOpenGLView(pEGLView);
    pEGLView->setDesignResolutionSize(768, 1024, kResolutionExactFit);

    SceneManager::getInstance().gotoScene(IntroScene::scene());
    return true;
}

void AppDelegate::applicationDidEnterBackground()
{
    SimpleAudioEngine::sharedEngine()->pauseAllEffects();
    SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();

    CCDirector::sharedDirector()->stopAnimation();
    CCDirector::sharedDirector()->pause();
}

void AppDelegate::applicationWillEnterForeground()
{
    CCDirector::sharedDirector()->resume();
    CCDirector::sharedDirector()->startAnimation();

    if (userstate::hasMusic()) {
        SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
    }
    SimpleAudioEngine::sharedEngine()->resumeAllEffects();

    userstate::refreshFreeHints();
}
