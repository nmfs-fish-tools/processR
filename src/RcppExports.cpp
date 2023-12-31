// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

#ifdef RCPP_USE_GLOBAL_ROSTREAM
Rcpp::Rostream<true>&  Rcpp::Rcout = Rcpp::Rcpp_cout_get();
Rcpp::Rostream<false>& Rcpp::Rcerr = Rcpp::Rcpp_cerr_get();
#endif

// copyEnvironment
Rcpp::Environment copyEnvironment(const Rcpp::Environment& sourceEnv);
RcppExport SEXP _processR_copyEnvironment(SEXP sourceEnvSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const Rcpp::Environment& >::type sourceEnv(sourceEnvSEXP);
    rcpp_result_gen = Rcpp::wrap(copyEnvironment(sourceEnv));
    return rcpp_result_gen;
END_RCPP
}
// writeToSharedMemory
void writeToSharedMemory(const std::string& shared_memory_name, const Rcpp::RawVector& serialized_env);
RcppExport SEXP _processR_writeToSharedMemory(SEXP shared_memory_nameSEXP, SEXP serialized_envSEXP) {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const std::string& >::type shared_memory_name(shared_memory_nameSEXP);
    Rcpp::traits::input_parameter< const Rcpp::RawVector& >::type serialized_env(serialized_envSEXP);
    writeToSharedMemory(shared_memory_name, serialized_env);
    return R_NilValue;
END_RCPP
}
// readEnvironmentFromSharedMemory
Rcpp::Environment readEnvironmentFromSharedMemory(const std::string& shared_memory_name);
RcppExport SEXP _processR_readEnvironmentFromSharedMemory(SEXP shared_memory_nameSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const std::string& >::type shared_memory_name(shared_memory_nameSEXP);
    rcpp_result_gen = Rcpp::wrap(readEnvironmentFromSharedMemory(shared_memory_name));
    return rcpp_result_gen;
END_RCPP
}
// CreateSharedEnvironment
SEXP CreateSharedEnvironment(const std::string& name);
RcppExport SEXP _processR_CreateSharedEnvironment(SEXP nameSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const std::string& >::type name(nameSEXP);
    rcpp_result_gen = Rcpp::wrap(CreateSharedEnvironment(name));
    return rcpp_result_gen;
END_RCPP
}
// CreateSharedEnvironmentNoReturn
void CreateSharedEnvironmentNoReturn(const std::string& name);
RcppExport SEXP _processR_CreateSharedEnvironmentNoReturn(SEXP nameSEXP) {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const std::string& >::type name(nameSEXP);
    CreateSharedEnvironmentNoReturn(name);
    return R_NilValue;
END_RCPP
}
// readSharedEnvironment
SEXP readSharedEnvironment(const std::string& name);
RcppExport SEXP _processR_readSharedEnvironment(SEXP nameSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const std::string& >::type name(nameSEXP);
    rcpp_result_gen = Rcpp::wrap(readSharedEnvironment(name));
    return rcpp_result_gen;
END_RCPP
}
// RunProcess
void RunProcess(Rcpp::Function fun, Rcpp::Environment env);
RcppExport SEXP _processR_RunProcess(SEXP funSEXP, SEXP envSEXP) {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< Rcpp::Function >::type fun(funSEXP);
    Rcpp::traits::input_parameter< Rcpp::Environment >::type env(envSEXP);
    RunProcess(fun, env);
    return R_NilValue;
END_RCPP
}
// RunChild
void RunChild(const std::string& fun, const std::string& env);
RcppExport SEXP _processR_RunChild(SEXP funSEXP, SEXP envSEXP) {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const std::string& >::type fun(funSEXP);
    Rcpp::traits::input_parameter< const std::string& >::type env(envSEXP);
    RunChild(fun, env);
    return R_NilValue;
END_RCPP
}
// CallRProcess
Rcpp::Environment CallRProcess(const Rcpp::Function& fun, const Rcpp::Environment& env);
RcppExport SEXP _processR_CallRProcess(SEXP funSEXP, SEXP envSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const Rcpp::Function& >::type fun(funSEXP);
    Rcpp::traits::input_parameter< const Rcpp::Environment& >::type env(envSEXP);
    rcpp_result_gen = Rcpp::wrap(CallRProcess(fun, env));
    return rcpp_result_gen;
END_RCPP
}
// HardwareConcurrency
size_t HardwareConcurrency();
RcppExport SEXP _processR_HardwareConcurrency() {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    rcpp_result_gen = Rcpp::wrap(HardwareConcurrency());
    return rcpp_result_gen;
END_RCPP
}
// CreateProcessPool
Rcpp::List CreateProcessPool(int size);
RcppExport SEXP _processR_CreateProcessPool(SEXP sizeSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< int >::type size(sizeSEXP);
    rcpp_result_gen = Rcpp::wrap(CreateProcessPool(size));
    return rcpp_result_gen;
END_RCPP
}

RcppExport SEXP _rcpp_module_boot_processR();

static const R_CallMethodDef CallEntries[] = {
    {"_processR_copyEnvironment", (DL_FUNC) &_processR_copyEnvironment, 1},
    {"_processR_writeToSharedMemory", (DL_FUNC) &_processR_writeToSharedMemory, 2},
    {"_processR_readEnvironmentFromSharedMemory", (DL_FUNC) &_processR_readEnvironmentFromSharedMemory, 1},
    {"_processR_CreateSharedEnvironment", (DL_FUNC) &_processR_CreateSharedEnvironment, 1},
    {"_processR_CreateSharedEnvironmentNoReturn", (DL_FUNC) &_processR_CreateSharedEnvironmentNoReturn, 1},
    {"_processR_readSharedEnvironment", (DL_FUNC) &_processR_readSharedEnvironment, 1},
    {"_processR_RunProcess", (DL_FUNC) &_processR_RunProcess, 2},
    {"_processR_RunChild", (DL_FUNC) &_processR_RunChild, 2},
    {"_processR_CallRProcess", (DL_FUNC) &_processR_CallRProcess, 2},
    {"_processR_HardwareConcurrency", (DL_FUNC) &_processR_HardwareConcurrency, 0},
    {"_processR_CreateProcessPool", (DL_FUNC) &_processR_CreateProcessPool, 1},
    {"_rcpp_module_boot_processR", (DL_FUNC) &_rcpp_module_boot_processR, 0},
    {NULL, NULL, 0}
};

RcppExport void R_init_processR(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
