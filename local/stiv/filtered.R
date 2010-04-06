library("RUnit")
library("DatABEL")
source("/home/stiv/genabel/pkg/DatABEL/inst/unitTests/shared_functions.R")
cat ("Running test.databel_filtered_R_class\n");
unlink("tmp*.fv?")
unlink("test*.fv?")
dta <- make_random_matrix()
null <- as(dta,"databel_base_R")
checkNumEq(dta,null)
checkIdentical("databel_base_R",class(null)[1])
x <- databel_filtered_R(null)
checkIdentical("databel_filtered_R",class(x)[1])
checkNumEq(dta,x)
x1 <- as(x,"databel_base_R")
checkIdentical(as(x,"matrix"),as(null,"matrix"))
#---------------------------1---------------------
checkNumEq(dta,x1)
disconnect(x)
checkNumEq(dta,null)
connect(x)
checkIdentical("databel_filtered_R",class(x)[1])
cat("1.2");
checkNumEq(dta,x)
#---------------------------2---------------------
rm(null);gc()
x
rm(x,x1);gc()
x <- as(dta,"databel_filtered_R")
#---------------------------2.1---------------------
checkIdentical("databel_filtered_R",class(x)[1])
checkNumEq(dta,x)
disconnect(x)
connect(x)
#---------------------------3---------------------
checkIdentical("databel_filtered_R",class(x)[1])
checkNumEq(dta,x)
x1 <- x
checkNumEq(dta,x1)

# submatrix
    rCol <- 1:3#sample(1:ncol(x),ceiling(ncol(x)/10))
    rCol
    rRow <- 1:3#sample(1:nrow(x),ceiling(nrow(x)/10))
    rRow

#---------------------------4---------------------
dta1 <- dta[rRow,rCol]
x1 <- x[rRow,rCol]
checkNumEq(dta1,x1)
cat("4.1")
disconnect(x1)
connect(x1)
cat("4.2")
checkNumEq(dta1,x1)
#-------------------------5-----------------------

dta1 <- dta[sort(rRow),sort(rCol)]
x1 <- x[sort(rRow),sort(rCol)]
checkNumEq(dta1,x1)

rCol <- round(runif(floor(ncol(x1)/2),min=1,max=ncol(x1)))
rCol
rRow <- round(runif(floor(nrow(x1)/2),min=1,max=nrow(x1)))
rRow

dta2 <- dta1[rRow,rCol]
x2 <- x1[rRow,rCol]
checkNumEq(dta2,x2)


logCol <- (runif(ncol(dta))<=0.1)
while (!any(logCol)) logCol <- (runif(ncol(dta))<=0.1)
logRow <- (runif(nrow(dta))<=0.1)
while (!any(logRow)) logRow <- (runif(nrow(dta))<=0.1)
table(logCol)
table(logRow)

dta1 <- dta[logRow,logCol]
x1 <- x[logRow,logCol]
checkIdentical(dta1,as(x1,"matrix"))

logCol <- (runif(ncol(dta1))<=0.1)
while (!any(logCol)) logCol <- (runif(ncol(dta1))<=0.1)
logRow <- (runif(nrow(dta1))<=0.1)
while (!any(logRow)) logRow <- (runif(nrow(dta1))<=0.1)
table(logCol)
table(logRow)

dta2 <- dta1[logRow,logCol]
x2 <- x1[logRow,logCol]
checkIdentical(dta2,as(x2,"matrix"))

dta1 <- dta[c(T,F,F,F,F),c(F,F,F,F,T)]
dim(x)
x1 <- x[c(T,F,F,F,F),c(F,F,F,F,T)]
checkIdentical(dta1,as(x1,"matrix"))

# submatrix and permute
# on integer
# on logical
# on names

apply(dta1,FUN=sum,MAR=1)
#apply(x1,FUN=sum,MAR=1)

dn <- dimnames(x)
ndn <- dn
ndn[[1]] <- sample(dn[[1]],10)
ndn[[2]] <- sample(dn[[2]],5)
ndn
dta1 <- dta[ndn[[1]],ndn[[2]]]
x1 <- x[ndn[[1]],ndn[[2]]]
dta1
x1
checkIdentical(dta1,as(x1,"matrix"))

unlink("tmp*.fv?")
unlink("test*.fv?")

