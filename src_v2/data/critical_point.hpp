/*
 * Author: Dr. Yevgeniy Kolokoltsev
 * Created: 12 Jun 2017
 *
 * Remarks: This code shell be compilable without any additional
 * libraries (for portability reasons), so we use a pure C++11 standard
 */

#ifndef DAIMAN_CRITICALPOINT_H
#define DAIMAN_CRITICALPOINT_H

#include <iostream>
#include <vector>
#include <typeindex>
#include <map>
#include <memory>

using namespace std;

/*
 * todo: separate CP defined by it's coords and type from it's property map
 * todo: the reasom is that all other properties depend strictly on the CP
 * todo: coords, and each CP has a strict type. If coords are changed somehow
 * todo: all properties shell be invalidated. On the other hand we can
 * todo: compute different properties in a different ways, so they may have
 * todo: disctinct origins for the same CP.
 *
 * At any critical point it is possible to calculate different properties
 * by different methods. These properties are not needed permanently,
 * also their number is unknown at the moment.
 *
 * It was decided here to keep only those properties that was calculated at runtime.
 * In a simplest case, there are just one data that is obligatory - it is a type of a CP.
 * //todo: may be coords are better???
 * It becomes known when a CP is found and it is constant. All the other properties
 * are maintained in a dynamical memory that is managed automatically with the help
 * of shared_ptr container (is deleted automatically with the last container object
 * even after it was removed from the object of this class).
 *
 * Each property has it's own data type. Therefore for a uniform access to the pointers
 * it is necessary to have a common minimal base class, CP_BASE_PROP. This class also
 * defines a requirement that each property type implements it's own "origin" function,
 * to keep track of ANY possible origins.
 *
 * In the case when an origin is static (say calculated by a well defined method without
 * any additional parameters), in a pure dynamic case there will be a lots of redundant
 * information stored (for example - same origin for 300 CPs). However, it is possible to
 * put this and the other info into a property type, so that store such additional
 * information inside a dynamic property object now is not necessary. This saves space,
 * the origin data is stored just once in the implementation of its vtable = an 'origin'
 * function.
 *
 * In the case when origin info is parametrized, but remains the same for a set of
 * computations, it is possible to store the info in a shared_ptr,
 * and share this same pointer among all CPs with the same origin property. Of course
 * this is just like a pure dynamic case, without advantages, however this is not all
 * benefits of the selected templete-based strategy.
 *
 * If we inherit property types one from another, there is just a little work left
 * (thanks to dynamic_pointer_cast), to manage a "castability" tree!
 *
 * It is possible to define a NOT VIRTUAL "compare" function in a base class,
 * so that different computations be compared in a transparent manner.
 *
 * The next interface extension is defined in a template CP_PROP. This template defines
 * a property value and it's type reference. CP_PROP is public, and can be used to define
 * any non-standard properties required for possible calculations. Also here is defined
 * a requirement that each property data type has a non-empty constructor (empty properties
 * shell not be stored).
 *
 * Definition of the property type can be done elsewhere, however, there exist some
 * predefined, common, properties, known to all users of the CP class. These are defined
 * as a data types in CP:: namespace, like rho, coord, tc.. It is possible (and necessary,
 * because of origin info) to extend these types outside CP class. In this case,
 * it is possible to access required standard property from any point of the programm
 * by upcasting.
 *
 * In the case when two properties has the same standard base class, there happens
 * a problem of choice of the value after upcast. The solution is simple - there
 * shell be an "active" flag be stored. When this flag is changed, from outside it
 * fills like the property was just recalculated, so all dependencies shell be
 * also recalculated.
 * Todo: The "active" flag shell be implemented here, in this class, when needed.
 *
 *
 * Advantages and use-cases:
 *  - comparison of different computational methods
 *  - selection of an active property among any possibly similar
 *  - compile time property standardization
 *  - space and code save on unnecessary properties
 *  - no speed loss at repetitive property access by pointer
 *  - use of CP object for any temporary data storage
 *  - check if necessary data is present
 *
 *  Disadvantages:
 *  - time used to find a pointer onto the property
 *  - each property require 15 bytes + type_index overhead
 *  - definition of property types shell be completed outside this class,
 *    it was decided to require the definition of "origin" requirement each time.
 *
 * Typical usage:
 *
 * //specify property origin whithin a type
 * struct aim_coord : public CP::coord{
 *   wfx_coord(CP::coord::valtype val) : CP::coord(val) {};
 *   string origin(){return string("AIMAll calc");}
 * };
 *
 * //create CP object
 * CP cp(CP::BCP);
 *
 * //create property object
 * aim_coord c{{1,2,3}};
 *
 * //copy 'c' into 'cp'
 * cp.add_prop(c);
 *
 * //get shared pointer to aim_coord (stores x,y,z)
 * auto p = cp.get_prop<aim_coord>();
 *
 * //if aim_coord declaration is unknown we can access x,y,z by its parent
 * cout << "z-coord is: " << cp.get_prop<CP::coord>()->val[2] << endl;
 *
 * //remove property from CP
 * cp.del_prop<CP::coord>();
 *
 * //data is alive until the last shared_ptr object deleted (it is in stack now)
 * cout << p->val[0] << endl;
 *
 * //the property is completely removed now
 * p.release();
 */


class CP{
public:

    enum TYPE{
        NACP,  //Nuclear Attractor Critical Point
        NNACP, //Non-Nuclear Attractor Critical Point
        BCP,   //Bond Critical Point
        RCP,   //Ring Critical Point
        CCP};  //Cage Critical Point

private:
    struct BASE_PROP{
        virtual string origin() = 0;
        virtual string name(){return "NA";}
        virtual string description(){return "NA";}
    };

public:
    template<typename T>
    struct PROP : public BASE_PROP{
        typedef T valtype;
        PROP(T val):val{val} {};
        valtype val;
    };

    /*
     * define here any standard properties that shell be known in
     * different places of an application
     */
    struct coord : public PROP<array<double, 3>>{
        coord(coord::valtype val) : PROP<array<double, 3>>(val){};
        virtual string name(){return "coord";}
        virtual string description(){return "cartesian coordinates";}
    };

    struct rho : public PROP<double>{
        rho(rho::valtype val) : PROP<double>(val){};
        virtual string name(){return "rho";}
        virtual string description(){return "electron density at CP";}
    };

public:
    CP(TYPE cp_type) : cp_type{cp_type} {};

    template<typename T>
    void add_prop(const T& val){
        static_assert(is_base_of<BASE_PROP,T>::value, "not inherited from BASE_PROP");
        type_map[typeid(T)] = shared_ptr<T>(new T(val));
    };

    template<typename T>
    shared_ptr<T> get_prop(){
        static_assert(is_base_of<BASE_PROP,T>::value, "not inherited from BASE_PROP");
        auto it = type_map.find(typeid(T));
        //exact coincidence
        if(it != type_map.end()) return dynamic_pointer_cast<T>(it->second);
        //check that T is a base type of any
        for(auto& it : type_map){
            if(shared_ptr<T> p = dynamic_pointer_cast<T>(it.second))
                return p;
        }
        throw runtime_error("CP: requested property not found");
    }

    template<typename T>
    void del_prop(){
        static_assert(is_base_of<BASE_PROP,T>::value, "not inherited from BASE_PROP");
        for(auto it = type_map.begin(); it != type_map.end();){
            if(dynamic_pointer_cast<T>(it->second)){
                type_map.erase(it++);
            }else{
                ++it;
            }
        }
    }

    template<typename T>
    bool check_prop(){
        static_assert(is_base_of<BASE_PROP,T>::value, "not inherited from BASE_PROP");
        try{
            get_prop<T>();
            return true;
        }catch(...){
            return false;
        }
    }

public:
    const TYPE cp_type;
private:
    std::map<std::type_index, shared_ptr<BASE_PROP>> type_map;
};

#endif //DAIMAN_CRITICALPOINT_H
