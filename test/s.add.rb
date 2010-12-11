#n=10**9/8/4
n = 2*10**6
puts n
m = 200

if ARGV[0]=="old"
  puts "old version"
  require "narray"
  a = NArray.float(n)
  a[] = 3.14
  m.times {
    #a.add!(1)
    a + 1
  }
else
  puts "new version"
  require "./narray.so"
  a = Num::DFloat.new(n)
  a.store(3.14)
  #a = a.inplace
  b=nil
  m.times {
    #a.add!(1)
    b = a.inplace + 1
  }
  a[0..10].debug_print
end

