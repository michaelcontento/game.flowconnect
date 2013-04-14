#ifndef __FlowConnect__Slot__
#define __FlowConnect__Slot__

#include "cocos2d.h"

#define SLOT_DEFAULT_NUMBER -1

#define SLOT_ZORDER_LINE_LAYER 7
#define SLOT_ZORDER_BACKGROUND 8
#define SLOT_ZORDER_LABEL_BACKGROUND 9
#define SLOT_ZORDER_LABEL 10

namespace SlotLineType
{
    enum Enum {
        NONE = 0,
        TOP,
        RIGHT,
        BOTTOM,
        LEFT
    };
}

class Slot : public cocos2d::CCNode
{
public:
    cocos2d::CCPoint gridIndex;
    
    Slot();
    virtual ~Slot();

    CREATE_FUNC(Slot);
    virtual bool init();

    CC_SYNTHESIZE_READONLY(int, number, Number)
    void setNumber(const int newNumber);
    bool isCheckpoint() const;

    CC_SYNTHESIZE_READONLY(SlotLineType::Enum, lineIn, LineIn);
    void setLineIn(const SlotLineType::Enum line);

    CC_SYNTHESIZE_READONLY(SlotLineType::Enum, lineOut, LineOut);
    void setLineOut(const SlotLineType::Enum line);

    void lockLineIn(const bool flag);
    void lockLineOut(const bool flag);
    bool isLocked() const;

    void markAsNextSlot(const bool flag);
    bool isNextSlot() const;

    void reset();
    bool isFree() const;

    cocos2d::CCSize getSize();

private:
    bool lineInLocked;
    bool lineOutLocked;
    bool isNextSlot_;
    cocos2d::CCSprite* background;
    cocos2d::CCLayer* lineLayer;
    cocos2d::CCLabelTTF* label;
    cocos2d::CCSprite* labelBackground;
    cocos2d::CCAction* labelPulseAction;

    void updateLineImage();
    bool isLineOpposite(const SlotLineType::Enum lineIn, const SlotLineType::Enum lineOut) const;
    void addLineImage(const SlotLineType::Enum type, const char* file, const bool locked);
    void hideNumber();
    void showNumber();
};

#endif /* defined(__FlowConnect__Slot__) */