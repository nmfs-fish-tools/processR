#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
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

Rcpp::Environment copyEnvironment_(const Rcpp::Environment& sourceEnv) {
  Rcpp::Environment newEnv = Rcpp::new_env();

  Rcpp::CharacterVector names = sourceEnv.ls(true);

  for (int i = 0; i < names.size(); ++i) {
    std::string name = Rcpp::as<std::string>(names[i]);
    //        Rcpp::Symbol symbol(names[i]);
    SEXP value = sourceEnv[name];

    // Assign the symbol and its value to the new environment
    newEnv.assign(name, value);
  }



  return newEnv;
}

void WriteEnvironmentToSharedMemory(Rcpp::Environment env, std::string name) {
  Rcpp::Environment my_env = copyEnvironment_(env); //copy the environment to avoid race conditions.

  boost::interprocess::shared_memory_object::remove(name.c_str());

  SEXP shared_symbol;
  PROTECT(shared_symbol = Rf_install("my_shared_env"));

  SEXP shared_env;
  PROTECT(shared_env = R_MakeExternalPtr(my_env, shared_symbol, R_NilValue));

  const char* segmentName = name.c_str(); // Replace with a suitable name
  std::size_t segmentSize = sizeof (SEXP); // Size of the shared environment


  boost::interprocess::shared_memory_object shm(boost::interprocess::open_or_create, segmentName, boost::interprocess::read_write);
  shm.truncate(segmentSize);

  // Map the shared memory segment
  boost::interprocess::mapped_region region(shm, boost::interprocess::read_write);

  // Get a pointer to the memory in the mapped region
  SEXP* sharedMemoryPointer = static_cast<SEXP*> (region.get_address());

  // Copy the shared environment to shared memory
  *sharedMemoryPointer = shared_env;

  UNPROTECT(2); // Unprotect shared_symbol and shared_env


}

Rcpp::Environment readEnvironmentFromSharedMemory_(const std::string& shared_memory_name) {
  try {
    // Open the shared memory segment
    boost::interprocess::shared_memory_object shm(
        boost::interprocess::open_only, shared_memory_name.c_str(), boost::interprocess::read_only
    );

    // Map the shared memory segment into this process's address space
    boost::interprocess::mapped_region region(shm, boost::interprocess::read_only);

    // Get a pointer to the shared memory
    const void* shared_memory_ptr = region.get_address();

    // Deserialize the data into an R environment
    Rcpp::RawVector serialized_env(region.get_size());
    //        std::memcpy(serialized_env.begin(), shared_memory_ptr, region.get_size());
    SEXP arg;
    PROTECT(arg = Rf_allocVector(RAWSXP, region.get_size()));
    std::memcpy(RAW(arg), shared_memory_ptr, region.get_size() * sizeof (unsigned char));

    SEXP unser;
    PROTECT(unser = Rf_lang2(Rf_install("unserialize"), arg));

    int errorOccurred;
    SEXP ret = R_tryEval(unser, R_GlobalEnv, &errorOccurred);
    if (errorOccurred) {
      std::cout << "Error occurred unserializing environment." << std::endl;
    }

    UNPROTECT(2);
    Rcpp::Environment loaded_environment = Rcpp::as<Rcpp::Environment>(ret);


    return loaded_environment;
  } catch (const std::exception& e) {
    // Handle exceptions
    return R_NilValue;
  }
}

Rcpp::Environment loadREnvironment(const std::string& filePath) {
  Rcpp::Function load("load", Rcpp::Environment::base_env());
  load(filePath);

  // Return the loaded environment
  return Rcpp::Environment::global_env(); // You can change this to the desired environment
}

void copyToGlobalEnvironment(const Rcpp::Environment& sourceEnv) {
  Rcpp::Environment newEnv = Rcpp::Environment::global_env();

  Rcpp::CharacterVector names = sourceEnv.ls(true);

  for (int i = 0; i < names.size(); ++i) {
    std::string name = Rcpp::as<std::string>(names[i]);
    //        Rcpp::Symbol symbol(names[i]);
    SEXP value = sourceEnv[name];

    // Assign the symbol and its value to the new environment
    newEnv.assign(name, value);
  }

}

void copyFromGlobalEnvironment(Rcpp::Environment& newEnv) {
  Rcpp::Environment sourceEnv = Rcpp::Environment::global_env();

  Rcpp::CharacterVector names = sourceEnv.ls(true);

  for (int i = 0; i < names.size(); ++i) {
    std::string name = Rcpp::as<std::string>(names[i]);
    //        Rcpp::Symbol symbol(names[i]);
    SEXP value = sourceEnv[name];

    // Assign the symbol and its value to the new environment
    newEnv.assign(name, value);
  }

}

Rcpp::Function readFunctionFromSharedMemory_(const std::string& name) {

  bip::shared_memory_object fun_shm(bip::open_only, name.c_str(), bip::read_only);
  bip::mapped_region fun_region(fun_shm, bip::read_only);



  const unsigned char* sharedCharArray_fun = static_cast<const unsigned char*> (fun_region.get_address());
  Rcpp::RawVector serialized_fun(fun_region.get_size());
  std::memcpy(serialized_fun.begin(), sharedCharArray_fun, fun_region.get_size());

  SEXP arg2;
  PROTECT(arg2 = Rf_allocVector(RAWSXP, serialized_fun.size()));
  memcpy(RAW(arg2), &serialized_fun[0], serialized_fun.size() * sizeof (unsigned char));

  SEXP unser2;
  PROTECT(unser2 = Rf_lang2(Rf_install("unserialize"), arg2));


  int errorOccurred2;
  SEXP ret2 = R_tryEval(unser2, R_GlobalEnv, &errorOccurred2);
  if (errorOccurred2) {
    std::cout << "Error occurred unserializing function." << std::endl;
  }

  UNPROTECT(2);
  Rcpp::Function function = Rcpp::as<Rcpp::Function>(ret2);

  return function;

}

void writeToSharedMemory_(const std::string& shared_memory_name, const Rcpp::RawVector& serialized_env) {
  try {
    boost::interprocess::shared_memory_object::remove(shared_memory_name.c_str());
    // Create or open the shared memory segment
    boost::interprocess::shared_memory_object shm(
        boost::interprocess::open_or_create, shared_memory_name.c_str(), boost::interprocess::read_write
    );

    // Set the size of the shared memory segment
    shm.truncate(serialized_env.size());

    // Map the shared memory segment into this process's address space
    boost::interprocess::mapped_region region(shm, boost::interprocess::read_write);

    // Get a pointer to the shared memory
    unsigned char* shared_memory_ptr = static_cast<unsigned char*> (region.get_address());

    std::copy(serialized_env.begin(), serialized_env.end(), shared_memory_ptr);
    // Copy the serialized data to shared memory
    // std::memcpy(shared_memory_ptr, serialized_env.begin(), serialized_env.size());

    // Note: Add error handling as needed
  } catch (const std::exception& e) {
    // Handle exceptions
    Rcpp::Rcout << e.what() << std::endl;
  }
}

 int main(int argc, char** argv) {
  RInside R(argc, argv);

  bp::ipstream parent_input;
  bp::opstream parent_output;

  // Read the message from the parent
  std::string ss_env_name = "processR_sm_env_1695317085";
  std::string ss_fun_name = "processR_sm_fun_1695317085";
  int rank;
  std::cin >> rank;
  std::getline(std::cin, ss_env_name);
  std::getline(std::cin, ss_fun_name);


  Rcpp::Environment environment = readEnvironmentFromSharedMemory_(ss_env_name);
  environment.assign("processR.rank", rank);
  copyToGlobalEnvironment(environment);
  Rcpp::Function function = readFunctionFromSharedMemory_(ss_fun_name);


  SEXP ret = function();
  

  std::stringstream sm_name_env_ret;
  sm_name_env_ret << ss_env_name << "_ret";

  Rcpp::Environment baseEnv("package:base");
  Rcpp::Function serializeFunc = baseEnv["serialize"];
  Rcpp::Environment ret_env = Rcpp::new_env(); // = Rcpp::Environment::global_env();
  copyFromGlobalEnvironment(ret_env);
  ret_env.assign("processR.return", ret);
  Rcpp::RawVector serialized_env = serializeFunc(ret_env, R_NilValue);

  writeToSharedMemory_(sm_name_env_ret.str(), serialized_env);

  return 0;
}