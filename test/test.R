
find.package('processR')
library(processR)
library(Rcpp)
library(RInside)
#library(rslurm)
list.files(getwd(), 'fims')

pk<-c(9.65987e-09, 3.05454e-08, 8.69655e-08, 2.22933e-07, 5.14548e-07, 1.06931e-06, 2.00082e-06, 3.37084e-06, 5.11322e-06, 6.98356e-06, 8.58785e-06, 9.50864e-06, 9.47933e-06, 8.5087e-06, 6.87661e-06, 5.00394e-06, 3.2785e-06, 1.93403e-06, 1.02726e-06, 4.91269e-07, 2.11537e-07, 8.20123e-08, 2.86284e-08, 8.99791e-09, 2.54631e-09, 6.48796e-10, 1.48844e-10, 3.07453e-11, 5.7181e-12, 9.57528e-13, 1.4437e-13, 1.95988e-14, 2.39555e-15, 2.63639e-16, 2.61239e-17, 2.33074e-18, 1.87229e-19, 1.35419e-20, 8.81888e-22, 5.17097e-23)

plot(pk)

rnorm_wrapper<-function(){
  set.seed(processR.rank)
  return(rnorm(n=10, mean = 50, sd = processR.rank))
}


processR::writeToSharedMemory("rnorm", serialize(rnorm_wrapper, connection = NULL))
processR::writeToSharedMemory("env", serialize(environment(), connection = NULL))

cmd<-"/usr/local/bin/R --slave --no-save -e \"library(processR) \n processR.rank<-1 \nprocessR::RunChild(\"rnorm\", \"env\")\""
 
proc<-new(processR::Process,  cmd)
proc$wait()
cat(proc$get_message())
env_ret<-readEnvironmentFromSharedMemory("env_ret")
cat(env_ret[["processR.return"]])

#create a pool child processes
pool <- processR::CreateProcessPool()#list()

for (i in 1:length(pool)){#processR::HardwareConcurrency()) {
  #creat a new child
  ###pool[[i]] <- new(processR::Process)
  #pass the entry function, environment, and child rank
  pool[[i]]$start(rnorm_wrapper, environment(), i)
  print(pool[[i]]$pid() )
}

results<-list()

print("waiting...")
for (j in 1:length(pool)) {
  #wait for the children to finish
  pool[[j]]$wait()
  
  #print the child output stream
  cat(pool[[j]]$get_message())
  
  #get the child's environment
  env<-pool[[j]]$get_environment()
  
  #access the process "results" list created by the test function
  #print(env[["processR.return"]])
  results[[j]]<-env[["processR.return"]]
}
print(results)
print("done.")

env<-pool[[1]]$get_environment()
