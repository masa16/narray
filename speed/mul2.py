from mybench import *

a = reshape(arrayrange(1000),(1000,1)).astype(Float64)
b = reshape(arrayrange(1000),(1,1000)).astype(Float64)

print "a.typecode:",a.typecode(),",  a.shape:",a.shape
print "b.typecode:",b.typecode(),",  b.shape:",b.shape
print "calculating  c = a*b ..."

def bench_body(a=a,b=b): c=a*b

bench_time(bench_body)

c = a*b
print "c.shape:", c.shape,"\n"
