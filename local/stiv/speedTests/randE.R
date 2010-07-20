library(DatABEL);
n=3000
d=10

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
	r = c(1:l)
	for(i in (1:l)){
		t=ceiling(runif(1,0,l+1-i))
		r[i] = take_nth(v,t)
		v[r[i]] = -1
	}
	return (r);
}	

perform <- function(){
	vx=gen_rand_seq(n);
	vy=gen_rand_seq(n);
	for(x in vx) {
		for(y in vy) {
			f[x,y]=123.456;
		}	
	}
}

unlink("name*");
f=make_empty_fvf("name",n,n,type="DOUBLE",cachesize=10);
system.time(perform())
