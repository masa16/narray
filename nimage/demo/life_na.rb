# NArray demo : life game
# by Masahiro Tanaka <masa@ir.isas.ac.jp>  2001-01-18

require 'narray'
require 'nimage'

class LifeGame
  def initialize(nx,ny)
    @d = NArray.byte(nx,ny)
    w = 50
    @d[nx/2-w,ny/2-w] = NArray.byte(w*2+1,w*2+1).random!(2)
    @step = 0
    @win = NImage.show @d
  end

  def life_step
    sum = 
      @d[0..-3,0..-3] + @d[0..-3,1..-2] + @d[0..-3,2..-1] +
      @d[1..-2,0..-3]                   + @d[1..-2,2..-1] +
      @d[2..-1,0..-3] + @d[2..-1,1..-2] + @d[2..-1,2..-1]
    @d[1,1] =
      (sum==3) | ((sum==2) & @d[1..-2,1..-2])
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

begin
  loop{ a.life_step }
ensure
  printf "step=%i\n",a.step
  print "Hit return key..."
  STDIN.getc
  a.close
end
