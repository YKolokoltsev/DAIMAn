//
// Created by morrigan on 3/27/18.
//

#ifndef DAIMAN_MATH_ATOMIC_GROUP_H
#define DAIMAN_MATH_ATOMIC_GROUP_H

#include <string>
#include <list>

#include "base_vertex.hpp"
#include "math_property_map.hpp"
#include "wfndata.h"
#include "ref_decorator.hpp"

/*
 * Atomic groups are separate molecules in supramolecular complexes,
 * functional groups or in general - any atomic sets useful for analysis.
 * Each atomic group is basically a set of atoms that are already present
 * in the computational data. Each of it's atoms points onto the corresponding
 * atom in the original data, so the correspondence is exact.
 *
 * To create an atomic group the corresponding atoms can be selected
 * within "Atoms" brunch, in this case an "Add new group" menu item activates.
 *
 * Within any atomic group it is possible to specify a connection graph. If the
 * complete system molecular graph is present it is possible to automate a process.
 * However it seems that a possibility to manually modify this graph can be
 * desirable. A nice idea is to use only those critical points that belong
 * to the group completely. For example, we can select a cycle of 5 nearest
 * carbons on the fullerene surface. In this case each atom will have some
 * critical points that belong to the other atoms that do not belong to
 * the atoms that have been selected into the group. So looks like a simple
 * rule - omit all surrounding critical points.
 *
 * The group interconnection graph is useful for automatic comparison
 * of the atomic properties between more than one group, including
 * such comparison between different computational data sets (two
 * or more different *.wfx computations). So that two groups are comparable
 * only in the case when their interconnection graphs are the same,
 * and atoms are the same. It is important to have possibility to
 * detect all possible comparison configurations
 * in the case if any symmetry is present. Selection of the concrete
 * comparison configuration shell be implemented in the GUI and for the moment
 * is not clear. For example, it is possible to enumerate them, and
 * show in the same window two complete systems, highliting a group under
 * the interest. In this case by simply switching between enumerated
 * configurations it will be possible to achieve the optimal meaningful
 * comparison.
 *
 * //TODO:
 * For the moment it is decided just to print a plain property list
 * for a selected group. Say, all atomic properties and all BCPS that
 * belong to the group to have a possibility to make a manual analysis.
 *
 * Than - parse integral data, and just print it for a group. At least both steps
 * are required to move forward.
 */


/*
 * Atomic group is a GUI independent mathematical object -
 * a set of atoms.
 */
struct AtomicGroup : public ThreadSafeBaseObject, public MathBase{
public:
    AtomicGroup(node_desc_t wfn_idx, std::string sname) : sname{sname} {
        reg(this,false);
        wfn = std::move(get_shared_obj_ptr<WFNData>(this, wfn_idx));
    }

    virtual std::string origin(){return "User defined atomic group";};
    virtual std::string name(){return sname;}

private:
    std::string sname;
    std::list<int> atomic_list;

    ext_shared_ptr_t<WFNData> wfn;
};

#endif //DAIMAN_MATH_ATOMIC_GROUP_H
