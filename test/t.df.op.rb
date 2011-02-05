require 'test/unit'
#require "complex"
require "./narray.so"
#require 'irb/xmp'

$p = true

class TestBinData < Test::Unit::TestCase

  def px(s)
    puts s
    p a = eval(s)
    if a.kind_of? NArray # a.class < Num::NArray
      a.debug_info
      p a
    end
    puts
    a
  end

  def test_dfloat_op
    #GC.disable
    px "$a = NArray::DFloat.new([5]).seq"
    px "$a + 1"
    px "$a * 2"
    px "$a - 4.5"
    px "$a / 3"
    px "$a % 3"
    px "$a ** 2"
    px "$a ** 4"
    px "$a ** 10"
    px "$a"
    px "($a.inplace*2 + 1.5)**2"
    px "$a"
  end
end
