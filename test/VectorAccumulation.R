library(processR)

vect <- seq(1, 10000000000, by = 200)  

rn<-rnorm(5)

sum<-0
accumulate<-function(){
  
  id <- processR.rank
  for(i in begin[id]:end[id]){
    sum<<- sum+vect[i]
  }
}



id <- 0#mpi.comm.rank(comm = 0)
ns <- processR::HardwareConcurrency() - 1

nsims <- length(vect)

begin <- rep(0, ns)
end <- rep(0, ns)

#create scenario segments
if (id == 0) {
  segments <- nsims / ns
  print(paste("segments ", segments))
  for (i in 1:ns) {
    if (i < ns) {
      begin[i] <- as.integer((i - 1) * segments+1)
      end[i] <- as.integer(i * segments)
    } else{
      begin[i] <- as.integer((i - 1) * segments+1)
      end[i] <- nsims
    }
  }
  print(begin)
  print(end)
}

start<-Sys.time()

#create a pool of child processes
pool<-list()

start<-Sys.time()
for(i in 1:ns){
  #create a new child process
  pool[[i]] <- new(processR::Process)
  
  #start the process. functionm, environment, rank
  pool[[i]]$start(accumulate, environment(), i)
}

local_sum <-0
for(i in 1:ns){
  pool[[i]]$wait()
  env<-pool[[i]]$get_environment()
  local_sum<-local_sum+env[["sum"]]
}

end_<-Sys.time()
runtime<- (end_ - start)
print(runtime)

print(local_sum)
start2<-Sys.time()
global_sum<-sum(vect)
end_2<-Sys.time()
runtime2<- (end_2 - start2)
print(runtime2)

