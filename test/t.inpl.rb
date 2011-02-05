require 'test/unit'
require "complex"
require "./narray.so"
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

  def test_dfloat_sum
    GC.disable
    px "$a = Num::DFloat.new([2,5]).seq"
    px "$b = Num::DFloat.new([2,5]).store(1.5)"
    px "$a.inplace + 1.25 + 3.2"
    px "$a"
    px "2.56 + $a.inplace"
    px "$a"
    px "$a.inplace += 1.1"
    px "$a"
    px "$a.out_of_place"
    px "$a + 1"
    px "$a"
  end
end
