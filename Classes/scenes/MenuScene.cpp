#include "MenuScene.h"

#include <avalon/GameCenter.h>
#include <avalon/i18n/LanguageKey.h>
#include <avalon/i18n/Localization.h>
#include <avalon/payment/Loader.h>
#include <avalon/ads/Manager.h>
#include "SceneManager.h"
#include "CategoryMenuScene.h"
#include "ShopScene.h"
#include "TimeAttackMenuScene.h"
#include "../buttons/ButtonFactory.h"
#include "../Globals.h"
#include "SettingsScene.h"
#include "SimpleAudioEngine.h"
#include "userstate.h"

using namespace cocos2d;
using avalon::i18n::_;

#pragma mark Initialization

MenuScene::MenuScene()
: menu(NULL)
{
}

MenuScene::~MenuScene()
{
}

CCScene* MenuScene::scene()
{
    CCScene *scene = CCScene::create();
    scene->addChild(MenuScene::create());
    return scene;
}

void MenuScene::onEnter()
{
    CCLayer::onEnter();
    avalon::ads::Manager::showBannerIgnoreTime();
    
    if (menu && !userstate::showAds()) {
        menu->removeChildByTag(TAG_REMOVEADS);
    }
}

bool MenuScene::init()
{
    if (!CCLayer::init()) {
        return false;
    }

    menu = CCMenu::create();
    addChild(menu);

    menu->addChild(ButtonFactory::create(_("menu.main", "play").get().c_str(), this, menu_selector(MenuScene::btnPlay)));
#if !(AVALON_PLATFORM_IS_IOS || AVALON_PLATFORM_IS_ANDROID_AMAZON || AVALON_PLATFORM_IS_ANDROID_GOOGLE)
    menu->addChild(ButtonFactory::createEmptyButton());
#endif
    menu->addChild(ButtonFactory::create(_("menu.main", "timeattack").get().c_str(), this, menu_selector(MenuScene::btnPLayTimeAttack)));
    menu->addChild(ButtonFactory::createEmptyButton());

#if AVALON_PLATFORM_IS_IOS || AVALON_PLATFORM_IS_ANDROID_AMAZON || AVALON_PLATFORM_IS_ANDROID_GOOGLE
    menu->addChild(ButtonFactory::create(_("menu.main", "leaderboard").get().c_str(), this, menu_selector(MenuScene::btnLeaderboard)));
    menu->addChild(ButtonFactory::create(_("menu.main", "achievements").get().c_str(), this, menu_selector(MenuScene::btnAchievements)));
#if AVALON_PLATFORM_IS_ANDROID_GOOGLE
    menu->addChild(ButtonFactory::createGoogleButton(this, menu_selector(MenuScene::btnGoogle)));
#endif
    menu->addChild(ButtonFactory::createEmptyButton());
#endif

    menu->addChild(ButtonFactory::create(_("menu.main", "shop").get().c_str(), this, menu_selector(MenuScene::btnShop)));
#if !(AVALON_PLATFORM_IS_IOS || AVALON_PLATFORM_IS_ANDROID_AMAZON || AVALON_PLATFORM_IS_ANDROID_GOOGLE)
    menu->addChild(ButtonFactory::createEmptyButton());
#endif
    menu->addChild(ButtonFactory::create(_("menu.main", "settings").get().c_str(), this, menu_selector(MenuScene::btnSettings)));

    menu->alignItemsVerticallyWithPadding(MENU_PADDING);
    addChild(ButtonFactory::createStar());

#if AVALON_PLATFORM_IS_ANDROID_GOOGLE
    auto ggames = CCSprite::createWithSpriteFrameName("google/games.png");
    ggames->setAnchorPoint(CCPoint(1, 0));
    ggames->setPosition(CCPoint(768 - 10, 10));
    addChild(ggames);
#endif

    if (userstate::showAds()) {
        auto name = std::string("removeads");
        auto langId = CCApplication::sharedApplication()->getCurrentLanguage();
        if (langId == kLanguageGerman) {
            name += "-de";
        }
        name += ".png";

        auto sprite = CCSprite::createWithSpriteFrameName(name.c_str());
        auto btn = CCMenuItemSprite::create(sprite, sprite, this, menu_selector(MenuScene::btnRemoveAds));
        btn->setTag(TAG_REMOVEADS);
        btn->setPosition(CCPoint(-384, 512));
        btn->setAnchorPoint(CCPoint(0, 1));
        menu->addChild(btn);
    }

    setKeypadEnabled(true);
    return true;
}

void MenuScene::keyBackClicked()
{
    CCDirector::sharedDirector()->end();
#if AVALON_PLATFORM_IS_IOS
    exit(0);
#endif
}

void MenuScene::btnPlay()
{
    CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("click.mp3");
    SceneManager::getInstance().gotoScene(CategoryMenuScene::scene());
}

void MenuScene::btnPLayTimeAttack()
{
    CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("click.mp3");
    SceneManager::getInstance().gotoScene(TimeAttackMenuScene::scene());
}

void MenuScene::btnLeaderboard()
{
    CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("click.mp3");
    auto gc = avalon::GameCenter();

#if AVALON_PLATFORM_IS_ANDROID_GOOGLE
    if (!gc.isSignedIn()) {
        showGoogleLoginDialog();
        return;
    }
#endif

    if (!gc.showScores()) {
        showGameCenterDialog();
        return;
    }
}

void MenuScene::btnAchievements()
{
    CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("click.mp3");
    auto gc = avalon::GameCenter();

#if AVALON_PLATFORM_IS_ANDROID_GOOGLE
    if (!gc.isSignedIn()) {
        showGoogleLoginDialog();
        return;
    }
#endif

    if (!gc.showAchievements()) {
        showGameCenterDialog();
        return;
    }
}

void MenuScene::btnShop()
{
    CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("click.mp3");
    SceneManager::getInstance().gotoScene(ShopScene::scene());
}

void MenuScene::btnSettings()
{
    CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("click.mp3");
    SceneManager::getInstance().gotoScene(SettingsScene::scene());
}

void MenuScene::btnRemoveAds()
{
    SettingsScene::triggerRemoveAds = true;
    btnSettings();
}

void MenuScene::btnGoogle(CCObject* object)
{
    auto btn = static_cast<CCMenuItemSprite*>(object);
    if (btn == NULL) {
        return;
    }

    auto label = static_cast<CCLabelTTF*>(btn->getChildByTag(TAG_GOOGLE_LABEL));
    if (label == NULL) {
        return;
    }

#if AVALON_PLATFORM_IS_ANDROID_GOOGLE
    auto gc = avalon::GameCenter();
    if (gc.isSignedIn()) {
        gc.signOut();
        label->setString(_("btn.googlelogin", "signin").get().c_str());
    } else {
        gc.signIn();
        label->setString(_("btn.googlelogin", "signout").get().c_str());
    }
#endif
}

void MenuScene::showGoogleLoginDialog()
{
    CCMessageBox(
        _("dialog.googleloginrequired", "body").get().c_str(),
        _("dialog.googleloginrequired", "headline").get().c_str()
    );
}

void MenuScene::showGameCenterDialog()
{
    char section[50] = {0};
    snprintf(
        section, sizeof(section),
        "dialog.gamecentererror.%s%s",
        avalon::utils::platform::getName().c_str(),
        avalon::utils::platform::getFlavor().c_str()
    );
    
    CCMessageBox(
        _(section, "body").get().c_str(),
        _(section, "headline").get().c_str()
    );
}