//
// Created by morrigan on 28/07/17.
//

#include <boost/spirit/include/qi.hpp>

#include "mgp_parser.h"

using namespace boost::spirit;

//todo: check for all parsers that it = end

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
}

void MGPParserTask::main_loop(){

    //todo: file exception!
    file->open(get_result()->get_params()->path);
    auto mgp_result = dynamic_pointer_cast<MGPParserResult>(result);
    std::string line;

    auto blocks = mgp_result->get_blocks();
    blocks.push_back(std::shared_ptr<MGPBlock>(new MGPBlock()));

    MGPBlock::LogicalBlock curr_block = MGPBlock::NONE;

    while(std::getline(*file,line)){
        reiterate_line: //block has changed, parse same line within new block
        for(auto& x : blocks){
            if(x->is_current(curr_block)) {
                auto new_block = x->parse_line(line);
                if(curr_block != new_block){
                    curr_block = new_block;
                    goto reiterate_line;
                }
            }
        }
    }

    mgp_result->discard("mgp not implemented yet");
    std::cout << "about to exit from MGP thread" << std::endl;
}

//===================MGPBlock (BLANK) ================

MGPBlock::LogicalBlock MGPBlock::parse_line(const std::string& s){
    if(qi::phrase_parse(s.begin(), s.end(),
                        "Nuclear Charges and Cartesian Coordinates:", qi::space)){
        return ATOMS;
    }else if(qi::phrase_parse(s.begin(), s.end(),
                              "CP# ", qi::space)){
        return CP;
    }else{
        return NONE; //also used to exit from any current block
    }
}

bool MGPBlock::isblank_str(const std::string& s){
    auto cnt = std::count_if(s.begin(), s.end(), [](unsigned char c){
        return std::isspace(c);
    });
    return cnt == s.length();
}

//===================AtomsMGPBlock (Atomic coords & descriptors) ================

MGPBlock::LogicalBlock AtomsMGPBlock::parse_line(const std::string& line){
    if(isblank_str(line)) return NONE;

    AtomicData a;
    if(qi::phrase_parse(line.begin(), line.end(),
      "" >> lexeme[+qi::alnum] >> float_
         >> double_ >> double_ >> double_ >> "" ,
      qi::space, a.atom_name, a.atom_charge, a.x, a.y, a.z)){
        atoms.push_back(a);
        std::cout << line << std::endl;
    }

    return ATOMS;
}

//===================CriticalPointsMGPBlock (List of all critical points) ======

MGPBlock::LogicalBlock  CriticalPointsMGPBlock::parse_line(const std::string& line) {
    if(isblank_str(line)){
        x = 0; y = 0; z = 0;
        return NONE;
    }

    int n = 0;
    std::string w1, w2;
    std::vector<std::string> words;
    int a, b;
    if(qi::phrase_parse(line.begin(), line.end(),
                        "CP#" >> qi::int_ >> "Coords =" >> double_ >> double_ >> double_ >> "" ,
                        qi::space, n, x, y, z)){
        //critical_points.push_back(CriticalPoint())
        //std::cout << n << " >>: " << line << std::endl;
    }else if(qi::phrase_parse(line.begin(), line.end(),
                              "Type =" >> ("(" >> qi::int_) >> "," >> (qi::int_ >> ")") >>
                                       qi::lexeme[+qi::char_("A-Z")] >> *qi::lexeme[(+qi::char_("A-Z0-9"))] >> "",
                              qi::space, a, b, w1, words)){
        std::cout << line << std::endl;
        std::cout << "detected: " << a << " " << b << " " << w1 << " ";
        for(auto & w : words) std::cout << w << " ";
        std::cout << std::endl;
    }
    return CP;
}