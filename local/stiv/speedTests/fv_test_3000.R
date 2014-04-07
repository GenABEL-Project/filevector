library(DatABEL);
n=3000
d=10
unlink("name*");
f=make_empty_fvf("name",n,n,type="DOUBLE",cachesize=10);
system.time(for(i in 1:(n/d)){f[,i]=c(1:n);})*d
system.time(for(i in 1:(n/d)){f[i,]=c(1:n);})*d
s=c(1:n);
system.time(for(i in 1:(n/d)){s=s+databel2matrix(f,,i);})*d
s=c(1:n);
system.time(for(i in 1:(n/d)){s=s+databel2matrix(f,i,);})*d
