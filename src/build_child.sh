clang++ -arch x86_64 -std=gnu++11 
-I"/Library/Frameworks/R.framework/Resources/include" -DNDEBUG  
-I'/Library/Frameworks/R.framework/Versions/4.3-x86_64/Resources/library/Rcpp/include' 
-I/opt/R/x86_64/include   -w  -lR 
-L/Library/Frameworks/R.framework/Resources/lib -L/opt/R/x86_64/lib  
RRunner.cpp -o RRunner.x
