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

    MgpCriticalPoints(std::shared_ptr<string> path) : path{path} {}

    typedef const std::shared_ptr<string> t_path;

    //===================================================================
    struct MGPCriticalPoint : public CriticalPoint{
        MGPCriticalPoint(const std::array<double, 3>& xyz, CP_TYPE type, t_path path) :
                CriticalPoint(xyz, type), path{path} {};
        virtual std::string origin() {return *path;}
    private:
        t_path path;
    };

    struct Rho : public PointPropertyMap::Rho{

        Rho(valtype val, t_path path) : PointPropertyMap::Rho(val), path{path} {}
        static std::shared_ptr<valtype> parse(const std::string& str){
            return parse_double(str, "Rho =");
        };
        virtual std::string origin() {return *path;}
    private:
        t_path path;
    };

    struct GradRho : public PointPropertyMap::GradRho{
        GradRho(valtype val, t_path path) : PointPropertyMap::GradRho(val), path{path} {}
        static std::shared_ptr<valtype> parse(const std::string& str){
            return parse_double_3arr(str, "GradRho =");
        };
        virtual std::string origin() {return *path;}
    private:
        t_path path;
    };

    struct HessRhoEigVals : public PointPropertyMap::HessRhoEigVals{
        HessRhoEigVals(valtype val, t_path path) : PointPropertyMap::HessRhoEigVals(val), path{path} {}
        virtual std::string origin() {return *path;}
        static std::shared_ptr<valtype> parse(const std::string& str){
            return parse_double_3arr(str, "HessRho_EigVals =");
        };
    private:
        t_path path;
    };

    struct HessRhoEigVec1 : public PointPropertyMap::HessRhoEigVec1{
        HessRhoEigVec1(valtype val, t_path path) : PointPropertyMap::HessRhoEigVec1(val), path{path} {}
        virtual std::string origin() {return *path;}
        static std::shared_ptr<valtype> parse(const std::string& str){
            return parse_double_3arr(str, "HessRho_EigVec1 =");
        };
    private:
        t_path path;
    };

    struct HessRhoEigVec2 : public PointPropertyMap::HessRhoEigVec2{
        HessRhoEigVec2(valtype val, t_path path) : PointPropertyMap::HessRhoEigVec2(val), path{path} {}
        virtual std::string origin() {return *path;}
        static std::shared_ptr<valtype> parse(const std::string& str){
            return parse_double_3arr(str, "HessRho_EigVec2 =");
        };
    private:
        t_path path;
    };

    struct HessRhoEigVec3 : public PointPropertyMap::HessRhoEigVec3{
        HessRhoEigVec3(valtype val, t_path path) : PointPropertyMap::HessRhoEigVec3(val), path{path} {}
        virtual std::string origin() {return *path;}
        static std::shared_ptr<valtype> parse(const std::string& str){
            return parse_double_3arr(str, "HessRho_EigVec3 =");
        };
    private:
        t_path path;
    };

    struct DelSqRho : public PointPropertyMap::DelSqRho{
        DelSqRho(valtype val, t_path path) : PointPropertyMap::DelSqRho(val), path{path} {}
        static std::shared_ptr<valtype> parse(const std::string& str){
            return parse_double(str, "DelSqRho =");
        };
        virtual std::string origin() {return *path;}
    private:
        t_path path;
    };

    struct V : public PointPropertyMap::V{
        V(valtype val, t_path path) : PointPropertyMap::V(val), path{path} {}
        static std::shared_ptr<valtype> parse(const std::string& str){
            return parse_double(str, "V =");
        };
        virtual std::string origin() {return *path;}
    private:
        t_path path;
    };

    struct G : public PointPropertyMap::G{
        G(valtype val, t_path path) : PointPropertyMap::G(val), path{path} {}
        static std::shared_ptr<valtype> parse(const std::string& str){
            return parse_double(str, "G =");
        };
        virtual std::string origin() {return *path;}
    private:
        t_path path;
    };

    struct K : public PointPropertyMap::K{
        K(valtype val, t_path path) : PointPropertyMap::K(val), path{path} {}
        static std::shared_ptr<valtype> parse(const std::string& str){
            return parse_double(str, "K =");
        };
        virtual std::string origin() {return *path;}
    private:
        t_path path;
    };

    struct L : public PointPropertyMap::L{
        L(valtype val, t_path path) : PointPropertyMap::L(val), path{path} {}
        static std::shared_ptr<valtype> parse(const std::string& str){
            return parse_double(str, "L =");
        };
        virtual std::string origin() {return *path;}
    private:
        t_path path;
    };

    struct DelSqV : public PointPropertyMap::DelSqV{
        DelSqV(valtype val, t_path path) : PointPropertyMap::DelSqV(val), path{path} {}
        static std::shared_ptr<valtype> parse(const std::string& str){
            return parse_double(str, "DelSqV =");
        };
        virtual std::string origin() {return *path;}
    private:
        t_path path;
    };

    struct DelSqVen : public PointPropertyMap::DelSqVen{
        DelSqVen(valtype val, t_path path) : PointPropertyMap::DelSqVen(val), path{path} {}
        static std::shared_ptr<valtype> parse(const std::string& str){
            return parse_double(str, "DelSqVen =");
        };
        virtual std::string origin() {return *path;}
    private:
        t_path path;
    };

    struct DelSqVrep : public PointPropertyMap::DelSqVrep{
        DelSqVrep(valtype val, t_path path) : PointPropertyMap::DelSqVrep(val), path{path} {}
        static std::shared_ptr<valtype> parse(const std::string& str){
            return parse_double(str, "DelSqVrep =");
        };
        virtual std::string origin() {return *path;}
    private:
        t_path path;
    };

    struct DelSqG : public PointPropertyMap::DelSqG{
        DelSqG(valtype val, t_path path) : PointPropertyMap::DelSqG(val), path{path} {}
        static std::shared_ptr<valtype> parse(const std::string& str){
            return parse_double(str, "DelSqG =");
        };
        virtual std::string origin() {return *path;}
    private:
        t_path path;
    };

    struct DelSqK : public PointPropertyMap::DelSqK{
        DelSqK(valtype val, t_path path) : PointPropertyMap::DelSqK(val), path{path} {}
        static std::shared_ptr<valtype> parse(const std::string& str){
            return parse_double(str, "DelSqK =");
        };
        virtual std::string origin() {return *path;}
    private:
        t_path path;
    };

    //===================================================================

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
                std::cerr << "critical point type undefined" << std::endl;
                return CriticalPoint::UNDEF;
            }
        }
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
        std::shared_ptr<MGPCriticalPoint> cp(new MGPCriticalPoint(tcp.xyz, tcp.type(), path));

        while(!lines.empty()){
            std::string& l = lines.front();
            if(Rho::parse(l)){
                cp->properties.add_prop(Rho(*Rho::parse(l), path));
                std::cout << "Rho ok: " << cp->properties.get_prop<Rho>()->val << std::endl;
            }else if(GradRho::parse(l)){
                cp->properties.add_prop(GradRho(*GradRho::parse(l), path));
                std::cout << "GradRho ok: " << cp->properties.get_prop<GradRho>()->val[0] << "..." << std::endl;
            }else if(HessRhoEigVals::parse(l)){
                cp->properties.add_prop(HessRhoEigVals(*HessRhoEigVals::parse(l), path));
                std::cout << "HessRhoEigVals ok: " << cp->properties.get_prop<HessRhoEigVals>()->val[0] << "..." << std::endl;
            }else if(HessRhoEigVec1::parse(l)){
                cp->properties.add_prop(HessRhoEigVec1(*HessRhoEigVec1::parse(l), path));
                std::cout << "HessRhoEigVec1 ok: " << cp->properties.get_prop<HessRhoEigVec1>()->val[0] << "..." << std::endl;
            }else if(HessRhoEigVec2::parse(l)){
                cp->properties.add_prop(HessRhoEigVec2(*HessRhoEigVec2::parse(l), path));
                std::cout << "HessRhoEigVec2 ok: " << cp->properties.get_prop<HessRhoEigVec2>()->val[0] << "..." << std::endl;
            }else if(HessRhoEigVec3::parse(l)){
                cp->properties.add_prop(HessRhoEigVec3(*HessRhoEigVec3::parse(l), path));
                std::cout << "HessRhoEigVec3 ok: " << cp->properties.get_prop<HessRhoEigVec3>()->val[0] << "..." << std::endl;
            }else if(DelSqRho::parse(l)){
                cp->properties.add_prop(DelSqRho(*DelSqRho::parse(l), path));
                std::cout << "DelSqRho ok: " << cp->properties.get_prop<DelSqRho>()->val << std::endl;
            }else if(V::parse(l)){
                cp->properties.add_prop(V(*V::parse(l), path));
                std::cout << "V ok: " << cp->properties.get_prop<V>()->val << std::endl;
            }else if(G::parse(l)){
                cp->properties.add_prop(G(*G::parse(l), path));
                std::cout << "G ok: " << cp->properties.get_prop<G>()->val << std::endl;
            }else if(K::parse(l)){
                cp->properties.add_prop(K(*K::parse(l), path));
                std::cout << "K ok: " << cp->properties.get_prop<K>()->val << std::endl;
            }else if(L::parse(l)){
                cp->properties.add_prop(L(*L::parse(l), path));
                std::cout << "L ok: " << cp->properties.get_prop<L>()->val << std::endl;
            }else if(DelSqV::parse(l)){
                cp->properties.add_prop(DelSqV(*DelSqV::parse(l), path));
                std::cout << "DelSqV ok: " << cp->properties.get_prop<DelSqV>()->val << std::endl;
            }else if(DelSqVen::parse(l)){
                cp->properties.add_prop(DelSqVen(*DelSqVen::parse(l), path));
                std::cout << "DelSqVen ok: " << cp->properties.get_prop<DelSqVen>()->val << std::endl;
            }else if(DelSqVrep::parse(l)){
                cp->properties.add_prop(DelSqVrep(*DelSqVrep::parse(l), path));
                std::cout << "DelSqVrep ok: " << cp->properties.get_prop<DelSqVrep>()->val << std::endl;
            }else if(DelSqG::parse(l)){
                cp->properties.add_prop(DelSqG(*DelSqG::parse(l), path));
                std::cout << "DelSqG ok: " << cp->properties.get_prop<DelSqG>()->val << std::endl;
            }else if(DelSqK::parse(l)){
                cp->properties.add_prop(DelSqK(*DelSqK::parse(l), path));
                std::cout << "DelSqK ok: " << cp->properties.get_prop<DelSqK>()->val << std::endl;
            }
            lines.pop_front();
        }

        cps.push_back(cp);
    }

    std::list<std::shared_ptr<MGPCriticalPoint>> cps;

private:
    static std::shared_ptr<double> parse_double(const std::string& str, const std::string& prefix){
        double val;
        bool is_ok = qi::phrase_parse(str.begin(), str.end(),
                                      prefix.c_str() >> qi::double_ ,
                                      qi::space, val);
        return is_ok ? std::shared_ptr<double>(new double(val)) : nullptr;
    };

    static std::shared_ptr<std::array<double,3>> parse_double_3arr(const std::string& str, const std::string& prefix){
        std::array<double,3> val;
        bool is_ok = qi::phrase_parse(str.begin(), str.end(),
                                      prefix.c_str() >> qi::double_ >> qi::double_ >> qi::double_ ,
                                      qi::space, val[0], val[1], val[2]);
        return is_ok ? std::shared_ptr<std::array<double,3>>(new std::array<double,3>(val)) : nullptr;
    };

private:
    const std::shared_ptr<string> path;
};

int main(){

    std::shared_ptr<string> path(new std::string("/home/morrigan/CLionProjects/DAIMAn/test/N4C2C2H.mgp"));
    std::ifstream file(*path, std::ifstream::in);

    Iterator fwd_begin = make_default_multi_pass(BaseIterator(file));
    Iterator fwd_end  = make_default_multi_pass(BaseIterator());

    MgpBlocks result;
    qi::parse(fwd_begin, fwd_end, MgpBlockGrammar(), result);

    MgpAtoms mgp_atoms;
    MgpCriticalPoints mgp_critical_points(path);

    for(auto& block : result){
        if(mgp_atoms.check(block.front())){
            mgp_atoms.parse(block);
        }else if(mgp_critical_points.check(block.front())){
            mgp_critical_points.parse(block);
        }
    }

    std::cout << "found " << mgp_critical_points.cps.size() << " critical points" << std::endl;
}
