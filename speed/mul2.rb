require 'mybench'

a = NArray.float(1000,1).indgen!
b = NArray.float(1,1000).indgen!
c = 0

print "a = "; p a;
print "b = "; p b;
print "calculating c = a*b ...\n"

bench_time{ c = a*b }

print "c = "; p c;
