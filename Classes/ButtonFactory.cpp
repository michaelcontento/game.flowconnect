#include "ButtonFactory.h"

#include "Globals.h"
#include "SceneManager.h"
#include "Colors.h"
#include "StarButton.h"

#define BOARD_WIDTH 696
#define TOP_MARGIN (1024 - 50)
#define SIDE_MARGIN ((768 - BOARD_WIDTH) / 2)

using namespace cocos2d;

unsigned int ButtonFactory::colorCounter = 0;

void ButtonFactory::resetColorCounter()
{
    ButtonFactory::colorCounter = 0;
}

CCMenuItemSprite* ButtonFactory::create(const char* text, CCObject* target, SEL_MenuHandler selector)
{
    auto sprite = CCSprite::createWithSpriteFrameName("buttons/borders/normal.png");
    auto button = CCMenuItemSprite::create(sprite, sprite);
    button->setTarget(target, selector);

    auto label = CCLabelTTF::create(text, DEFAULT_FONT_NAME, 48);
    label->setAnchorPoint(CCPoint(0.5, 0.5));
    label->setPosition(ccpMult(ccpFromSize(button->getContentSize()), 0.5));
    label->setColor(LINE_COLORS[ButtonFactory::colorCounter++]);
    button->addChild(label);

    return button;
}

CCLabelTTF* ButtonFactory::createHeadline(const char* text)
{
    auto label = CCLabelTTF::create(text, DEFAULT_FONT_NAME, 36);
    label->setAnchorPoint(CCPoint(0, 0.5));
    label->setPositionX(SIDE_MARGIN + 75);
    label->setPositionY(TOP_MARGIN);
    label->setOpacity(DISABLED_OPACITY);

    return label;
}

CCMenuItemSprite* ButtonFactory::createCategory(LoaderCategory* category, CCObject* target, SEL_MenuHandler selector)
{
    auto sprite = CCSprite::createWithSpriteFrameName("buttons/borders/normal.png");
    auto button = CCMenuItemSprite::create(sprite, sprite);
    button->setTarget(target, selector);

    auto name = CCLabelTTF::create(category->name, DEFAULT_FONT_NAME, 36);
    name->setAnchorPoint(CCPoint(0, 1));
    name->setPosition(CCPoint(35, button->getContentSize().height - 7));
    name->setColor(LINE_COLORS[ButtonFactory::colorCounter++]);
    button->addChild(name);

    if (category->description) {
        auto desc = CCLabelTTF::create(category->description, SMALL_FONT_NAME, 28);
        desc->setAnchorPoint(CCPoint(0, 0));
        desc->setPosition(CCPoint(35, 7));
        desc->setOpacity(DISABLED_OPACITY);
        button->addChild(desc);
    } else {
        name->setAnchorPoint(CCPoint(0, 0.5));
        name->setPositionY(button->getContentSize().height / 2);
    }

    if (category->pages.size() > 0) {
        char buf[10] = {0};
        snprintf(
            buf, sizeof(buf),
            "%d / %d",
            category->countLevelsSolved(),
            category->countLevels()
        );

        auto solved = CCLabelTTF::create(buf, SMALL_FONT_NAME, 36);
        solved->setOpacity(DISABLED_OPACITY);
        solved->setAnchorPoint(CCPoint(1, 0.5));
        solved->setPosition(CCPoint(
            button->getContentSize().width - 30,
            button->getContentSize().height / 2
        ));
        button->addChild(solved);
    }

    return button;
}

CCMenu* ButtonFactory::createSceneBackButton()
{
    auto menu = CCMenu::create();

    auto normal = CCSprite::createWithSpriteFrameName("buttons/home.png");
    menu->addChild(CCMenuItemSprite::create(
        normal, normal, normal,
        &SceneManager::getInstance(), menu_selector(SceneManager::popScene)
    ));

    menu->setPositionX(SIDE_MARGIN);
    menu->setPositionY(TOP_MARGIN);
    menu->setAnchorPoint(CCPoint(0, 0.5));
    menu->alignItemsHorizontally();

    return menu;
}

GameButton* ButtonFactory::createLevelButton(const LoaderLevel* level)
{
    return GameButton::createWithLevel(level);
}

CCMenuItem* ButtonFactory::createEmptyButton()
{
    auto result = CCMenuItem::create();
    result->setContentSize(CCSize(0, 0));
    return result;
}

StarButton* ButtonFactory::createStar()
{
    auto star = StarButton::create();
    star->setPositionX(768 - SIDE_MARGIN);
    star->setPositionY(TOP_MARGIN);
    star->setAnchorPoint(CCPoint(1, 0.5));

    return star;
}