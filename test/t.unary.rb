require 'test/unit'
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
    px "$a = Num::DFloat.new([3,5]); $a.seq; $a-=4"
    px "$b = Num::DFloat.new(3); $b.seq; $b+=5"
    px "$a/4"
    px "($a/4).round"
    px "($a/4).floor"
    px "($a/4).abs"
    px "$a = Num::DFloat.new([3,5]); $a.store(0); $a"
    px "$a/0"
    px "($a/0).isnan"
  end
end
