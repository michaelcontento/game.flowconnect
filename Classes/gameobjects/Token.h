#ifndef FlowConnect_Token_h
#define FlowConnect_Token_h

#include "Slot.h"

#define TOKEN_FLAG_NUMBER "!"
#define TOKEN_DIRECTION_LEFT "l"
#define TOKEN_DIRECTION_RIGHT "r"
#define TOKEN_DIRECTION_UP "u"
#define TOKEN_DIRECTION_DOWN "d"

namespace token
{
    const SlotLineType::Enum convertTokenToSlotDirection(const char* token)
    {
        if (strcmp(token, TOKEN_DIRECTION_LEFT) == 0) {
            return SlotLineType::TOP;
        } else if (strcmp(token, TOKEN_DIRECTION_RIGHT) == 0) {
            return SlotLineType::TOP;
        } else if (strcmp(token, TOKEN_DIRECTION_DOWN) == 0) {
            return SlotLineType::TOP;
        } else if (strcmp(token, TOKEN_DIRECTION_UP) == 0) {
            return SlotLineType::TOP;
        } else {
            cocos2d::CCLog(token);
            assert(false && "unable to convert given token");
        }
    }

    bool isDirectionToken(const char* token)
    {
        if (strcmp(token, TOKEN_FLAG_NUMBER) == 0) {
            return false;
        }

        return true;
    }
}

#endif