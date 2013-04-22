#include "LevelMenuScene.h"

#include "ButtonFactory.h"
#include "Colors.h"

using namespace cocos2d;
using namespace cocos2d::extension;

extern LoaderLevel* globalLevel;

#pragma mark Initialization

LevelMenuScene::LevelMenuScene()
: scrollView(NULL)
, pageIndicator(NULL)
, totalPages(0)
, currentPage(0)
, isFirstEnter(true)
, buttons(NULL)
{
    buttons = CCArray::create();
    buttons->retain();
}

LevelMenuScene::~LevelMenuScene()
{
    CC_SAFE_RELEASE_NULL(pageIndicator);
    CC_SAFE_RELEASE_NULL(buttons);
}

CCScene* LevelMenuScene::scene()
{
    CCScene *scene = CCScene::create();
    scene->addChild(LevelMenuScene::create());
    return scene;
}

void LevelMenuScene::onEnter()
{
    CCLayer::onEnter();

    if (isFirstEnter) {
        isFirstEnter = false;
        return;
    }

    CCObject* it = NULL;
    CCARRAY_FOREACH(buttons, it) {
        auto button = static_cast<GameButton*>(it);
        if (button->getTag() == currentPage) {
            button->updateStateIndicator();
        }
    }
}


bool LevelMenuScene::init()
{
    if (!CCLayer::init()) {
        return false;
    }

    CCDirector::sharedDirector()
        ->getTouchDispatcher()
        ->addTargetedDelegate(this, 1, false);

    scrollView = CCScrollView::create(CCSize(768, 1024));
    scrollView->setDirection(kCCScrollViewDirectionHorizontal);
    scrollView->setPosition(CCPointZero);
    scrollView->setContentOffset(CCPointZero);
    scrollView->setContainer(createMenuContainer());
    scrollView->setClippingToBounds(true);
    addChild(scrollView);

    auto indicator = createPageIndicatorContainer();
    indicator->setAnchorPoint(CCPoint(0.5, 0.5));
    indicator->setPosition(CCPoint(384, 120));
    addChild(indicator);

    addChild(ButtonFactory::createSceneBackButton());
    addChild(ButtonFactory::createStar());

    return true;
}

CCNode* LevelMenuScene::createPageIndicatorContainer()
{
    assert(!pageIndicator && "pageIndicator array already set");
    assert(scrollView && "scrollView is required to be set");

    pageIndicator = CCArray::create();
    pageIndicator->retain();
    auto container = CCNode::create();
    auto size = CCSize();

    for (int page = 0; page < totalPages; ++page) {
        auto indicator = CCSprite::createWithSpriteFrameName("buttons/pageindicator.png");
        indicator->setAnchorPoint(CCPointZero);
        indicator->setPositionX(size.width);

        size.height = MAX(size.height, indicator->getContentSize().height);
        size.width += indicator->getContentSize().width + INDICATOR_SPACING;

        pageIndicator->addObject(indicator);
        container->addChild(indicator);
    }

    size.width -= INDICATOR_SPACING;
    container->setContentSize(size);

    setPageIndicatorPage();

    return container;
}

void LevelMenuScene::setPageIndicatorPage()
{
    CCObject* it = NULL;
    int iterPage = -1;

    CCARRAY_FOREACH(pageIndicator, it) {
        auto indicator = static_cast<CCSprite*>(it);
        ++iterPage;

        if (currentPage == iterPage) {
            indicator->setOpacity(255 * 0.9);
        } else {
            indicator->setOpacity(255 * 0.4);
        }
    }
}

void LevelMenuScene::adjustScrollView()
{
    assert(scrollView && "scrollView is required to be set");

    auto offset = scrollView->getContentOffset();
    int pageOffset = (int)offset.x % 768;
    currentPage = abs(offset.x) / 768;

    if (pageOffset <= -384) {
        ++currentPage;
    }

    auto maxOffset = (scrollView->getContentSize().width - 768) * -1;
    auto newOffset = CCPoint(currentPage * 768 * -1, offset.y);
    newOffset.x = fmax(newOffset.x, maxOffset);

    currentPage = MIN(currentPage, totalPages - 1);
    setPageIndicatorPage();
    
    scrollView->unscheduleAllSelectors();
    scrollView->setContentOffset(newOffset, true);
}

CCNode* LevelMenuScene::createMenuContainer()
{
    CCNode* container = CCNode::create();
    
    // add all pages
    totalPages = 0;
    for (auto page : globalLevel->page->category->pages) {
        auto pageMenu  = createPageMenu(page);
        pageMenu->setPositionX(pageMenu->getPositionX() + (768 * totalPages++));
        container->addChild(pageMenu);
    }

    // detect the right content size
    auto firstObject = container->getChildren()->objectAtIndex(0);
    auto firstPage = dynamic_cast<CCNode*>(firstObject);
    container->setContentSize(CCSize(
        totalPages * 768,
        firstPage->getContentSize().height
    ));

    // we need to wrap the container again or the positioning
    // we've done before would be removed by CCScrollView :/
    auto posContainer = CCNode::create();
    posContainer->setContentSize(container->getContentSize());
    posContainer->addChild(container);

    return posContainer;
}

CCNode* LevelMenuScene::createPageMenu(const LoaderPage* page) const
{
    auto menu = CCMenu::create();
    for (auto level : page->levels) {
        auto button = ButtonFactory::createLevelButton(level);
        button->setBorderColor(LINE_COLORS[page->localid]);
        button->setTag(page->localid - 1);

        menu->addChild(button);
        buttons->addObject(button);
    }

    alignMenu(menu);
    menu->setAnchorPoint(CCPointZero);
    menu->setPosition(
        (768 - menu->getContentSize().width) / 2,
        (1024 - menu->getContentSize().height) / 2
    );


    auto container = CCNode::create();
    container->setAnchorPoint(CCPointZero);
    container->setPosition(CCPointZero);
    container->setContentSize(menu->getContentSize());
    container->addChild(menu);

    static char headline[50] = {0};
    snprintf(headline, sizeof(headline), "%s %s", page->category->name, page->name);
    container->addChild(ButtonFactory::createHeadline(headline));

    return container;
}

void LevelMenuScene::alignMenu(CCMenu* menu) const
{
    auto btnPos = CCPoint();
    auto btnSize = CCSize();
    auto pageSize = CCSize();

    CCObject* it = NULL;
    CCARRAY_FOREACH_REVERSE(menu->getChildren(), it) {
        auto button = dynamic_cast<GameButton*>(it);

        btnSize.width = (button->getContentSize().width + ITEM_PADDING);
        btnSize.height = (button->getContentSize().height + ITEM_PADDING);

        if (btnPos.x == 0) {
            pageSize.height += btnSize.height;
        }
        if (btnPos.y == 0) {
            pageSize.width += btnSize.width;
        }

        button->setAnchorPoint(CCPointZero);
        button->setPosition(CCPoint(
            btnSize.width * (ITEMS_PER_ROW - ++btnPos.x),
            btnSize.height * btnPos.y
        ));

        if (btnPos.x >= ITEMS_PER_ROW) {
            btnPos.x = 0;
            ++btnPos.y;
        }
    }

    pageSize.width -= ITEM_PADDING;
    pageSize.height -= ITEM_PADDING;
    menu->setContentSize(pageSize);
}

bool LevelMenuScene::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    return true;
}

void LevelMenuScene::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
}

void LevelMenuScene::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
    adjustScrollView();
}

void LevelMenuScene::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent)
{
    adjustScrollView();
}