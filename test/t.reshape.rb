require 'test/unit'
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
    #GC.disable
    px "$a = Num::DFloat.new([3,5,4]); $a.seq"
    px "$a.reshape(5,4,3)"
    px "$a.reshape(10,6)"
    px "$a[:all,:all,Num.step(1,nil,2)].reshape(10,3)"
  end
end
