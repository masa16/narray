.SUFFIXES: .en.rd .en.html .ja.rd .ja.html .rb $(SUFFIXES)

rd2=ruby myrd2html.rb

.en.rd.en.html:
	$(rd2) --lang=en $(opt) $< > $@

.ja.rd.ja.html:
	$(rd2) --lang=ja $(opt) $< > $@

all: index.en.html \
 index.ja.html \
 demo/mandel.en.html \
 demo/image.en.html \
 demo/smooth.en.html \
 demo/lifegame.en.html \
 quanty/quanty.en.html \
 quanty/quanty.ja.html \
 bench.en.html

index.en.html: opt= --title="Numerical Ruby NArray" --kw="NArray,Ruby"
index.ja.html: opt= --title="Numerical Ruby NArray" --kw="NArray,Ruby"
demo/mandel.en.html: opt= --title="NArray demo: Mandelbrot"
demo/image.en.html: opt= --title="NArray demo: Shrink image"
demo/smooth.en.html: opt= --title="NArray demo: Smooth image"
demo/lifegame.en.html: opt= --title="NArray demo: Life game"
bench.en.html: opt= --title="NArray benchmark" --rt
quanty/quanty.en.html: opt= --title="Quanty: unit-math library"
quanty/quanty.ja.html: opt= --title="Quanty: 単位変換ライブラリ"

clean:
	rm -f *~ */*~ *.en.html *.ja.html */*.en.html */*.ja.html

up:
	sh up.sh
