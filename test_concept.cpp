#include <iostream>
#include <string>
#include <cctype>

#include <boost/spirit/include/qi.hpp>
#include <string>
#include <sstream>
#include <fstream>
#include <memory>
#include <list>

using namespace boost::spirit::qi;


struct BLOCK{
    enum TextBlock{NONE, ATOMS, CP};

    BLOCK() : blk{NONE} {}
    BLOCK(TextBlock blk) : blk{blk} {};

    bool check_block(TextBlock blk){return this->blk == blk;};

    virtual TextBlock parse_line(const std::string& s){
        if(phrase_parse(s.begin(), s.end(),
                       "Nuclear Charges and Cartesian Coordinates:", space)){
            return ATOMS;
        }else if(phrase_parse(s.begin(), s.end(), "CP# ", space)){
            return CP;
        }else{
            return NONE; //this is also to get out from any current block
        }
    }

    const TextBlock blk;

    bool isblank_str(const std::string& s){
        auto cnt = std::count_if(s.begin(), s.end(), [](unsigned char c){
            return std::isspace(c);
        });
        return cnt == s.length();
    }

    virtual void report(){}
};

struct ATOMS_BLOCK : public BLOCK{
    ATOMS_BLOCK() : BLOCK(ATOMS) {};

    struct AtomicData{
        std::string atom_name;
        float atom_charge;
        double x,y,z;
        void print(){
            std::cout << atom_name << " " << atom_charge
                      << " " << x << " " << y << " " << z << std::endl;
        }
    };

    virtual TextBlock parse_line(const std::string& line){
        if(isblank_str(line)) return NONE;

        AtomicData a;
        if(phrase_parse(line.begin(), line.end(),
                        "" >> lexeme[+alnum] >> float_
                           >> double_ >> double_ >> double_ >> "" ,
                        space, a.atom_name, a.atom_charge, a.x, a.y, a.z)){
            atoms.push_back(a);
        }
        return ATOMS;
    }

    virtual void report(){
        std::cout << "total atoms: " << atoms.size() << std::endl;
    }

    std::list<AtomicData> atoms;
};

struct CP_BLOCK : public BLOCK{
    CP_BLOCK() : BLOCK(CP) {};

    virtual TextBlock parse_line(const std::string& line){
        if(isblank_str(line)) return NONE;

        int n = 0;
        double x,y,z;
        if(phrase_parse(line.begin(), line.end(),
                        "CP#" >> int_ >> "Coords =" >> double_ >> double_ >> double_ >> "" ,
                        space, n, x, y, z)){

            std::cout << n << " >>: " << line << std::endl;
        }
        return CP;
    }

};

int main(){
    std::ifstream file("/home/morrigan/CLionProjects/DAIMAn/test/N4C2C2H.mgp");
    std::string line;

    using t_block_ptr = std::shared_ptr<BLOCK>;

    std::list<t_block_ptr> blocks;
    blocks.push_back(t_block_ptr(new BLOCK()));

    auto atoms = t_block_ptr(new ATOMS_BLOCK());
    blocks.push_back(atoms);

    auto cps = t_block_ptr(new CP_BLOCK());
    blocks.push_back(cps);

    BLOCK::TextBlock curr_block = BLOCK::NONE;

    while( std::getline(file,line)){
        reiterate_line: //block has changed, parse same line within new block
        for(auto& x : blocks){
            if(x->check_block(curr_block)) {
                auto new_block = x->parse_line(line);
                if(curr_block != new_block){
                    curr_block = new_block;
                    goto reiterate_line;
                }
            }
        }
    }

    for(auto& x : blocks){
        x->report();
    }

    std::cout << "exit" << std::endl;
}
