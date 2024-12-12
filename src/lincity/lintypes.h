/* ---------------------------------------------------------------------- *
 * src/lincity/lintypes.h
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 1995-1997 I J Peters
 * Copyright (C) 1997-2005 Greg Sharp
 * Copyright (C) 2000-2004 Corey Keasling
 * Copyright (C) 2022-2024 David Bears <dbear4q@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
** ---------------------------------------------------------------------- */

#ifndef __lintypes_h__
#define __lintypes_h__

#include <array>            // for array
#include <cstring>          // for NULL
#include <iostream>         // for basic_ostream, operator<<, cout, endl
#include <list>             // for list
#include <map>              // for map
#include <string>           // for char_traits, string, basic_string, operat...
#include <vector>           // for vector

#include "commodities.hpp"  // for Commodity, CommodityRule, operator++
#include "resources.hpp"    // for ResourceGroup, ExtraFrame (ptr only)

class ConstructionGroup;

namespace xmlpp {
class TextReader;
}  // namespace xmlpp
typedef struct _xmlTextWriter *xmlTextWriterPtr;

#define OLD_MAX_NUMOF_SUBSTATIONS 100
#define MAX_NUMOF_SUBSTATIONS 512

#define NUMOF_COAL_RESERVES ((world.len() * world.len()) / 400)

#define OLD_MAX_NUMOF_MARKETS 100
#define MAX_NUMOF_MARKETS 512

#define NUMOF_DAYS_IN_MONTH 100
#define NUMOF_DAYS_IN_YEAR (NUMOF_DAYS_IN_MONTH*12)

unsigned short get_group_of_type(unsigned short selected_type);

class Construction {
public:
  Construction();
  virtual ~Construction() {}

  virtual void update() = 0;
  virtual void report() = 0;
  virtual void animate() {}

  /**
   * Initializes a new construction (i.e. one that is not loaded from a save
   * file) before placement on the map.
   *
   * Note: Overloads are required to forward the call the base-class method.
  **/
  virtual void initialize() {}

  void load(xmlpp::TextReader& xmlReader);

protected:
  /**
   * Loads a member from a save file for rebuilding an existing construction.
   *
   * xmlReader: an xmlReader positioned at the element of the member to load.
   *
   * Returns true iff the member is recognized (by the current class or a
   * base class).
   *
   * Note: If the member name (i.e. the XML element name) is unrecognized by a
   * derived class, then the call must be forwarded to each direct base class
   * until it is recognized.
  **/
  virtual bool loadMember(xmlpp::TextReader& xmlReader);

public:
  /**
   * Called when this construction is placed onto the map and before any
   * simulation or animation steps to initialize derived (i.e. based on loaded
   * members) and/or position-dependent state.
   *
   * At the time this method is called, this construction may or may not
   * actually be placed on the map or registered for simulation/animation. But
   * if it is not yet placed or registered, then it will be placed and
   * registered at the specified x/y coordinate before the next
   * simulation/animation step and before `place` is called on any other
   * Construction.
   *
   * Note: Overloads are required to forward the call the base-class method.
  **/
  virtual void place(int x, int y);

  /**
   * Saves all persistent, non-derived members as XML elements to the provided
   * xmlWriter.
   *
   * Note: In general, XML elements, along with their attributes, are
   * caller-owned. A callee is expected to populate an element with child
   * elements. For example, a `World` may own a `Construction`, so
   * `World::save` may create a `<construction>` XML start element, add some
   * attributes to it, like x/y position, and then call `Construction::save`
   * to populate the `<construction>` element with child elements.
   * `Construction::save` is NOT the owner of the `<construction>` element and
   * should neither create it nor add attributes to it.
  **/
  virtual void save(xmlTextWriterPtr xmlWriter);



  ConstructionGroup *constructionGroup;
  ResourceGroup *soundGroup;

  int x, y;
  int flags;              //flags are defined in lin-city.h

  // std::map<Commodities, int> commodityCount;  //map that holds all kinds of stuff
  std::array<int, STUFF_COUNT> commodityCount;    // inventory
  std::array<int, STUFF_COUNT> commodityProd;     // production month-to-date
  std::array<int, STUFF_COUNT> commodityProdPrev; // production last month
  std::array<int, STUFF_COUNT> commodityMaxProd;  // max production
  std::array<int, STUFF_COUNT> commodityMaxCons;  // max consumption
  std::vector<Construction*> neighbors;       //adjacent for transport
  std::vector<Construction*> partners;        //remotely for markets
  std::list<ExtraFrame>::iterator frameIt;
  static std::string getStuffName(Commodity stuff_id); //translated name of a commodity
  virtual void init_resources(void);              //sets sounds and graphics according to constructionGroup
  void list_commodities(int *);                   //prints a sorted list all commodities in report()
  void list_inventory(int *);                     // prints list of commodity inventory in report()
  void list_production(int *);                    // prints list of commodity production in report()
  void reset_prod_counters(void);                 // monthly update to production counters
  int produceStuff(Commodity stuff_id, int amt);  // increases inventory by amt and updates production counter
  int consumeStuff(Commodity stuff_id, int amt);  // decreases inventory by amt and updates production counter
  int levelStuff(Commodity stuff_id, int amt);    // sets inventory level and updates production counter
  void report_commodities(void);                  //adds commodities and capacities to gloabl stat counter
  void initialize_commodities(void);              //sets all commodities to 0 and marks them as saved members
  void bootstrap_commodities(int percentage);     // sets all commodities except STUFF_WASTE to percentage.

  void detach();      //removes all references from world, ::constructionCount
  void deneighborize(); //cancells all neighbors and partners mutually
  void   neighborize(); //adds all neigborconnections once (call twice for double connections)
  int countPowercables(int mask); //removes all but one suspended cable on each edge
  void link_to(Construction* other); //establishes mutual connection to neighbor or partner
  int  tellstuff( Commodity stuff_ID, int level); //tell the filling level of commodity
  void trade(); //exchange commodities with neigbhors
  int equilibrate_stuff(int *rem_lvl, CommodityRule rem_rule , int ratio, Commodity stuff_ID);
  //equilibrates stuff with an external reservoir (e.g. another construction invoking this method)
  void playSound();//plays random chunk from constructionGroup
};

//global Vars for statistics on commodities
extern std::map<Commodity, int> tstat_capacities;
extern std::map<Commodity, int> tstat_census;

class ConstructionGroup {
public:
    ConstructionGroup(
        const char *name,
        bool no_credit,
        unsigned short group,
        unsigned short size, int colour,
        int cost_mul, int bul_cost, int fire_chance,
        int cost, int tech, int range, int mps_pages
    ) {
        this->name = name;
        this->no_credit = no_credit;
        this->group = group;
        this->size = size;
        this->colour = colour;
        this->cost_mul = cost_mul;
        this->bul_cost = bul_cost;
        this->fire_chance = fire_chance;
        this->cost = cost;
        this->tech = tech;
        this->range = range;
        this->mps_pages = mps_pages;
        this->count = 0;

        for(Commodity stuff = STUFF_INIT; stuff < STUFF_COUNT; stuff++) {
          this->commodityRuleCount[stuff] = (CommodityRule){
            .maxload = 0,
            .take = false,
            .give = false
          };
        }
    }
    virtual ~ConstructionGroup() {}

    std::array<CommodityRule, STUFF_COUNT> commodityRuleCount;
    int getCosts();
    bool is_allowed_here(int x, int y, bool msg);//check if construction could be placed

    virtual int placeItem(int x, int y);

    // this method must be overriden by the concrete ConstructionGroup classes.
    virtual Construction *createConstruction() = 0;

    std::string getName(void);

    std::string resourceID;           /* name for matching resources from XML*/
    const char *name;           /* inGame name of group */
    bool no_credit;   /* TRUE if need credit to build */
    unsigned short group;       /* This is redundant: it must match
                                   the index into the table */
    unsigned short size;        /* shape in x and y */
    int colour;                 /* summary map colour */
    int cost_mul;               /* group cost multiplier */
    int bul_cost;               /* group bulldoze cost */
    int fire_chance;            /* probability of fire */
    int cost;                   /* group cost */
    int tech;                   /* group tech */
    int range;                  /* range beyond size*/
    int mps_pages;
    int count;

    static void addConstructionGroup(ConstructionGroup *constructionGroup)
    {
        if ( groupMap.count(constructionGroup->group) )
        {
            std::cout << "rejecting " << constructionGroup->name << " as "
            << constructionGroup->group << " from ConstructionGroup::groupMap"
            << std::endl;
        }
        else
        {   groupMap[constructionGroup->group] = constructionGroup;}


    }

    static void addResourceID(std::string resID, ConstructionGroup *constructionGroup)
    {

        if ( resourceMap.count(resID))
        {
            std::cout << "rejecting " << constructionGroup->name << " as "
            << constructionGroup->resourceID << " from ConstructionGroup::resouceMap"
            << std::endl;
        }
        else
        {
            constructionGroup->resourceID = resID;
            resourceMap[constructionGroup->resourceID] = constructionGroup;
            new ResourceGroup(resID); //adds itself to ResourceGroup::resmap
        }
    }

    static void clearGroupMap()
    {
        // removes all entries from groupMap
        groupMap.clear();
    }

    static void clearResourcepMap()
    {
        // removes all entries from resourceMap
        resourceMap.clear();
    }


    static ConstructionGroup *getConstructionGroup(unsigned short group)
    {
        if (groupMap.count(group))
            return groupMap[group];
        else
            return NULL;
    }

    static void printGroups();

    static int countConstructionGroup(unsigned short group) {
        return groupMap.count(group);
    }

    static std::map<std::string, ConstructionGroup*> resourceMap;
protected:
    // Map associating group ids with the respective construction group objects
    static std::map<unsigned short, ConstructionGroup*> groupMap;

};

#endif /* __lintypes_h__ */

/** @file lincity/lintypes.h */
