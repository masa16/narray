=begin
= Numerical Ruby NArray

((<English|URL:index.html>)) / ((<Japanese|URL:index.ja.html>))

== NArray 開発版

((<GitHub|URL:https://github.com/masa16/narray-devel>))

開発中の次期 NArray です。未完成です。
旧版とは非互換ですが、大幅な機能向上が行われています。
ご意見をお待ちしております。

* 次期NArrayについての説明がある((<スライド|URL:http://www.slideshare.net/masa16tanaka/narray-pwrake>))

== NArray 旧バージョン

((<GitHub|URL:https://github.com/masa16/narray>))

多次元 数値 配列クラスです。 1,2,4 byte 整数、単/倍 精度 実数/複素数、 およびRubyオブジェクトを要素に持つことができます。 これにより Ruby でも 大量の数値を扱う計算が、簡単かつ高速にできるようになります。

今後旧版に新機能は加えられません。上記の新版に移行する予定です。

* ドキュメント
  * ((<NArray 特徴|URL:feature-ja.html>))
  * ((<NArray メソッド一覧|URL:SPEC.ja.txt>))
  * ((<大林さんによるNArrayの解説|URL:http://www.kmc.gr.jp/~ohai/narray/refm.html>))
  * ((<princelab's NArray documents|URL:https://github.com/princelab/narray/wiki>))
    * Tentative NArray Tutorial, etc.

* 使用例:
  * ((<マンデルブロ|URL:demo/mandel.en.html>))
  * ((<画像のリサイズ|URL:demo/image.en.html>))
  * ((<画像のスムージング(FFTW使用)|URL:demo/smooth.en.html>))
  * ((<ライフゲーム|URL:demo/lifegame.en.html>))
  * ((<NMatrix & NVector - LU分解により線形方程式を解く|URL:matrix-ja.html>))

* インストール
  * ソースコード: バージョン: _narray_version_ (((<.tar.gz|URL:https://github.com/masa16/narray/archive/_narray_version_.tar.gz>))|((<.zip|URL:https://github.com/masa16/narray/archive/_narray_version_.zip>))) (_narray_release_date_)
  * Gem インストール（要コンパイラ）:
 gem install narray

== NArrayの拡張

=== FFTW3

((<FFTW|URL:http://www.fftw.org/>))
(高速 DFT (Discrete Fourier Transform) ライブラリ) ver.3 の NArray インタフェース

* ソース:
  ((<ver 0.1.0|URL:http://rubyforge.org/frs/download.php/7661/ruby-fftw3-0.1.0.tar.gz>))
  (リンク切れ) (2003-01-04)

=== na_geev

((<Lapack|URL:http://www.netlib.org/lapack/>))
dgeev, zgeev ルーチン の NArray インタフェース。
固有値・固有ベクトルメソッドを NMatrix に追加。

* ソース:
  ((<ver 0.1.2|URL:http://rubyforge.org/frs/download.php/7660/na_geev-0.1.2.tar.gz>))
  (リンク切れ) (2004-03-01)

=== NArrayを利用しているプロジェクト

* ((<電脳Rubyプロジェクト|URL:http://ruby.gfd-dennou.org/index-j.htm>)) (RubyDCL, RubyNetCDF, ...)
* ((<"Ruby/GSL"|URL:http://rb-gsl.rubyforge.org/>))
* ((<GridFlow|URL:http://gridflow.ca/>))
* ((<"ruby/audio"|URL:http://hans.fugal.net/src/ruby-audio/>))
* ((<"Ruby/PGPLOT"|URL:http://pgplot.rubyforge.org/>))

== Other Tips

* ((<Ruby Multi-dimensional Arrays|URL:mdary.html>))

== Link

* ((<Ruby for Science @ Rwiki|URL:ruby4science.ja.html>)) - 2000年代の古い情報
* ((<SciRuby Wiki|URL:http://sciruby.codeforpeople.com/sr.cgi/FrontPage>))

== Author

Masahiro Tanaka

=end
