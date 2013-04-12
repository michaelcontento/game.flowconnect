#ifndef __FlowConnect__Board__
#define __FlowConnect__Board__

#include "cocos2d.h"
#include "Slot.h"
#include <vector>

#define BOARD_ZORDER_TOUCH_INDICATOR 10

class Board : public cocos2d::CCLayer
{
public:
    Board();
    virtual ~Board();

    CREATE_FUNC(Board);
    virtual bool init();
    bool initWithLevel(const cocos2d::CCSize newSize, const char* data);

    CC_SYNTHESIZE_READONLY(cocos2d::CCSize, size, Size);

    virtual bool ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    virtual void ccTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    virtual void ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    virtual void ccTouchCancelled(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);

private:
    cocos2d::CCArray* slots;
    cocos2d::CCArray* userPath;
    std::vector<int> directions;
    Slot* lastCheckpoint;
    cocos2d::CCSprite* touchIndicator;
    bool allCheckpointVisited;

    void removeAllSlots();
    void createSlotsFromData(const char* data);
    void positionSlotsOnScreen();
    void clearAllSlotsAfter(Slot* slot) const;
    void activateNextCheckpoint();
    void lockCompleteLines() const;
    Slot* getLastUserPathSlot() const;
    Slot* getUserPathSlotBefore(const Slot* slot) const;
    void appendUserPath(Slot* slot);
    void handleAllCheckpointsVisited();

    bool isFirstCheckpoint(const Slot* slot) const;
    void createTouchIndicator();
    int convert2dTo1dIndex(const cocos2d::CCPoint grid) const;
    cocos2d::CCPoint get2dIndexFromPoint(const cocos2d::CCPoint point) const;
    Slot* getSlotFromPoint(const cocos2d::CCPoint point) const;
};

#endif /* defined(__FlowConnect__Board__) */