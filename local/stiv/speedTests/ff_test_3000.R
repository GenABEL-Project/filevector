library(ff);
n=3000;
d=20;
x=ff(0.123465,dim=c(n,n))
s=c(1:n);
for(l in 1:n) s[l]=1.12345;
system.time(for(l in 1:(n/d)) {x[l,]=s;})*d
system.time(for(l in 1:(n/d)) {x[,l]=s;})*d
s=c(1:n);
system.time(for(l in 1:(n/d)) {s=s+x[,l]})*d
system.time(for(l in 1:(n/d)) {s=s+x[l,]})*d
