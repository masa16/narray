require 'mybench'

n = 500

a = NArray.float(n,n).indgen!
a = a % (n+1) + 1
a = NMatrix.ref(a)#.transpose

b = NArray.float(n,n).indgen!
b = b % (n-1) + 1
b = NMatrix.ref(b)#.transpose

c = 0

puts 'a='
p a
puts 'b='
p b

printf "executing %ix%i Matrix product...\n",n,n
puts 'c=a*b'

bench_time(1) { c=a*b }

# time: 4.21 sec

puts 'c='
p c
