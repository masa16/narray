require "benchmark"
require "narray"

Num::NArray.inspect_cols = 200

a = Num::DFloat.new(20).rand
a[5]=0.0/0.0
a[2]=1.0/0.0
p a
p a.sort
#p a.format("%.2f").to_a
#p a.sort.format("%.2f").to_a



n = 5000000
$a = Num::DFloat.new(n).rand
#idx = (0...n).map{|i| i}
#$b = $a[idx]

m = 1
puts Benchmark::CAPTION
puts Benchmark.measure{
  m.times do
    $r = $a.sort
  end
}
p $r
exit

puts Benchmark.measure{
  m.times do
    $b.rand
  end
}

