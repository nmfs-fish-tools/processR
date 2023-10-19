#include <filesystem>
#include <thread>
#include <Rcpp.h>
#include <sstream>
#include <chrono>
#include <boost/process.hpp>
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
    
    //Copy environment to shared memory
    Rcpp::Environment E = copyEnvironment(env);
    E.assign("processR.rank", rank);
    std::string path = "/Users/mattadmin/FIMS-Testing/FIMS-v0100_2/RChild/dist/Debug/GNU-MacOSX/rchild"; //"""/Users/mattadmin/rprojects/processR/src/RRunner.x";
    std::stringstream ss_path;
    
    ss_path<<getprocessRPath()<<"/bin";
    if (std::filesystem::exists(ss_path.str()) && std::filesystem::is_directory(ss_path.str())) {
      ss_path<<"/rchild";
      path = ss_path.str();
    } else {
     Rcpp::Rcout << "Library directory \""<<ss_path.str()<<" does not exist.\"" << std::endl;
    }
    
    
    std::stringstream sm_name_env;
    std::stringstream sm_name_env_ret;
    std::stringstream sm_name_fun;
    std::time_t t = std::time(0);
    sm_name_env << "processR_sm_env_" << t<<"_"<<rank;
    sm_name_fun << "processR_sm_fun_" << t<<"_"<<rank;
    sm_name_env_ret<< sm_name_env.str()<<"_ret";
    this->sm_name_m = sm_name_env_ret.str();
    // Rcpp::Rcout << sm_name_env.str() << "\n\n";
    
    
    // this->fun = fun;
    // this->env = E;
    
    Rcpp::Environment baseEnv("package:base");
    Rcpp::Function serializeFunc = baseEnv["serialize"];
    Rcpp::RawVector serialized_env = serializeFunc(E, R_NilValue);
    Rcpp::RawVector serialized_fun = serializeFunc(fun, R_NilValue);
    
    writeToSharedMemory(sm_name_env.str(), serialized_env);
    writeToSharedMemory(sm_name_fun.str(), serialized_fun);
    
    // Launch the child process
    
    this->child_m = std::make_shared<boost::process::child>(path,
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
    .field("rank", &Process::rank, "The user assigned rank.");
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

