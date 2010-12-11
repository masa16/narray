require "benchmark"
require "narray"

n = 5000000
$a = Num::DFloat.new(n)
idx = (0...n).map{|i| i}
$b = $a[idx]

m = 10
puts Benchmark::CAPTION
puts Benchmark.measure{
  m.times do
    $a.rand
  end
}
puts Benchmark.measure{
  m.times do
    $b.rand
  end
}

