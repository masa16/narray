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
    px "$a.pack"
    px "$r = $a.to_a"
    px "$r.pack('d*')"
    px "$a['1::2'].pack"
  end
end
