library(DatABEL);

f <- function(n, type){
d=100;
f=make_empty_fvf("name",n,n,type="DOUBLE",cachesize=10);
system.time(for(i in 1:(n/d)){s=s+databel2matrix(f,,i);})*d
rm(f);gc();
unlink("name*");
}


f(3000,"CHAR")
f(3000,"SHORT_INT");
f(3000,"INT");
f(3000,"DOUBLE");
f(5000,"CHAR")
f(5000,"SHORT_INT");
f(5000,"INT");
f(5000,"DOUBLE");
f(10000,"CHAR")
f(10000,"SHORT_INT");
f(10000,"INT");
f(10000,"DOUBLE");
