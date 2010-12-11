require 'benchmark'
require 'timeout'

Hoge = Struct.new(:int, :float, :complex)

n=0
Timeout.timeout(1,StandardError) do
  10000000.times do |i|
    n = i
    s = Hoge.new()
    s[:int]=1
    s[:float]=2
    s[:complex]=3
  end
end rescue

puts n


#n = 1000000

b = Benchmark.measure do
  n.times do 
    s = Hoge.new()
    s[:int]=1
    s[:float]=2
    s[:complex]=3
  end
end

puts b

b = Benchmark.measure do
  n.times do 
    s = Hoge.new()
    s[0]=1
    s[1]=2
    s[2]=3
  end
end

puts b

b = Benchmark.measure do
  n.times do 
    s = {}
    s[:int]=1
    s[:float]=2
    s[:complex]=3
  end
end

puts b

b = Benchmark.measure do
  n.times do 
    s = []
    s[0]=1
    s[1]=2
    s[2]=3
  end
end

puts b

b = Benchmark.measure do
  n.times do 
    s = Hoge.new(1,2,3)
    s[:int]
  end
end

puts b

b = Benchmark.measure do
  n.times do 
    s = Array.new([1,2,3])
    s[0]
  end
end

puts b

b = Benchmark.measure do
  n.times do 
    s = {:int=>1, :float=>2, :complex=>3}
    s[:int]
  end
end
puts b

