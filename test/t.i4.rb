require 'test/unit'
#require "complex"
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

  def test_dfloat_sum
    #GC.disable
    px "$a = Num::Int32.new([5])"
    px "$a = Num::Int32.cast([1,3,7,13])"
    px "$a[2]"
    px "$a[2] = 123; $a"
    px "Num::DFloat.cast($a)"
  end
end
