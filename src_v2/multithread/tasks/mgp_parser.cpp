//
// Created by morrigan on 28/07/17.
//

#include <boost/spirit/include/qi.hpp>

#include "mgp_parser.h"
#include "algorithm.hpp"

using namespace boost::spirit;

MGPParserTask::MGPParserTask(
        std::shared_ptr<MGPParserParams> params,
        node_desc_t idx_thread_pool,
        node_desc_t idx_wfx) : BaseTask(idx_thread_pool){

    result.reset(new MGPParserResult(params));
    wfn = std::move(get_shared_obj_ptr<WFNData>(this, idx_wfx, DependencyType::HoldShared));
    file = std::shared_ptr<std::ifstream>(new std::ifstream, [](std::ifstream* p){
        p->close();
        delete p;
    });
    start();
}

void MGPParserTask::main_loop(){

    auto mgp_result = dynamic_pointer_cast<MGPParserResult>(result);

    try {
        file->open(get_result()->get_params()->path->c_str(), std::ifstream::in);
        if(!file->is_open()) throw runtime_error("error while opening file");

        Iterator fwd_begin = make_default_multi_pass(BaseIterator(*file));
        Iterator fwd_end = make_default_multi_pass(BaseIterator());

        MgpBlocks blocks;
        if(!qi::parse(fwd_begin, fwd_end, MgpBlockGrammar(), blocks))
            throw runtime_error("can't parse blocks");
        if(file->bad()) throw runtime_error("error while reading file");

        double progress = 0;
        for (auto &block : blocks) {
            if (mgp_result->atoms->check(block.front())) {
                mgp_result->atoms->parse(block);
            } else if (mgp_result->critical_points->check(block.front())) {
                mgp_result->critical_points->parse(block);
            }
            progress++;
            update_progress(progress / (double) blocks.size());
        }

        if(mgp_result->atoms->atoms.empty()) throw runtime_error("no atoms found");
        if(mgp_result->critical_points->cps.empty()) throw runtime_error("no critical points found");

        //build atomic map (parsed atom name VS wfx atom idx)
        std::map<std::string, int> atom_name_at_wfx;
        for(const auto& atom : mgp_result->atoms->atoms){
            std::pair<int, double> dist_idx{-1, 10};
            wfn->exec_r([&atom, &dist_idx](const WFNData* p_wfn){
                for(int i = 0; i < p_wfn->nat; i++){

                    double dx = p_wfn->atcoords[i][0] - atom.x;
                    double dy = p_wfn->atcoords[i][1] - atom.y;
                    double dz = p_wfn->atcoords[i][2] - atom.z;
                    double dist = sqrt(dx*dx + dy*dy + dz*dz);

                    if(dist < dist_idx.second && p_wfn->atcharges[i] == atom.charge){
                        dist_idx.first = i;
                        dist_idx.second = dist;
                    }
                }
            });

            if(dist_idx.second < 1e-4){
                atom_name_at_wfx.insert(std::pair<string,int>(atom.name, dist_idx.first));
            }else{
                throw runtime_error("mgp atom not found in wfx, wrong mgp?");
            }
        }
        if(atom_name_at_wfx.size() != mgp_result->atoms->atoms.size())
            throw runtime_error("mgp atomic names not unique");
        mgp_result->atoms.reset();

        //replace NeighAtNames (strings) by NeighAt (wfx integers)
        for(auto cp : mgp_result->critical_points->cps){
            //the NNACP shell not have neighbors
            if(cp->properties.check_prop<MgpCriticalPoints::NeighAtNames>()){
                auto at_names = cp->properties.get_prop<MgpCriticalPoints::NeighAtNames>();
                std::list<int> neigh_at;
                for(const string& at_name : at_names->val) {
                    auto it = atom_name_at_wfx.find(at_name);
                    if(it != atom_name_at_wfx.end()){
                        neigh_at.push_back(it->second);
                    }else{
                        auto mgp_cp_idx = cp->properties.get_prop<MgpCriticalPoints::MgpCPIdx>()->val;
                        auto cp_str_idx = std::to_string(mgp_cp_idx);
                        std::cerr << "WARNING: neighbor atom not found for #CP " + cp_str_idx + ", atom name is: " + at_name << std::endl;
                    }
                }
                cp->properties.add_prop(MgpCriticalPoints::NeighAt(neigh_at, get_result()->get_params()->path));
                cp->properties.del_prop<MgpCriticalPoints::NeighAtNames>();
                neigh_at.clear();
            }
        }

        result->res_code = TaskResult::RES_OK;

    }catch(runtime_error& e){
        cout << "About to discard results" << endl;
        std::cerr << e.what() << std::endl;
        result->discard(e.what());
    }
}

void MGPParserResult::add_to_document(const BaseTask* task){
    if(res_code == RES_OK) {
        try {
            auto dt_lock = DocTree::inst();
            auto idx_wfx = doc_first_nearest_father<WFNData>(task->get_idx());
            for(auto & cp : critical_points->cps) {
                cp->append_doc(idx_wfx, get_params()->time_id);
            }
            critical_points.reset();
        } catch (runtime_error &e) {
            std::cerr << e.what() << std::endl;
            discard(e.what());
        }
    }

    TaskResult::add_to_document(task);
};