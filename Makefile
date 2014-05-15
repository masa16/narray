.SUFFIXES: .en.rd .html.en .ja.rd .html.ja .rb $(SUFFIXES)

rd2=ruby myrd2html.rb

.en.rd.html.en:
	$(rd2) --lang=en $(opt) $< > $@

.ja.rd.html.ja:
	$(rd2) --lang=ja $(opt) $< > $@

all: index.html.en \
 index.html.ja \
 demo/mandel.html.en \
 demo/image.html.en \
 demo/smooth.html.en \
 demo/lifegame.html.en \
 quanty/quanty.html.en \
 quanty/quanty.html.ja \
 bench.html.en

index.html.en: opt= --title="Numerical Ruby NArray" --kw="NArray,Ruby"
index.html.ja: opt= --title="Numerical Ruby NArray" --kw="NArray,Ruby"
demo/mandel.html.en: opt= --title="NArray demo: Mandelbrot"
demo/image.html.en: opt= --title="NArray demo: Shrink image"
demo/smooth.html.en: opt= --title="NArray demo: Smooth image"
demo/lifegame.html.en: opt= --title="NArray demo: Life game"
bench.html.en: opt= --title="NArray benchmark" --rt
quanty/quanty.html.en: opt= --title="Quanty: unit-math library"
quanty/quanty.html.ja: opt= --title="Quanty: 単位変換ライブラリ"

clean:
	rm -f *~ */*~ *.html.en *.html.ja */*.html.en */*.html.ja

up:
	sh up.sh
