require 'mybench'

a, b = bench_float

print "a = "; p a;
print "calculating c = a+b ...\n"

bench_time{ c = a+b }
