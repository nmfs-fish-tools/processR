library(processR)
library(Rcpp)
library(RInside)

#get the Rcpp module
p <- Rcpp::Module(module = "processR", PACKAGE = "processR")





v <- 10



test <- function() {
  results<<-list()
  for (i in 1:2000) {
    v <- v + 1
    results[[i]]<<-i
    
  }
  
  
  
  for (i in 1:2) {
    print(paste("rank",paste(processR.rank, " -> hi from test")))
  }
  
}

ee <- environment()
pool <- list()
for (i in 1:processR::HardwareConcurrency()) {
  pool[[i]] <- new(p$Process)
  pool[[i]]$start(test, ee, i)
}

print("waiting...")

for (j in 1:10) {
  pool[[j]]$wait()
  pool[[j]]$print()
  length((pool[[j]]$get_environment()))
}

 #View(pool[[1]]$get_environment())

