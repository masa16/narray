require 'test/unit'
require "complex"
require "narray"
#require 'irb/xmp'

$p = true

class TestBinData < Test::Unit::TestCase

  def px(s)
    puts s
    p a = eval(s)
    if a.kind_of? Num::NArray # a.class < Num::NArray
      a.debug_info
      a.debug_print
    end
    puts
    a
  end

  def test_dcomplex
    #GC.disable
    px "$a = Num::DComplex.cast([5,4.im,3,2.im,1])"
    px "$a = Num::DComplex.new([5]).seq"
    px "$a.imag = 2; $a"
    px "$a += 1.im"
    px "$a * 2"
    px "$a - 4.5"
    px "$a / 3"
    px "$a.inverse"
    px "$a.abs"
    px "$a.conj"
    px "$a.real"
    px "$a.imag"
    px "$a.arg"
    px "$a.real = 1.5; $a"
    px "$a.im"
    px "$a.inplace.im; $a"
    px "$a ** 2.0"
    px "$a ** 2"
    px "$a ** 4"
    px "$a ** 10"
    px "$a"
    px "($a.inplace*2 + 1.5)**2"
    px "$a"
  end
end
