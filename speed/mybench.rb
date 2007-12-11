require 'narray'

REPEAT = 100
ARRSZ = 1_000_0000
T = (RUBY_VERSION<"1.8.0") ? Time : Process

def bench_array(type=Float)
  [ NArray.new(type,ARRSZ).indgen!,
    NArray.new(type,ARRSZ).indgen!  ]
end

def bench_float
  bench_array(Float)
end

def bench_int
  bench_array(Integer)
end

def bench_complex
  bench_array(Complex)
end

def bench_time(n=REPEAT)
  t1 = T.times.utime
   for i in 1..n
     yield
   end
  t2 = T.times.utime 
  puts " Time: %.2f sec\n\n" % [t2-t1]
end
