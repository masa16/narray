require 'test/unit'
require "./narray.so"
#require 'irb/xmp'

$p = true

class TestBinData < Test::Unit::TestCase

  def px(s)
    puts "-----------------------------------------------"
    puts s
    p a = eval(s)
    if a.kind_of? NArray # a.class < NArray::NArray
      a.debug_info
      #a.debug_print
      p a
    end
    puts
    a
  end

  def test_bit
    #GC.disable
    px "$a = NArray::Bit.new([2,5]); $a.fill(0); $a"
    px "$b = NArray::Bit.cast([[1,1,1,0,1],[0,1,0,0,1]])"
    px "~$b"
    px "$c = NArray::Bit.new([2,3,7]); $c.store(1); $c"
    px "$d = $c['*,1,::2']"
    px "$d.store(0); $d"
    px "$c"
    px "$f = $c[0,0..1,0..4]"
    px "$c[0,0..1,0..4] & $b"
    px "$c[0,0..1,0..4]"
    px "$c[0,0..1,0..4].inplace & $b"
    px "$c[0,0..1,0..4]"
    px "$c"
    #px "$c.transpose(2,1,0).copy"
    px "$c.count_true"
    px "$c.count_false"
    px "$c.count_true(0,2)"
    px "$g = $c['+,*,+']"
    px "$g.count_false"
    #px "$c['+,*,+'].count_false"
    px "$c.where"
    px "$c"
    px "$c.fill(0)"
    px "$c['*,::2,::2'].fill(1)"
    px "$c"
  end


end
