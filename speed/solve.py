from mybench import *
from LinearAlgebra import *

n = 500

a = reshape(arrayrange(n*n)%(n+1)+1,(n,n)).astype(Float64)

b = reshape(arrayrange(n*n)+1,(n,n)).astype(Float64)
#b = arrayrange(n).astype(Float64)+1

print "LU factorize & solve %ix%i matrix ..."%(n,n)

def bench_body(a=a,b=b): c=solve_linear_equations(a,b)
bench_time(bench_body,1)

# Time:   8.120 sec
