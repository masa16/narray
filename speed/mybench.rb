require 'narray'

REPEAT = 100
ARRSZ = 1_000_000

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
  t1 = Time.times.utime
   for i in 1..n
     yield
   end
  t2 = Time.times.utime 
  puts " Time: %.2f sec\n\n" % [t2-t1]
end
