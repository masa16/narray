require 'mybench'

n = 1000
m = NArray.float(n,n).indgen!
m = m % (n+1) + 1
m = NMatrix.ref(m).transpose

puts 'm ='
p m

printf "executing %ix%i LU...\n",n,n
bench_time(1){lu = m.lu}
exit

puts 'lu='
p lu
puts 'y='
p y
puts 'm*y='
p m*y

#system "ps -v"
