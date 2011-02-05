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

  def test_bit
    #GC.disable
    px "$a = Num::Bit.new([2,5]); $a.store(0); $a"
    px "$b = Num::Bit.cast([[1,1,1,0,1],[0,1,0,0,1]])"
    px "~$b"
    px "$c = Num::Bit.new([2,3,7]); $c.store(1); $c"
    px "$d = $c['*,1,::2']"
    px "$d.store(0); $d"
    px "$c"
    px "$c[0,0..1,0..4] & $b"
    px "$c[0,0..1,0..4]"
    px "$c[0,0..1,0..4].inplace & $b"
    px "$c[0,0..1,0..4]"
    px "$c"
    px "$c.transpose(2,1,0).copy"
    px "$c.count_true"
    px "$c.count_false"
    px "$c.count_true(0,2)"
    px "$c['+,*,+'].count_false"
    px "$c.where"
    px "$c"
    px "$c.fill(0)"
    px "$c['*,::2,::2'].fill(1)"
    px "$c"
  end


end
