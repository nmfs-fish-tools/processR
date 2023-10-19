
\name{processR}
\alias{processR}
\title{processR: Multi-processing in R}
\description{
Multi-processing utility for the R programming language.
}
\examples{\donttest{
library(processR)

rnorm_wrapper<-function(){
  set.seed(processR.rank)
  return(rnorm(n=10, mean = 50, sd = processR.rank))
}

 


#create a pool child processes
pool <- processR::CreateProcessPool()

for (i in 1:length(pool)) {
  #pass the entry function, environment, and child rank
  pool[[i]]$start(rnorm_wrapper, environment(), i)
  print(pool[[i]]$pid() )
}

\keyword{optimize}

