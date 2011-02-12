require "narray"


klasses = [NArray::DFloat, NArray::Int32, NArray::Int16, NArray::Int8]

klasses.each do |klass|
  describe klass do
    array = [1,2,3]

    describe ".cast(#{array})" do
      before do
        @narray = klass.cast(array)
      end

      it "== #{array} " do
        @narray.should == array
      end

      it "kind_of #{klass}" do
        @narray.should be_kind_of klass
      end

      it ".to_a == #{array} " do
        @narray.to_a.should == array
      end

      after do
        @narray = nil
      end
    end
  end
end


describe NArray::Int32 do
  array = [1.5, 2.5, 3.5]
  describe ".cast(#{array})" do
    before do
      @narray = NArray::Int32.cast(array)
    end

    it "not #{array}" do
      @narray.should_not == array
    end

    after do
      @narray = nil
    end
  end
end


klasses.each do |klass|
  describe klass do

    result = [0,1,2,3,4]

    describe ".new([#{result.size}]).seq" do

      before do
        @narray = klass.new([result.size]).seq
      end

      it "kind_of #{klass}" do
        @narray.should be_kind_of klass
      end

      it "== #{result} " do
        @narray == result
      end

      after do
        @narray = nil
      end

    end
  end
end


klasses.each do |klass|
  describe klass do

    result = [[0,1],[2,3],[4,5]]

    describe ".new([3,2]).seq" do

      before do
        @narray = klass.new([3,2]).seq
      end

      it "== #{result} " do
        @narray == result
      end

      after do
        @narray = nil
      end

    end
  end
end


klasses = [NArray::DFloat, NArray::Int32, NArray::Bit]

klasses.each do |klass|
  describe klass do

    [[7], [7,8], [7,8,9]].each do |shape|

      describe ".new(#{shape})" do
        ndim = shape.size
        total = shape.inject(1){|x,y| x*y}

        before do
          @narray = klass.new(shape)
        end

        it "is kind_of #{klass}" do
          @narray.should be_kind_of klass
        end

        it ".fill(1) is kind_of #{klass}" do
          @narray.fill(1).should be_kind_of klass
        end

        it ".ndim is #{ndim}" do
          @narray.ndim.should == ndim
        end

        it ".shape is #{shape}" do
          @narray.shape.should == shape
        end

        it ".size is #{total}" do
          @narray.size.should == total
        end

        after do
          @narray = nil
        end
      end
    end
  end
end


[NArray::DFloat, NArray::Int32].each do |klass|
  describe klass do

    [[7], [7,8], [7,8,9]].each do |shape|

      describe ".new(#{shape})" do
        ndim = shape.size
        total = shape.inject(1){|x,y| x*y}

        before do
          @narray = klass.new(shape)
        end

        it ".seq is kind_of #{klass}" do
          @narray.seq.should be_kind_of klass
        end

        after do
          @narray = nil
        end
      end
    end

    describe ".new([4,1]).seq ** [0,1,2,3]" do
      result = [[1, 0, 0, 0],
                [1, 1, 1, 1],
                [1, 2, 4, 8],
                [1, 3, 9, 27]]
      before do
        @narray = klass.new([4,1]).seq ** [0,1,2,3]
      end

      it "is kind_of #{klass}" do
        @narray.should be_kind_of klass
      end

      it "== #{result} " do
        @narray.should == result
      end

      after do
        @narray = nil
      end
    end
  end
end

[NArray::Int32, NArray::DFloat].each do |klass|
  describe klass do

    pow = [0, 1.5, 2.5, 10]
    describe ".new([4,1]).seq ** #{pow}" do
      result = [[1, 0, 0, 0],
                [1, 1, 1, 1],
                [1, 2.8284271247461903, 5.196152422706632, 1024],
                [1, 5.656854249492381, 15.588457268119896, 59049]]

      before do
        @narray = klass.new([4,1]).seq ** pow
      end

      it "is kind_of #{klass}" do
        @narray.should be_kind_of klass
      end

      it "== #{result} " do
        @narray.should == result
      end

      after do
        @narray = nil
      end
    end
  end
end
