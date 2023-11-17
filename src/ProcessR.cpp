#include <filesystem>
#include <fstream>
#include <thread>

#include "../inst/include/SharedR.hpp"

#include <Rcpp.h>
#include <Rinternals.h>


#include <sstream>
#include <chrono>
#include <boost/process.hpp>

#if defined(linux) || defined(__linux) || defined(__linux__) || defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
#define P_POSIX
#include <unistd.h>  
#include <pwd.h>  
#endif  

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(_WIN64) || defined(__WIN64__) || defined(WIN64)
#define P_WINDOWS
#include<Windows.h>  
#endif  

std::string getUserName() {
#if defined P_POSIX
  uid_t userid;
  struct passwd* pwd;
  userid = getuid();
  pwd = getpwuid(userid);
  return pwd->pw_name;
  
#elif defined P_WINDOWS
  const int MAX_LEN = 100;
  char szBuffer[MAX_LEN];
  DWORD len = MAX_LEN;
  if (GetUserName(szBuffer, &len))
    return szBuffer;
  
#else
  return "some_user";
#endif  
}



#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
// #  define BOOST_USE_WINDOWS_H
// #pragma comment(lib, "ws2_32.lib")
#define _glibcxx_use_cxx11_abi=0
#elif defined(_WIN64) || defined(__WIN64__) || defined(WIN64)
// #  define BOOST_USE_WINDOWS_H
// #pragma comment(lib, "ws2_32.lib")
#define _glibcxx_use_cxx11_abi=0
#endif

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/process.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>


namespace bp = boost::process;
namespace bip = boost::interprocess;

std::string getprocessRPath() {
  Rcpp::Function find_package("find.package");
  Rcpp::CharacterVector result = find_package("processR");
  if (result.size() == 1) {
    return Rcpp::as<std::string>(result[0]);
  } else {
    // Handle the case where the environment variable is not found or there are multiple paths
    return ""; // You can return an appropriate default value or handle the error as needed
  }
}

std::string getRPath(){
  Rcpp::Function find_package("Sys.getenv");
  Rcpp::CharacterVector result = find_package("R_HOME");
  if (result.size() == 1) {
    std::stringstream ss;
    ss<<Rcpp::as<std::string>(result[0])<<"/bin/R";
    return ss.str();
  } else {
    // Handle the case where the environment variable is not found or there are multiple paths
    return ""; // You can return an appropriate default value or handle the error as needed
  }
  
}

int check(std::string ss_env_name, std::string ss_fun_name) {
  bp::ipstream parent_input;
  bp::opstream parent_output;
  Rcpp::Rcout << "check....\n";
  Rcpp::Rcout << ss_env_name << "\n";
  
  
  bip::shared_memory_object env_shm(bip::open_only, ss_env_name.c_str(), bip::read_only);
  bip::shared_memory_object fun_shm(bip::open_only, ss_fun_name.c_str(), bip::read_only);
  
  bip::mapped_region env_region(env_shm, bip::read_only);
  bip::mapped_region fun_region(fun_shm, bip::read_only);
  
  const unsigned char* sharedCharArray_env = static_cast<const unsigned char*> (env_region.get_address());
  Rcpp::RawVector serialized_env(env_region.get_size());
  std::memcpy(serialized_env.begin(), sharedCharArray_env, env_region.get_size());
  
  const unsigned char* sharedCharArray_fun = static_cast<const unsigned char*> (fun_region.get_address());
  Rcpp::RawVector serialized_fun(fun_region.get_size());
  std::memcpy(serialized_fun.begin(), sharedCharArray_fun, fun_region.get_size());
  
  
  SEXP arg;
  PROTECT(arg = Rf_allocVector(RAWSXP, env_region.get_size()));
  std::memcpy(RAW(arg), &serialized_env[0], serialized_env.size() * sizeof (unsigned char));
  
  
  SEXP unser;
  PROTECT(unser = Rf_lang2(Rf_install("unserialize"), arg));
  
  int errorOccurred;
  SEXP ret = R_tryEval(unser, R_GlobalEnv, &errorOccurred);
  if (errorOccurred) {
    Rcpp::Rcout << "Error occurred unserializing environment." << std::endl;
  }
  
  
  Rcpp::Environment environment = Rcpp::as<Rcpp::Environment>(ret);
  
  
  // 
  SEXP arg2;
  PROTECT(arg2 = Rf_allocVector(RAWSXP, serialized_fun.size()));
  memcpy(RAW(arg2), &serialized_fun[0], serialized_fun.size() * sizeof (unsigned char));
  
  SEXP unser2;
  PROTECT(unser2 = Rf_lang2(Rf_install("unserialize"), arg2));
  
  int errorOccurred2;
  SEXP ret2 = R_tryEval(unser2, R_GlobalEnv, &errorOccurred2);
  if (errorOccurred2) {
    Rcpp::Rcout << "Error occurred unserializing function." << std::endl;
  }
  
  Rcpp::Function function = Rcpp::as<Rcpp::Function>(ret2);
  
  Rcpp::List l = Rcpp::as<Rcpp::List>(environment.ls(true));
  
  Rcpp::Rcout << "print env->\n";
  for (int i = 0; i < l.size(); i++) {
    Rcpp::Rcout << Rcpp::as<std::string>(l[i]) << "\n";
  }
  Rcpp::Rcout << "->done\n" << std::flush;
  function();
  
  Rcpp::Rcout << "done check.\n\n\n" << std::flush;
  
  
  
  return 0;
}

// [[Rcpp::export]]
Rcpp::Environment copyEnvironment(const Rcpp::Environment& sourceEnv) {
  Rcpp::Environment newEnv = Rcpp::new_env();
  
  Rcpp::CharacterVector names = sourceEnv.ls(true);
  
  for (int i = 0; i < names.size(); ++i) {
    std::string name = Rcpp::as<std::string>(names[i]);
    SEXP value = sourceEnv[name];
    
    // Assign the symbol and its value to the new environment
    newEnv.assign(name, value);
  }
  
  
  
  return newEnv;
}

// [[Rcpp::export]]
void writeToSharedMemory(const std::string& shared_memory_name, const Rcpp::RawVector& serialized_env) {
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



// [[Rcpp::export]]
Rcpp::Environment readEnvironmentFromSharedMemory(const std::string& shared_memory_name) {
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
      Rcpp::Rcout << "Error occurred unserializing environment." << std::endl;
    }
    
    
    Rcpp::Environment loaded_environment = Rcpp::as<Rcpp::Environment>(ret);
    
    UNPROTECT(2);
    
    return loaded_environment;
  } catch (const std::exception& e) {
    // Handle exceptions
    Rcpp::Rcout <<"Unable to read environment from shared memory."<<std::endl;
    return R_NilValue;
  }
}

Rcpp::Function readFunctionFromSharedMemory(const std::string& name) {
  
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

void copyEnvironment(const Rcpp::Environment& sourceEnv, Rcpp::Environment& newEnv) {
  // Rcpp::Environment newEnv = Rcpp::Environment::global_env();
  
  Rcpp::CharacterVector names = sourceEnv.ls(true);
  
  for (int i = 0; i < names.size(); ++i) {
    std::string name = Rcpp::as<std::string>(names[i]);
    //        Rcpp::Symbol symbol(names[i]);
    SEXP value = sourceEnv[name];
    
    // Assign the symbol and its value to the new environment
    newEnv.assign(name, value);
  }
  
}

Rcpp::Environment CallRProcess(const Rcpp::Function& fun, const Rcpp::Environment& env);

class Process {
  std::shared_ptr<boost::process::child> child_m;
  std::string sm_name_m;
public:
  int rank;
  Rcpp::RawVector fun; //serialized function
  Rcpp::RawVector env; //serialized environment
  Rcpp::Environment environment;
  boost::process::ipstream child_output;
  boost::process::opstream child_input;
  std::thread thread_object;
  std::stringstream message;
  bool is_r_process = true;
  bool write_log = false;
  
  Process(const Process& other) :
    child_m((other.child_m)), rank(other.rank), fun(other.fun), env(other.env),is_r_process(other.is_r_process) {
  }
  
  
  Process() {
    
  }
  
  Process(const Rcpp::Function& fun, const Rcpp::Environment& env, const int& rank){
    this->start(fun, env, rank);
  }
  
  Process(const std::string& path) {
    this->is_r_process = false;
    // Launch the child process
    this->child_m = std::make_shared<boost::process::child>(path,
                                                            boost::process::std_in < this->child_input,
                                                            boost::process::std_out > this->child_output
    );
  }
  
  void start(const Rcpp::Function& fun, const Rcpp::Environment& env, const int& rank) {
    this->is_r_process = true;
    // Rcpp::Environment E = copyEnvironment(env);
    // E.assign("processR.rank", rank);
    // 

    
    // 
    // //Copy environment to shared memory
    // Rcpp::Environment E = copyEnvironment(env);
    // E.assign("processR.rank", rank);
    // std::string path = "/Users/mattadmin/FIMS-Testing/FIMS-v0100_2/RChild/dist/Debug/GNU-MacOSX/rchild"; //"""/Users/mattadmin/rprojects/processR/src/RRunner.x";
    // std::stringstream ss_path;
    // 
    // ss_path<<getprocessRPath()<<"/bin";
    // if (std::filesystem::exists(ss_path.str()) && std::filesystem::is_directory(ss_path.str())) {
    //   ss_path<<"/rchild";
    //   path = ss_path.str();
    // } else {
    //  Rcpp::Rcout << "Library directory \""<<ss_path.str()<<" does not exist.\"" << std::endl;
    // }
    // 
    // 
    // std::stringstream sm_name_env;
    // std::stringstream sm_name_env_ret;
    // std::stringstream sm_name_fun;
    // std::time_t t = std::time(0);
    // sm_name_env << "processR_sm_env_" << t<<"_"<<rank;
    // sm_name_fun << "processR_sm_fun_" << t<<"_"<<rank;
    // sm_name_env_ret<< sm_name_env.str()<<"_ret";
    // this->sm_name_m = sm_name_env_ret.str();
    // // Rcpp::Rcout << sm_name_env.str() << "\n\n";
    // 
    // 
    // // this->fun = fun;
    // // this->env = E;
    // 
    // Rcpp::Environment baseEnv("package:base");
    // Rcpp::Function serializeFunc = baseEnv["serialize"];
    // Rcpp::RawVector serialized_env = serializeFunc(E, R_NilValue);
    // Rcpp::RawVector serialized_fun = serializeFunc(fun, R_NilValue);
    // 
    // writeToSharedMemory(sm_name_env.str(), serialized_env);
    // writeToSharedMemory(sm_name_fun.str(), serialized_fun);
    // 
    
    std::string path = getRPath() +" --slave --no-save -e "; 
    
    std::stringstream sm_name_env;
    std::stringstream sm_name_env_ret;
    std::stringstream sm_name_fun;
    std::time_t t = std::time(0);
    sm_name_env << "processR_sm_env_" << t<<"_"<<rank;
    sm_name_fun << "processR_sm_fun_" << t<<"_"<<rank;
    sm_name_env_ret<< sm_name_env.str()<<"_ret";
    this->sm_name_m = sm_name_env_ret.str();
    
    //Copy environment to shared memory
    Rcpp::Environment E = copyEnvironment(env);
    E.assign("processR.rank", rank);
    
    Rcpp::Environment baseEnv("package:base");
    Rcpp::Function serializeFunc = baseEnv["serialize"];
    Rcpp::RawVector serialized_env = serializeFunc(E, R_NilValue);
    Rcpp::RawVector serialized_fun = serializeFunc(fun, R_NilValue);
    
    writeToSharedMemory(sm_name_env.str(), serialized_env);
    writeToSharedMemory(sm_name_fun.str(), serialized_fun);
    
    std::stringstream ss;
    
    ss<<path;
    ss<<"\"library(processR) \n processR::RunChild("<< "\""<< sm_name_fun.str() <<"\", \""<<sm_name_env.str()<<"\")\"";
    
    
    
    
    // Launch the child process
    this->child_m = std::make_shared<boost::process::child>(ss.str(),
                                                            boost::process::std_in < this->child_input,
                                                            boost::process::std_out > this->child_output
    );

    child_input << rank;
    child_input.flush();

    child_input << sm_name_env.str() << std::endl;
    child_input.flush();

    child_input << sm_name_fun.str() << std::endl;
    child_input.flush();
    
  }
  
  void join() {
    this->collect();
    this->child_m->join();
    if(this->is_r_process){
      this->environment = readEnvironmentFromSharedMemory(sm_name_m); 
    }
  }
  
  bool joinable(){
    return this->child_m->joinable();
  }
  
  void wait() {
    this->collect();
    this->child_m->wait();
    if(this->is_r_process){
      this->environment = readEnvironmentFromSharedMemory(sm_name_m); 
    }
  }
  
  void terminate() {
    this->collect();
    this->child_m->terminate();
    if(this->is_r_process){
      this->environment = readEnvironmentFromSharedMemory(sm_name_m); 
    }
  }
  
  void collect(){
    std::string line;
    while (this->child_output && std::getline(this->child_output, line) && !line.empty()) {
      this->message << line << std::endl;
    }
    
    if(this->write_log){
      std::stringstream ss;
      ss<<getUserName()<<"_processR_"<<this->pid()<<".log";
      std::ofstream out(ss.str().c_str());
      out << this->message.str() <<"\n";
      out.close();
    }
  }
  
  bool valid(){
    return this->child_m->valid();
  }
  
  bool running(){
    return this->child_m->running();
  }
  
  int exit_code(){
    return this->child_m->exit_code();
  }
  
  size_t pid(){
    return this->child_m->id();
  }
  
  std::string get_message(){
    this->collect();
    return this->message.str();;
  }
  
  void print(){
    Rcpp::Rcout<<this->message.str();
  }
  
  Rcpp::Environment get_environment(){
    return this->environment;
  }
  
  std::string read_line(){
    std::string line;
    std::getline(this->child_output, line);
    return line;
  }
  
  void write(const std::string& str){
    this->child_input <<str;
  }
  
};



// // [[Rcpp::export]]
// Process CreateProcess(){
//   Process p;
//   return p;
// }

template<typename CLASS>
class SharedStorage{
  
public:
  
  SharedStorage() : data(R_NilValue), token(R_NilValue){}
  
  ~SharedStorage(){
    // Rcpp::Rcpp_PreciousRelease(token) ;
    // data = R_NilValue;
    // token = R_NilValue;
  }
  
  inline void set__(SEXP x){
    if (data != x) {
      try {
        std::string shared_memory_name = "RSharedStorage";
        boost::interprocess::shared_memory_object::remove(shared_memory_name.c_str());
        // Create or open the shared memory segment
        boost::interprocess::shared_memory_object shm(
            boost::interprocess::open_or_create, shared_memory_name.c_str(), boost::interprocess::read_write
        );
        
        // Set the size of the shared memory segment
        shm.truncate(1024);
        
        // Map the shared memory segment into this process's address space
        boost::interprocess::mapped_region region(shm, boost::interprocess::read_write);
        
        // Get a pointer to the shared memory
        unsigned char* shared_memory_ptr = static_cast<unsigned char*> (region.get_address());
        data = static_cast<SEXP> (region.get_address());
        unsigned char* X = (unsigned char*)x;
        memcpy(&X[0], shared_memory_ptr, sizeof(X) * sizeof (unsigned char));        
      
        Rcpp::Rcpp_PreciousRelease(token);
        token = Rcpp::Rcpp_PreciousPreserve(data);
      
      }catch(...){
        Rcpp::Rcout <<"Error creating shared storage. "<<std::endl;
      }

    }
    
    // calls the update method of CLASS
    // this is where to react to changes in the underlying SEXP
    static_cast<CLASS&>(*this).update(data) ;
  }
  
  inline SEXP get__() const {
    return data ;
  }
  
  inline SEXP invalidate__(){
    SEXP out = data ;
    Rcpp::Rcpp_PreciousRelease(token);
    data = R_NilValue ;
    token = R_NilValue ;
    return out ;
  }
  
  template <typename T>
  inline T& copy__(const T& other){
    if( this != &other){
      set__(other.get__());
    }
    return static_cast<T&>(*this) ;
  }
  
  inline bool inherits(const char* clazz) const {
    return ::Rf_inherits( data, clazz) ;
  }
  
  inline operator SEXP() const { return data; }
  
  
private:
  
  SEXP data ;
  SEXP token ;
  
  
};

// [[Rcpp::export]]
SEXP CreateSharedEnvironment(const std::string& name){
  // std::string name = "RSharedPtr";
  SEXP res = Rf_allocSExp(ENVSXP);
  boost::interprocess::shared_memory_object::remove(name.c_str());
  
  SEXP shared_symbol;
  PROTECT(shared_symbol = Rf_install("my_shared_env"));
  
  SEXP shared_env;
  PROTECT(shared_env = R_MakeExternalPtr(res, shared_symbol, R_NilValue));
  
  const char* segmentName = name.c_str(); // Replace with a suitable name
  std::size_t segmentSize = sizeof (SEXP); // Size of the shared environment
  
  
  boost::interprocess::shared_memory_object shm(boost::interprocess::open_or_create, segmentName, boost::interprocess::read_write);
  shm.truncate(10000000);
  
  // Map the shared memory segment
  boost::interprocess::mapped_region region(shm, boost::interprocess::read_write);
  std::memset(region.get_address(), 0, region.get_size());
  
  // Get a pointer to the memory in the mapped region
  SEXP* sharedMemoryPointer = static_cast<SEXP*> (region.get_address());
  
  // Copy the shared environment to shared memory
  *sharedMemoryPointer = shared_env;
  
  UNPROTECT(2);
  Rcpp::Rcout<<"Address from write "<<region.get_address()<<std::endl;
  return static_cast<SEXP>(R_ExternalPtrAddr(*sharedMemoryPointer));
}

// [[Rcpp::export]]
void CreateSharedEnvironmentNoReturn(const std::string& name){
  // std::string name = "RSharedPtr";
  SEXP res = Rf_allocSExp(ENVSXP);
  boost::interprocess::shared_memory_object::remove(name.c_str());
  
  SEXP shared_symbol;
  PROTECT(shared_symbol = Rf_install("my_shared_env"));
  
  SEXP shared_env;
  PROTECT(shared_env = R_MakeExternalPtr(res, shared_symbol, R_NilValue));
  
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
  
  UNPROTECT(2);
  // return static_cast<SEXP>(R_ExternalPtrAddr(*sharedMemoryPointer));
}


// [[Rcpp::export]]
SEXP readSharedEnvironment(const std::string& name){
  Rcpp::Rcout<<"Line 685"<<std::endl;
  // Open the shared memory segment
  boost::interprocess::shared_memory_object shm(
      boost::interprocess::open_only, name.c_str(), boost::interprocess::read_write
  );
    
    Rcpp::Rcout<<"Line 691"<<std::endl;
  // Map the shared memory segment into this process's address space
  boost::interprocess::mapped_region region(shm, boost::interprocess::read_write);
  Rcpp::Rcout<<"Address from read "<<region.get_address()<<std::endl;
  Rcpp::Rcout<<"Line 695"<<std::endl;
  // Get a pointer to the shared memory
  const void* shared_memory_ptr = region.get_address();

  Rcpp::Rcout<<"Line 699"<<std::endl;
  SEXP sharedMemoryPointer = static_cast<SEXP> (region.get_address());
  Rcpp::Rcout<<"Line 701"<<std::endl;
  SEXP env = static_cast<SEXP>(/*R_ExternalPtrAddr*/(sharedMemoryPointer));
  Rcpp::Rcout<<"Line 703"<<std::endl;
  return env;//Rcpp::Environment(env);
}

class SharedEnvironment{
  std::string name_m;
  bool exists = false;
  Rcpp::Environment env;
public: 
  
  SharedEnvironment(){}
  
  SharedEnvironment(const std::string& name){
    this->name_m = name;
    this->env = readSharedEnvironment(name);
  }
  
  void create(const std::string& name){
    this->name_m = name;
    this->env = CreateSharedEnvironment(name);
  }
  
  void load(const std::string& name){
    this->name_m = name;
    this->env = readSharedEnvironment(name);
  }
  
  operator Rcpp::Environment(){
    return this->env;
  }
  
  Rcpp::Environment get(){
    return this->env;
  }
  
  std::string name(){
    return this->name_m;
  }
};

// [[Rcpp::export]]
void RunProcess(Rcpp::Function fun, Rcpp::Environment env) {
  
  std::string path = getprocessRPath() +"bin/rchild"; //"""/Users/mattadmin/rprojects/processR/src/RRunner.x";
  
  
  std::stringstream sm_name_env;
  std::stringstream sm_name_fun;
  std::time_t t = std::time(0);
  sm_name_env << "processR_sm_env_" << t;
  sm_name_fun << "processR_sm_fun_" << t;
  Rcpp::Rcout << sm_name_env.str() << "\n\n";
  
  //Copy environment to shared memory
  Rcpp::Environment E = copyEnvironment(env);
  
  
  Rcpp::Environment baseEnv("package:base");
  Rcpp::Function serializeFunc = baseEnv["serialize"];
  Rcpp::RawVector serialized_env = serializeFunc(E, R_NilValue);
  Rcpp::RawVector serialized_fun = serializeFunc(fun, R_NilValue);
  
  writeToSharedMemory(sm_name_env.str(), serialized_env);
  writeToSharedMemory(sm_name_fun.str(), serialized_fun);
  
  // Launch the child process
  boost::process::ipstream child_output;
  boost::process::opstream child_input;
  boost::process::child childProcess(path,
                                     boost::process::std_in < child_input,
                                                              boost::process::std_out > child_output
  );
  
  
  int rank = 0;
  child_input << rank;
  child_input.flush();
  
  child_input << sm_name_env.str() << std::endl;
  child_input.flush();
  
  child_input << sm_name_fun.str() << std::endl;
  child_input.flush();
  
  std::string line;
  while (child_output && std::getline(child_output, line) && !line.empty()) {
    Rcpp::Rcout << line << std::endl;
  }
  
  // // Wait for the child process to finish
  childProcess.wait();
  
  
}



void copyToGlobalEnvironment_(const Rcpp::Environment& sourceEnv) {
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


void copyFromGlobalEnvironment_(Rcpp::Environment& newEnv) {
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

// [[Rcpp::export]]
void RunChild(const std::string& fun, const std::string& env){
  Rcpp::Function func = readFunctionFromSharedMemory(fun);
  Rcpp::Environment environ_ = readEnvironmentFromSharedMemory(env);
  copyToGlobalEnvironment_(environ_);

  
  SEXP ret = func();
  
  
  std::stringstream sm_name_env_ret;
  sm_name_env_ret << env << "_ret";
  
  Rcpp::Environment baseEnv("package:base");
  Rcpp::Function serializeFunc = baseEnv["serialize"];
  Rcpp::Environment ret_env = Rcpp::new_env(); // = Rcpp::Environment::global_env();
  copyFromGlobalEnvironment_(ret_env);
  ret_env.assign("processR.return", ret);
  Rcpp::RawVector serialized_env = serializeFunc(ret_env, R_NilValue);
  
  writeToSharedMemory(sm_name_env_ret.str(), serialized_env);
}

// [[Rcpp::export]]
Rcpp::Environment CallRProcess(const Rcpp::Function& fun, const Rcpp::Environment& env){
  std::string path = getRPath() +" --slave --no-save -e "; //"""/Users/mattadmin/rprojects/processR/src/RRunner.x";
  
  
  std::stringstream sm_name_env;
  std::stringstream sm_name_fun;
  std::time_t t = std::time(0);
  sm_name_env << "processR_sm_env_" << t;
  sm_name_fun << "processR_sm_fun_" << t;
  Rcpp::Rcout << sm_name_env.str() << "\n\n";
  
  //Copy environment to shared memory
  Rcpp::Environment E = copyEnvironment(env);
  
  
  Rcpp::Environment baseEnv("package:base");
  Rcpp::Function serializeFunc = baseEnv["serialize"];
  Rcpp::RawVector serialized_env = serializeFunc(E, R_NilValue);
  Rcpp::RawVector serialized_fun = serializeFunc(fun, R_NilValue);
  
  writeToSharedMemory(sm_name_env.str(), serialized_env);
  writeToSharedMemory(sm_name_fun.str(), serialized_fun);
  
  std::stringstream ss;
  
  ss<<path;
  ss<<"\"library(processR) \n processR::RunChild("<< "\""<< sm_name_fun.str() <<"\", \""<<sm_name_env.str()<<"\")\"";
  
  Rcpp::Rcout <<ss.str()<<"\n";
  
  Process p(ss.str());
  p.wait();
  Rcpp::Rcout << p.get_message();
  ss.str("");
  ss<<sm_name_env.str()<<"_ret";
  
 return readEnvironmentFromSharedMemory(ss.str());
}

RCPP_EXPOSED_CLASS(Process)
  
  RCPP_MODULE(processR) {
    Rcpp::class_<Process>("Process")
    .constructor()
    .constructor<std::string>()
    .constructor<Rcpp::Function, Rcpp::Environment, int>()
    .method("start", &Process::start, "Start the child process.")
    .method("join", &Process::join, "Join the child. This just calls wait.")
    .method("joinable", &Process::joinable, "Check if the child is joinable.")
    .method("wait", &Process::wait,"Wait for the child process to exit.")
    .method("terminate", &Process::terminate, "Terminate the child process.This function will cause the child process to unconditionally and immediately exit. It is implement with SIGKILL on posix and TerminateProcess on windows.")
    .method("valid", &Process::valid, "Check if this handle holds a child process.")
    .method("pid", &Process::pid, "Get the Process Identifier.")
    .method("running", &Process::running,"Check if the child process is running.")
    .method("exit_code", &Process::exit_code,"Get the exit_code. The return value is without any meaning if the child wasn't waited for or if it was terminated.")
    .method("get_message", &Process::get_message, "Get data sent to output stream")
    .method("print", &Process::print, "Print the output stream.")
    .method("get_environment", &Process::get_environment, "Get the return R environment.")
    .method("read_line", &Process::read_line, "Read a line from the process out stream.")
    .method("write", &Process::write, "Write to the process in stream.")
    .field("write_log", &Process::write_log, "Write to the process in stream to log.")
    .field("rank", &Process::rank, "The user assigned rank.");
    Rcpp::class_<SharedList>("SharedList")
      .constructor()
    .constructor<SharedList>()
      .method("create", &SharedList::create, "create shared memory for SharedList.")
      .method("open", &SharedList::open, "open shared memory for SharedList.")
      .method("get", &SharedList::get, "get element from SharedList.")
      .method("set", &SharedList::set, "set element in SharedList.")
      .method("destroy", &SharedList::destroy, "destroy SharedList.");
    Rcpp::class_<SharedVector>("SharedVector")
      .constructor()
      .constructor<SharedVector> ()
      .method("create", &SharedVector::create, "create shared memory for SharedVector.")
      .method("open", &SharedVector::open, "open shared memory for SharedVector.")
      .method("get", &SharedVector::get, "get element from SharedVector.")
      .method("set", &SharedVector::set, "set element in SharedVector.")
      .method("resize", &SharedVector::resize, "resize SharedVector.")
      .method("size", &SharedVector::size, "get the size of SharedVector.")
      .method("destroy", &SharedVector::destroy, "destroy SharedVector.");
      Rcpp::function("as_shared_vector", &as_shared_vector);
      }

// // [[Rcpp::export]]
// Process CreateProcess(){
//   return Process();
// }


// [[Rcpp::export]]
size_t HardwareConcurrency(){
  return std::thread::hardware_concurrency();
}

/**
 * Returns a list of children.
 */

// [[Rcpp::export]]
Rcpp::List CreateProcessPool(int size) {
  
  Rcpp::List l(size);
  
  for (int i = 0; i < l.size(); i++) {
    
    l.at(i) = Rcpp::wrap(Process());
    // Rcpp::as<Process>(l.at(i)).rank = i;
  }
  
  return l;
}

