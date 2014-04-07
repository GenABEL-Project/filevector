library(DatABEL);
unlink("name*");
n=3000;
d=20;
x=make_empty_fvf("name",n,n,type="DOUBLE",cachesize=10);

take_nth <- function(v, n){
	r<-0; p = 0;
	for(i in (1:(length(v)))){
		if (v[i] > 0 ) {
			p=p+1; 
		}
		if (p==n){
			r<-v[i];
			break;
		}
	}
	return (r);
}

gen_rand_seq <- function(l){
	v = c(1:l)
	r = c(1:(l/d))
	for(i in (1:(l/d))){
		t=ceiling(runif(1,0,l+1-i))
		r[i] = take_nth(v,t)
		v[r[i]] = -1
	}
	return (r);
}	

perform1 <- function(){
	vx=gen_rand_seq(n);
	for(xx in vx) {
		x[xx,]=c(1:n);
	}
}

perform2 <- function(){
	vx=gen_rand_seq(n);
	for(xx in vx) {
		x[,xx]=c(1:n);
	}
}

s = c(1:n)

perform3 <- function(){
	vx=gen_rand_seq(n);
	for(xx in vx) {
		s=s+databel2matrix(x,xx);
	}
}

perform4 <- function(){
	vx=gen_rand_seq(n);
	for(xx in vx) {
		s=s+databel2matrix(x,,xx);
	}
}

system.time(perform1())*d
system.time(perform2())*d
system.time(perform3())*d
system.time(perform4())*d


