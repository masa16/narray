from mybench import *

a = arrayrange(ARRSZ)
b = arrayrange(ARRSZ)

print "a.typecode:",a.typecode(),",  a.shape:",a.shape
print "b.typecode:",b.typecode(),",  b.shape:",b.shape
print "calculating c = a+b ..."

def bench_body(a=a,b=b): c=a+b

bench_time(bench_body)
