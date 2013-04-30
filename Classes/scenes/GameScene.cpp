#include "GameScene.h"

#include "SceneManager.h"
#include "ButtonFactory.h"
#include "../Colors.h"
#include "Globals.h"
#include "Localization.h"
#include "LanguageKey.h"
#include "userstate.h"
#include "LevelMenuScene.h"

using namespace cocos2d;

#pragma mark Initialization

GameScene::GameScene()
: board(NULL)
, leftMenu(NULL)
, rightMenu(NULL)
, stats(NULL)
, headlineLabel(NULL)
, hintLabel(NULL)
, hintNumber(NULL)
{
}

GameScene::~GameScene()
{
}

CCScene* GameScene::scene()
{
    CCScene *scene = CCScene::create();
    scene->addChild(GameScene::create());
    return scene;
}

bool GameScene::init()
{
    if (!CCLayer::init()) {
        return false;
    }

    addChild(ButtonFactory::createSceneBackButton());
    addChild(ButtonFactory::createStar());
    initLeftMenu();
    initRightMenu();
    
    initBoard();

    return true;
}

void GameScene::onBtnGoBack()
{
    LoaderLevel* prevLevel = globalLevel->prev;
    if (!prevLevel) {
        LoaderPage* prePage = globalLevel->page->prev;
        if (prePage) {
            if (!userstate::isPageFree(prePage)) {
                CCMessageBox(
                    _("dialog.pagelocked", "body")->getCString(),
                    _("dialog.pagelocked", "headline")->getCString()
                );
                LevelMenuScene::scrollTo = prePage;
                SceneManager::getInstance().popScene();
            } else {
                prevLevel = prePage->levels.back();
            }
        }
    }

    if (prevLevel) {
        globalLevel = prevLevel;
        initBoardAndFade();
    }
}

void GameScene::onBtnReset()
{
    board->reset();
}

void GameScene::onBtnGoNext()
{
    LoaderLevel* nextLevel = globalLevel->next;
    if (!nextLevel) {
        LoaderPage* nextPage = globalLevel->page->next;
        if (nextPage) {
            if (!userstate::isPageFree(nextPage)) {
                CCMessageBox(
                    _("dialog.pagelocked", "body")->getCString(),
                    _("dialog.pagelocked", "headline")->getCString()
                );
                LevelMenuScene::scrollTo = nextPage;
                SceneManager::getInstance().popScene();
            } else {
                nextLevel = nextPage->levels.front();
            }
        }
    }

    if (nextLevel) {
        globalLevel = nextLevel;
        initBoardAndFade();
    }
}

void GameScene::onBtnHint()
{
    if (board->isFinished()) {
        return;
    }
    
    auto freeHints = userstate::getFreeHints();
    auto showWarning = userstate::showHintWarning();

    if (freeHints == 0 && showWarning) {
        CCMessageBox(
            _("dialog.hintwarning", "body")->getCString(),
            _("dialog.hintwarning", "headline")->getCString()
        );
        userstate::setHintWarning(false);
        return;
    }

    if (freeHints == 0 && !userstate::addStarsToUser(PRICE_HINT * -1)) {
        return;
    }

    if (board->finishTillNextCheckpoint() && freeHints > 0) {
        userstate::addFreeHint(-1);
        updateHintLabel();
    }
}

void GameScene::onBtnHelp()
{
    CCLog("HELP");
}

void GameScene::onBtnMenu()
{
    SceneManager::getInstance().popScene();
}

void GameScene::initBoardAndFade()
{
    removeChild(boardContainer);
    initBoard();

    auto oldScale = boardContainer->getScale();
    boardContainer->setScale(boardContainer->getScale() * 0.5);

    auto action = CCSequence::create(
        CCEaseOut::create(CCScaleTo::create(0.2, oldScale), 1),
        CCCallFunc::create(this, callfunc_selector(GameScene::enableMenus)),
        NULL
    );

    enableMenus(false);
    boardContainer->runAction(action);
}

void GameScene::enableMenus()
{
    enableMenus(true);
}

void GameScene::enableMenus(const bool flag)
{
    leftMenu->setEnabled(flag);
    rightMenu->setEnabled(flag);
}

void GameScene::initBoard()
{
    board = Board::create();
    board->initWithLevel(globalLevel);
    addBoardWithinContainer(board);

    if (headlineLabel) {
        removeChild(headlineLabel);
    }
    
    headlineLabel = ButtonFactory::createHeadline(globalLevel->getLocalizedName());
    addChild(headlineLabel);
}

void GameScene::addBoardWithinContainer(Board* board)
{
    auto boardContainerInner = CCNode::create();
    boardContainerInner->setContentSize(board->getContentSize());
    boardContainerInner->setScale(BOARD_WIDTH / board->getContentSize().width);
    boardContainerInner->setAnchorPoint(CCPoint(0.5, 0.5));
    boardContainerInner->addChild(board);

    stats = BoardStats::createWithBoard(board);
    stats->setAnchorPoint(CCPoint(0.5, 0));
    stats->setPositionY(BOARD_WIDTH / 2);

    boardContainer = CCNode::create();
    boardContainer->setAnchorPoint(CCPoint(0.5, 0.5));
    boardContainer->setPosition(CCPoint(384, 512));
    boardContainer->addChild(boardContainerInner);
    boardContainer->addChild(stats);
    addChild(boardContainer);
}

CCMenuItemSprite* GameScene::createMenuitem(const char* imagename, CCMenu* menu, SEL_MenuHandler selector)
{
    auto normal = CCSprite::createWithSpriteFrameName(imagename);
    auto child = CCMenuItemSprite::create(normal, normal, normal, this, selector);

    menu->addChild(child);
    return child;
}

void GameScene::initRightMenu()
{
    rightMenu = CCMenu::create();
    addChild(rightMenu);

    createMenuitem("buttons/go-back.png", rightMenu, menu_selector(GameScene::onBtnGoBack));
    createMenuitem("buttons/reset.png", rightMenu, menu_selector(GameScene::onBtnReset));
    createMenuitem("buttons/go-next.png", rightMenu, menu_selector(GameScene::onBtnGoNext));

    rightMenu->setPositionX((768 - BOARD_WIDTH) / 2 + BOARD_WIDTH);
    rightMenu->setPositionY((1024 - BOARD_WIDTH) / 4);
    rightMenu->setAnchorPoint(CCPoint(1, 0.5));

    rightMenu->alignItemsHorizontallyWithPadding(BUTTON_SPACING);
}

void GameScene::updateHintLabel()
{
    auto hints = userstate::getFreeHints();

    if (hints == 0) {
        hintNumber->setVisible(false);
    } else {
        hintNumber->setVisible(true);

        char buf[10] = {0};
        snprintf(buf, sizeof(buf), "%d", hints);
        hintLabel->setString(buf);
    }
}

void GameScene::initLeftMenu()
{
    leftMenu = CCMenu::create();
    addChild(leftMenu);

    auto bg = CCSprite::createWithSpriteFrameName("buttons/hint-number.png");
    bg->setAnchorPoint(CCPointZero);

    hintLabel = CCLabelTTF::create("0", DEFAULT_FONT_NAME, 24);
    hintLabel->setPosition(ccpMult(ccpFromSize(bg->getContentSize()), 0.5));
    hintLabel->setAnchorPoint(CCPoint(0.5, 0.5));
    hintLabel->setPositionY(hintLabel->getPositionY() + 1);

    createMenuitem("buttons/help.png", leftMenu, menu_selector(GameScene::onBtnHelp));

    auto hint = createMenuitem("buttons/hint.png", leftMenu, menu_selector(GameScene::onBtnHint));
    hintNumber = CCNode::create();
    hintNumber->addChild(bg);
    hintNumber->addChild(hintLabel);
    hintNumber->setContentSize(bg->getContentSize());
    hintNumber->setAnchorPoint(CCPoint(0.5, 0.5));
    hintNumber->setPosition(
        hint->getContentSize().width - 5,
        hint->getContentSize().height - 5
    );
    hint->addChild(hintNumber);
    updateHintLabel();

    leftMenu->setPositionX((768 - BOARD_WIDTH) / 2);
    leftMenu->setPositionY((1024 - BOARD_WIDTH) / 4);
    leftMenu->setAnchorPoint(CCPoint(0, 0.5));

    leftMenu->alignItemsHorizontallyWithPadding(BUTTON_SPACING);
}