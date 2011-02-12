require 'test/unit'
require "complex"
require "narray"
#require 'irb/xmp'

$p = true

class TestBinData < Test::Unit::TestCase

  def px(s)
    puts s
    a = eval(s)
    if a.kind_of? NArray
      a.debug_info
    end
    p a
    puts
    a
  end

  def test_dcomplex
    #GC.disable
    px "$a = NArray::DComplex.cast([5,4.im,3,2.im,1])"
    px "$a = NArray::DComplex.new([5]).seq"
    px "$a.imag = 2; $a"
    px "$a += 1"
    p "1.im = ", 1.im
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
