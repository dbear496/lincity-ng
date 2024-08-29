#define GROUP_WATERWELL_COLOUR    (blue(31))
#define GROUP_WATERWELL_COST      1
#define GROUP_WATERWELL_COST_MUL 2
#define GROUP_WATERWELL_BUL_COST      1
#define GROUP_WATERWELL_TECH      0
#define GROUP_WATERWELL_FIREC 0
#define GROUP_WATERWELL_RANGE 0
#define GROUP_WATERWELL_SIZE 2

#define MAX_POLLUTION_AT_WATERWELL 3000
#define WATER_PER_UGW 400
#define MAX_WATER_AT_WATERWELL (20 * WATER_PER_UGW * GROUP_WATERWELL_SIZE * GROUP_WATERWELL_SIZE)
#include <array>                    // for array

#include "modules.h"

class WaterwellConstructionGroup: public ConstructionGroup {
public:
    WaterwellConstructionGroup(
        const char *name,
        unsigned short no_credit,
        unsigned short group,
        unsigned short size, int colour,
        int cost_mul, int bul_cost, int fire_chance,
        int cost, int tech, int range
    ): ConstructionGroup(
        name, no_credit, group, size, colour, cost_mul, bul_cost, fire_chance,
        cost, tech, range, 2/*mps_pages*/
    ) {
        commodityRuleCount[STUFF_WATER].maxload = MAX_WATER_AT_WATERWELL;
        commodityRuleCount[STUFF_WATER].give = true;
        commodityRuleCount[STUFF_WATER].take = false;
    }
    // overriding method that creates a waterwell
    virtual Construction *createConstruction();
};

extern WaterwellConstructionGroup waterwellConstructionGroup;

class Waterwell: public Construction {
public:
    Waterwell(ConstructionGroup *cstgrp) {
        this->constructionGroup = cstgrp;
        init_resources();
        this->busy = 0;
        this->working_days = 0;
        initialize_commodities();
        int w = 0;
        for (int i = 0; i < constructionGroup->size; i++)
        {
            for (int j = 0; j < constructionGroup->size; j++)
            {
                if (world(x+j, y+i)->flags & FLAG_HAS_UNDERGROUND_WATER)
                    w++;
            }// end j
        }//end i
        this->ugwCount = w;
        this->water_output = w * WATER_PER_UGW;

        commodityMaxProd[STUFF_WATER] = 100 * water_output;
    }

    virtual ~Waterwell() { }
    virtual void update();
    virtual void report();

    int water_output;
    int ugwCount;
    int working_days, busy;
};

/** @file lincity/modules/waterwell.h */
