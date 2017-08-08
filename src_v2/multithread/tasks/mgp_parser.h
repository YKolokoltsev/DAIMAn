//
// Created by morrigan on 28/07/17.
//

#ifndef DAIMAN_MGPPARSERTASK_H
#define DAIMAN_MGPPARSERTASK_H

#include <string>
#include <fstream>

#include "base_task.h"
#include "wfndata.h"

/*
 * In AIMAll mgp file the data is logically devided into the blocks
 * (this is common for many formats in chemistry). This class by parsing
 * any line of such txt document defines if we entered inside of any
 * logical block (these are define in the LogicalBlock enum).
 */
struct MGPBlock{
    enum LogicalBlock{NONE, ATOMS, CP};

    MGPBlock() : blk{NONE} {}
    MGPBlock(LogicalBlock blk) : blk{blk} {};

    bool is_current(LogicalBlock blk){return this->blk == blk;};
    virtual LogicalBlock parse_line(const std::string& s);
    virtual void log(){};

protected:
    bool isblank_str(const std::string& s);

private:
    const LogicalBlock blk;
};

//-----------------------------------------------------
/*
 * List of atoms and it's cartesian coordinates. On the one hand,
 * this list have to coinside with the atoms in WFX, on the other -
 * the atomic names (atom_name) is an atom descriptor in the MGP file,
 * each atom is referred by it's atom_name inside of critical points.
 */
struct AtomsMGPBlock : public MGPBlock{
    AtomsMGPBlock() : MGPBlock(ATOMS) {};

    struct AtomicData{
        std::string atom_name;
        float atom_charge;
        double x,y,z;
    };

    virtual LogicalBlock parse_line(const std::string& s);
    std::list<AtomicData> atoms;
};

//-----------------------------------------------------
/*
 * List of CriticalPoints with their attribute maps attached.
 * This is the main parser of MGP file the main parser.
 */

struct CriticalPointsMGPBlock : public MGPBlock{
    CriticalPointsMGPBlock() : MGPBlock(CP) {};

    virtual LogicalBlock parse_line(const std::string& s);

    //std::list<CriticalPoint> critical_points;

private:
    double x,y,z; //current coords
};

struct MGPParserParams : public TaskParams{
    std::string path;
};

struct MGPParserResult : TaskResult{
    MGPParserResult(std::shared_ptr<MGPParserParams> params) :
            TaskResult(params) {
        atoms = std::shared_ptr<AtomsMGPBlock>(new AtomsMGPBlock);
        critical_points = std::shared_ptr<CriticalPointsMGPBlock>(new CriticalPointsMGPBlock);
    }

    std::shared_ptr<const MGPParserParams> get_params() const {
        return dynamic_pointer_cast<const MGPParserParams>(params);
    }

    std::list<std::shared_ptr<MGPBlock>> get_blocks(){
        std::list<std::shared_ptr<MGPBlock>> blocks;
        blocks.push_back(atoms);
        blocks.push_back(critical_points);
        return blocks;
    }

private:
    std::shared_ptr<AtomsMGPBlock> atoms;
    std::shared_ptr<CriticalPointsMGPBlock> critical_points;
};

class MGPParserTask : public BaseTask {
public:
    MGPParserTask(std::shared_ptr<MGPParserParams> params,
                  node_desc_t idx_thread_pool,
                  node_desc_t idx_wfx);

    std::shared_ptr<const MGPParserResult> get_result() const {
        return dynamic_pointer_cast<const MGPParserResult>(result);
    }

protected:
    virtual void main_loop();

private:
    ext_shared_ptr_t<WFNData> wfn;
    std::shared_ptr<std::ifstream> file;
};


#endif //DAIMAN_MGPPARSERTASK_H
