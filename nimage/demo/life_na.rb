# NArray demo : life game
# by Masahiro Tanaka <masa@ir.isas.ac.jp>  2001-01-18

require 'narray'
require 'nimage'

class NArray
  def magnify(n)
    src = shape.collect{|x| [1,x]}.flatten
    dst = shape.collect{|x| [n,x]}.flatten
    a = NArray.new(typecode,*dst)
    a[*[true]*(rank*2)] = self.reshape(*src)
    a.reshape!(*NArray[*shape]*n)
  end
end

class LifeGame
  def initialize(nx,ny)
    @d = NArray.byte(nx,ny)
    w = 50
    @d[nx/2-w,ny/2-w] = NArray.byte(w*2+1,w*2+1).random!(2)
    @step = 0
    @win = NImage.show @d #.magnify(2)
  end

  def life_step
    sum = 
      @d[0..-3,0..-3] + @d[0..-3,1..-2] + @d[0..-3,2..-1] +
      @d[1..-2,0..-3]                   + @d[1..-2,2..-1] +
      @d[2..-1,0..-3] + @d[2..-1,1..-2] + @d[2..-1,2..-1]
    @d[1,1] =
      (sum.eq 3) | ((sum.eq 2) & @d[1..-2,1..-2])
    @step += 1
    @win.update( (@d*(NImage.ncolors-1)).to_s )
  end

  def close
    @win.close
  end

  attr_reader :step
end

srand(1)
a = LifeGame.new(200,200)

print "Hit return key to start..."
STDIN.getc

begin
  loop{ a.life_step }
ensure
  printf "step=%i\n",a.step
  print "Hit return key..."
  STDIN.getc
  a.close
end
