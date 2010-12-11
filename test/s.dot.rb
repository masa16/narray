require "narray"

T = (RUBY_VERSION<"1.8.0") ? Time : Process

REPEAT = 100
n = 500

a = Num::DFloat.new([n,n]).seq
b = Num::DFloat.new([n,n]).seq

def bench_time(n=REPEAT)
  t1 = T.times.utime
   for i in 1..n
     yield
   end
  t2 = T.times.utime 
  puts " Time: %.2f sec\n\n" % [t2-t1]
end

  def px(a)
    if a.kind_of? Num::NArray # a.class < Num::NArray
      a.debug_info
      a[0..2,0..5].debug_print
    end
    puts
    a
  end

## matrix dot
3.times do
  n = 500
  a = Num::DFloat.new([n,n]).seq
  a = a % (n+1) + 1
  b = Num::DFloat.new([n,n]).seq
  b = b % (n-1) + 1
  c = 0
  puts 'a='; px a
  puts 'b='; px b
  printf "executing %ix%i Matrix product...\n",n,n
  puts 'c=dot_mm(a,b)'
  bench_time(10) { c=Num::Linalg.dot_mm(a,b) }
  puts 'c='; px c
end
