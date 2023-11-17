#ifndef SHAREDROBJECT_HPP
#define SHAREDROBJECT_HPP
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include <Rcpp.h>
#include <RInside.h>
#include <Rinternals.h>
#include <Rembedded.h>
#include <boost/process.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>

namespace bp = boost::process;
namespace bip = boost::interprocess;

enum SMTYPE {
    SMVECTOR = 0,
    SMLIST,
    SMFUNCTION,
    SMENVIRONEMT,
    SMNUMERIC,
    UNKNOWN

};

struct sm_tuple {
    std::string sm_name;
    bool initialized;
    SMTYPE sm_type;
    void* object;
};

using namespace boost::interprocess;

// typedef allocator<sm_tuple, managed_shared_memory::segment_manager> SharedTupleAllocator;
// typedef std::vector<sm_tuple, SharedTupleAllocator> SharedTupleVector;
// managed_shared_memory segment_g;
// SharedTupleAllocator  allocator_g;
// SharedTupleVector processR_sm_declarations_g;

class SharedRObject {
    void* ptr = NULL;
public:
    std::string name;

    SharedRObject() {
    }

    bool is_SharedRObject(SEXP s) {
        try {
            
            if(this->is_Numeric(s)){
                return true;
            }
            
            if (TYPEOF(s) != S4SXP) {
                ::Rf_error("supplied object not from SharedR Library.");
                return false;
            }

            Rcpp::S4 s4obj(s);
            if (s4obj.is("Rcpp_SharedList")) {
                return true;
            } else if (s4obj.is("Rcpp_SharedVector")) {
                return true;
            }

        } catch (...) {
            ::Rf_error("supplied object not from SharedR Library.");
        }

        return false;
    }

    bool is_Numeric(SEXP s) {
        //REALSXP
        if (TYPEOF(s) == REALSXP) {
            return true;
        }
        return false;
    }

    SMTYPE getSharedRObjectType(SEXP s) {

        if (this->is_Numeric(s)) {
            return SMTYPE::SMNUMERIC;
        } 


        try {


            if (TYPEOF(s) != S4SXP ) {
                Rcpp::Rcout << __LINE__ << std::endl;
                std::stringstream ss;
                ss << TYPEOF(s) << " supplied object not from SharedR Library.";
                ::Rf_error(ss.str().c_str());
                return SMTYPE::UNKNOWN;
            }

            Rcpp::S4 s4obj(s);
            if (s4obj.is("Rcpp_SharedList")) {
                return SMTYPE::SMLIST;
            } else if (s4obj.is("Rcpp_SharedVector")) {
                return SMTYPE::SMVECTOR;

            }

        } catch (...) {
            ::Rf_error(" supplied object not from SharedR Library.");
        }

        return SMTYPE::UNKNOWN;
    }

};

namespace Rcpp {
    //    template <> SEXP wrap(const SharedList&);
    //    template <> SharedList as<SharedList>(SEXP) throw (not_compatible);

    template <> SEXP wrap<SharedRObject>(const SharedRObject& el) {
        Rcpp::Rcout << __LINE__ << std::endl;
        Rcpp::Language call("new", Symbol("SharedRObject"));
        return call.eval();
    }

    template <> SharedRObject as(SEXP s) {
        try {
            if (TYPEOF(s) != S4SXP) {
                ::Rf_error("supplied object is not of type SharedRObject.");
            }

            Rcpp::S4 s4obj(s);
            if (!s4obj.is("Rcpp_SharedRObject")) {
                ::Rf_error("supplied object is not of type SharedRObject.");
            }

            Rcpp::Environment env(s4obj);
            Rcpp::XPtr<SharedRObject> xptr(env.get(".pointer"));

            // build new SharedRObject object with copied data

            return SharedRObject((* xptr.get()));
        } catch (...) {
            ::Rf_error("supplied object could not be converted to SharedRObject.");
        }
    }
}

void SMInitialize(std::string system_name, bool open) {
    // shared_memory_object::remove(system_name.c_str());
    // 
    // //Create a new segment with given name and size
    // segment_g = managed_shared_memory(bip::create_only, system_name.c_str(), 65536);
    // 
    // //Initialize shared memory STL-compatible allocator
    // const SharedTupleAllocator alloc_inst(segment_g.get_segment_manager());
    // 
    // //Construct a vector named "MyVector" in shared memory with argument alloc_inst
    // processR_sm_declarations_g = segment_g.construct<SharedTupleVector>("SharedTupleVector")(alloc_inst);
}




#endif
