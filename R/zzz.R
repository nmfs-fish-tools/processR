library(methods)


#setClass("SharedVector", representation("Rcpp_SharedVector"))



setMethod("[", c(x = "Rcpp_SharedVector", i = "numeric"),
          function(x, i) {
            return(x$get(i))
          })

# setMethod("[<-", c(x = "Rcpp_SharedVector", i = "numeric", y = "numeric"),
#           function(x, i, y) {
#             (x$set(i,y))
#           })


          