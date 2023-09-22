library(processR)
library(Rcpp)

#get the Rcpp module
p <- Rcpp::Module(module = "processR", PACKAGE = "processR")





v <- 10


test <- function() {
  for (i in 1:200000000) {
    v <- v + 1
  }
  
  for (i in 1:2) {
    print(paste("rank",paste(processR.rank, " -> hi from test")))
  }
  
}

ee <- environment()
pool <- list()
for (i in 1:(processR::HardwareConcurency())) {

  ee <- environment()
  print(paste("initializing child with rank ",i))

  rank <- i
  pool[[i]] <- new(p$Process)
  pool[[i]]$start(test, ee, rank)
}

print("waiting...")

for (j in 1:10) {
  pool[[j]]$wait()
  pool[[j]]$print()
}
