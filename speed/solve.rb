require 'mybench'

n = 500
m = NArray.float(n,n).indgen!
m = m % (n+1) + 1
m = NMatrix.ref(m).transpose
x = NMatrix.float(n,n).indgen!(1).transpose
#x = NVector.float(n).indgen!(1)
y = 0

puts 'm ='
p m
puts 'x ='
p x

printf "solving  y=x/m ...\n",n,n
bench_time(1){ y = x/m }
#  Time: 5.87 sec

puts 'y='
p y
