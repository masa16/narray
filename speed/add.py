from mybench import *

a = bench_array()
b = bench_array()

print "a.typecode:",a.typecode(),",  a.shape:",a.shape
print "b.typecode:",b.typecode(),",  b.shape:",b.shape
print "calculating c = a+b ..."

def bench_body(a=a,b=b): c=a+b

bench_time(bench_body)
