library(processR)
library(Rcpp)
library(RInside)


rnorm_wrapper<-function(){
  set.seed(processR.rank)
  ret<-rnorm(n=100, mean = 50, sd = processR.rank)
  cat(ret)
  return(ret)
}

my_env<-processR::CreateSharedEnvironment()
my_env[["my_list"]]<-list()
# env__ret = processR::CallRProcess(rnorm_wrapper, environment())
# cat(env__ret[["processR.return"]])


#create a pool child processes
pool <- processR::CreateProcessPool()#list()

for (i in 1:length(pool)){#processR::HardwareConcurrency()) {
  pool[[i]]$start(rnorm_wrapper, environment(), i)
  print(pool[[i]]$pid() )
}

results<-list()

print("waiting...")
for (j in 1:length(pool)) {
  pool[[j]]$write_log<-FALSE
  
  #wait for the children to finish
  pool[[j]]$wait()
  
  #print the child output stream
  # cat(pool[[j]]$get_message())
  
  #get the child's environment
  env<-pool[[j]]$get_environment()
  
  #access the process "results" list created by the test function
   print(env[["processR.return"]])
}