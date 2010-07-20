library(DatABEL);

f <- function(n, type){
d=20;
s=c(1:n);
f=make_empty_fvf(paste("name",toString(n),type),n,n,type,cachesize=1);
print(system.time(for(i in 1:(n/d)){s=s+databel2matrix(f,,i);})*d)
#rm(f);gc();
#unlink("name*");
}

unlink("name*");
f(5000,"CHAR")
f(5000,"SHORT_INT");
f(5000,"INT");
f(5000,"DOUBLE");
f(10000,"CHAR")
f(10000,"SHORT_INT");
f(10000,"INT");
f(10000,"DOUBLE");
f(20000,"CHAR")
f(20000,"SHORT_INT");
f(20000,"INT");
f(20000,"DOUBLE");
f(30000,"CHAR")
f(30000,"SHORT_INT");
f(30000,"INT");
f(30000,"DOUBLE");


