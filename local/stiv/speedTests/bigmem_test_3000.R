library(bigmemory);
n=3000;
d=250;
x=filebacked.big.matrix(n,n,backingfile="a.dat")

system.time(for(l in 1:n) {x[l,]=c(1:n);})
system.time(for(l in 1:n) {x[,l]=c(1:n);})
s=c(1:n);
system.time(for(l in 1:n) {s=s+x[,l]})
system.time(for(l in 1:n) {s=s+x[l,]})
