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

  def test_dfloat_op
    #GC.disable
    px "$a = Num::DFloat.new([5]).seq"
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
