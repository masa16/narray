require 'mybench'

a, b = bench_float

print "a = "; p a;
print "b = "; p b;
print "calculating c = a*b ...\n"

bench_time{ c = a*b }
