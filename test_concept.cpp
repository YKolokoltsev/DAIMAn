#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include <list>
#include <array>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <critical_point.hpp>

//#include "critical_point.hpp"

using namespace boost::spirit;

//https://theboostcpplibraries.com/boost.spirit-grammar
//http://kiri11.ru/boost_spirit_qi_part2

typedef std::istreambuf_iterator<char> BaseIterator;
typedef multi_pass<BaseIterator> Iterator;

typedef std::list<std::list<std::string>> MgpBlocks;
struct MgpBlockGrammar : public qi::grammar<Iterator, MgpBlocks()>
{
    MgpBlockGrammar() : MgpBlockGrammar::base_type(start)
    {
        //ignore any number of blank symbols at line start
        trim = *(qi::blank - qi::eol);
        //read block (a list of text lines)
        block = +(trim >> +(qi::char_ - qi::eol) >> qi::eol);
        //parse blocks delimited by blank lines (a list of blocks)
        start  = +block % +(trim >> qi::eol);

        BOOST_SPIRIT_DEBUG_NODES((start));
    }

private:
    qi::rule<Iterator, MgpBlocks()> start;
    qi::rule<Iterator, std::list<std::string>()> block;
    qi::rule<Iterator> trim;
};

struct MgpAtoms {

    struct MgpAtom{
        std::string name;
        float charge;
        double x,y,z;
    };

    virtual bool check(const std::string& head){
        return qi::parse(head.begin(), head.end(), "Nuclear Charges and Cartesian Coordinates:");
    }

    virtual void parse(std::list<std::string>& lines){
        MgpAtom a;
        for(auto & l : lines) {
            if (qi::phrase_parse(l.begin(), l.end(),
                                 "" >> lexeme[+qi::alnum] >> float_ >>
                                         double_ >> double_ >> double_ >> "",
                                 qi::space, a.name, a.charge, a.x, a.y, a.z)) {
                atoms.push_back(a);
                std::cout << l << std::endl;
            }
        }
    }

    std::list<MgpAtom> atoms;
};

struct MgpCriticalPoints {

    struct CP{
        int n;
        std::array<double, 3> xyz;
        int l1, l2;
        std::string type_name;
        std::list<std::string> adjacent_atoms;

        CriticalPoint::CP_TYPE type(){
            if(qi::parse(type_name.begin(),type_name.end(),"NACP")){
                return CriticalPoint::NACP;
            }else if(qi::parse(type_name.begin(),type_name.end(),"NNACP")){
                return CriticalPoint::NNACP;
            }else if(qi::parse(type_name.begin(),type_name.end(),"BCP")){
                return CriticalPoint::BCP;
            }else if(qi::parse(type_name.begin(),type_name.end(),"RCP")){
                return CriticalPoint::RCP;
            }else if(qi::parse(type_name.begin(),type_name.end(),"CCP")){
                return CriticalPoint::CCP;
            }else{
                return CriticalPoint::UNDEF;
            }
        }
    };

    struct MGPCriticalPoint : public CriticalPoint{
        MGPCriticalPoint(const std::array<double, 3>& xyz, CP_TYPE type) : CriticalPoint(xyz, type) {};
        virtual std::string origin(){return "MGP file";}
    };

    virtual bool check(const std::string& head){
        return qi::parse(head.begin(), head.end(), "CP#");
    }

    virtual void parse(std::list<std::string>& lines){
        CP tcp;

        if(!qi::phrase_parse(lines.front().begin(), lines.front().end(),
                            "CP#" >> qi::int_ >> "Coords =" >> double_ >> double_ >> double_ >> "" ,
                            qi::space, tcp.n, tcp.xyz[0], tcp.xyz[1], tcp.xyz[2])) return;
        lines.pop_front();

        if(!qi::phrase_parse(lines.front().begin(), lines.front().end(),
                            "Type =" >> ("(" >> qi::int_) >> "," >> (qi::int_ >> ")") >>
                                     qi::lexeme[+qi::char_("A-Z")] >> *qi::lexeme[(+qi::char_("A-Z0-9"))] >> "",
                            qi::space, tcp.l1, tcp.l2, tcp.type_name, tcp.adjacent_atoms)) return;
        lines.pop_front();
        std::shared_ptr<MGPCriticalPoint> cp(new MGPCriticalPoint(tcp.xyz, tcp.type()));

        //fill properties (including tempraries)

        cps.push_back(cp);
    }

    std::list<std::shared_ptr<MGPCriticalPoint>> cps;
};

int main(){
    std::ifstream file("/home/morrigan/CLionProjects/DAIMAn/test/N4C2C2H.mgp", std::ifstream::in);

    Iterator fwd_begin = make_default_multi_pass(BaseIterator(file));
    Iterator fwd_end  = make_default_multi_pass(BaseIterator());

    MgpBlocks result;
    qi::parse(fwd_begin, fwd_end, MgpBlockGrammar(), result);

    MgpAtoms mgp_atoms;
    MgpCriticalPoints mgp_critical_points;

    for(auto& block : result){
        if(mgp_atoms.check(block.front())){
            mgp_atoms.parse(block);
        }else if(mgp_critical_points.check(block.front())){
            mgp_critical_points.parse(block);
        }
    }

    std::cout << "found " << mgp_critical_points.cps.size() << " critical points" << std::endl;
}
