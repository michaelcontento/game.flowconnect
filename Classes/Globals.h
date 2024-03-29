#ifndef FlowConnect_Globals_h
#define FlowConnect_Globals_h

#include "LevelLoader.h"

#define DEFAULT_FONT_NAME "RobotoCondensed-Bold"
#define DEFAULT_FNT_42 "RobotoCondensed-Bold-42.fnt"
#define DEFAULT_FNT_36 "RobotoCondensed-Bold-36.fnt"
#define SMALL_FONT_NAME "RobotoCondensed-Regular"
#define SMALL_FNT_36 "RobotoCondensed-Regular-36.fnt"
#define SMALL_FNT_28 "RobotoCondensed-Regular-28.fnt"

#define PRICE_REMOVE_ADS 75
#define PRICE_HINT 1
#define PRICE_PAGE 36
#define PRICE_TIMEATTACK_CONTINUE 2
#define AMOUNT_TIMEATTACK_MODES 5

#define TAG_GOOGLE_LABEL 42

class StarButton;

extern LevelLoader globalLevelLoader;
extern const LoaderLevel* globalLevel;
extern StarButton* globalLastStarButton;
extern std::vector<LoaderLevel*> globalAttackLevels;
extern std::vector<LoaderLevel*>::iterator globalAttackIterator;

#endif