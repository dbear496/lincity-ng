/* ---------------------------------------------------------------------- *
 * cricket.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */


#include "cricket.h"

#include <list>                     // for _List_iterator
#include <vector>                   // for vector

#include "gui_interface/mps.h"      // for mps_store_sd, mps_store_sfp, mps_...
#include "lincity/engine.h"         // for real_time
#include "lincity/groups.h"         // for GROUP_CRICKET
#include "lincity/lin-city.h"       // for ANIM_THRESHOLD, FALSE, FLAG_CRICK...
#include "lincity/lintypes.h"       // for Commodity, ExtraFrame, Constructi...
#include "lincity/stats.h"          // for cricket_cost
#include "tinygettext/gettext.hpp"  // for N_

// cricket place:
CricketConstructionGroup cricketConstructionGroup(
    N_("Basketball court"),
     FALSE,                     /* need credit? */
     GROUP_CRICKET,
     GROUP_CRICKET_SIZE,
     GROUP_CRICKET_COLOUR,
     GROUP_CRICKET_COST_MUL,
     GROUP_CRICKET_BUL_COST,
     GROUP_CRICKET_FIREC,
     GROUP_CRICKET_COST,
     GROUP_CRICKET_TECH,
     GROUP_CRICKET_RANGE
);

Construction *CricketConstructionGroup::createConstruction(int x, int y) {
    return new Cricket(x, y, this);
}

void Cricket::update()
{
    ++daycount;
    if (commodityCount[STUFF_JOBS] >= CRICKET_JOBS
    &&  commodityCount[STUFF_GOODS] >= CRICKET_GOODS
    &&  commodityCount[STUFF_WASTE] + (CRICKET_GOODS / 3) <= MAX_WASTE_AT_CRICKET)
    {
        consumeStuff(STUFF_JOBS, CRICKET_JOBS);
        consumeStuff(STUFF_GOODS, CRICKET_GOODS);
        produceStuff(STUFF_WASTE, CRICKET_GOODS / 3);
        ++covercount;
        ++working_days;
    }
    //monthly update
    if (total_time % 100 == 99) {
        reset_prod_counters();
        busy = working_days;
        working_days = 0;
    }
    /* That's all. Cover is done by different functions every 3 months or so. */
    cricket_cost += CRICKET_RUNNING_COST;
    if(refresh_cover)
    {   cover();}
}

void Cricket::cover()
{
    if(covercount + COVER_TOLERANCE_DAYS < daycount)
    {
        daycount = 0;
        active = false;
        return;
    }
    active = true;
    covercount -= daycount;
    daycount = 0;
    animate_enable = true;
    for(int yy = ys; yy < ye; ++yy)
    {
        for(int xx = xs; xx < xe; ++xx)
        {   world(xx,yy)->flags |= FLAG_CRICKET_COVER;}
    }
}

void Cricket::animate() {
  int& frame = frameIt->frame;
  if(animate_enable && real_time >= anim) {
    anim = real_time + ANIM_THRESHOLD(CRICKET_ANIMATION_SPEED);
    if(++frame >= (int)frameIt->resourceGroup->graphicsInfoVector.size())
    {
      frame = 0;
      animate_enable = false;
    }
  }
}

void Cricket::report()
{
    int i = 0;
    const char* p;

    mps_store_sd(i++,constructionGroup->name, ID);
    mps_store_sfp(i++, N_("busy"), busy);
    // i++;
    list_commodities(&i);
    p = active?N_("Yes"):N_("No");
    mps_store_ss(i++, N_("Public sports"), p);
}

/** @file lincity/modules/cricket.cpp */
