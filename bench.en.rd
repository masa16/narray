=begin
= Benchmark

* CPU: Pentium 4 Prescott, 2.8GHz
* OS: Linux 2.6.14-1.1644_FC4

=end
=begin RT
caption = elapsed time (sec)

    , == ,  array add, array mul, matrix dot, matrix solve
    , == , 10^6 elm * 100 times, == , 500x500 elm * 3 tiems, ==
Ruby 1.8.2,   NArray 0.5.8,             1.56,  1.52,  2.10,  6.54
Python 2.4.1, numarray 1.5.0,           5.08,  5.06,  2.10,  3.05
Python 2.4.1, Numeric 23.7-2,           5.58,  5.61,  5.34,  6.15
Octave 2.1.72 (i686-pc-linux-gnu), == , 1.43,  1.39,  2.12,  2.96

=end
=begin
((<back|URL:./>))
=end
