# -*- coding: utf-8 -*-
= Ruby for Scientific Computing

Rubyの科学計算における応用例の紹介です。

このページは、((<RWiki:Ruby for Science|URL:https://web.archive.org/web/20101221083022/http://pub.cozmixng.org/~the-rwiki/rw-cgi.rb?cmd=view;name=Ruby+for+Science>)) に記載した古い情報の転載です。
最初のページ作成は2000年頃、最終更新は2010年12月15日です。
それ以降に登場した情報は掲載していません。
リンクの変更先がわかるものについては直しています。

== Ruby科学計算関連総合
* ((<SciRuby|URL:http://sciruby.codeforpeople.com/sr.cgi/FrontPage>))
  - Since June 2005, by Ara T. Howard.
* ((<るびま0006 Ruby Library Report [第5回] 数値計算と可視化|URL:http://jp.rubyist.net/magazine/?0006-RLR>))
  - 著者: 堀之内武さん

== 各分野での利用
=== Mathmatics - 数学
==== Algebra - 計算代数
* ((<Polynominal|URL:http://www.math.kobe-u.ac.jp/HOME/kodama/tips-RubyPoly.html>))
  - by 児玉さん
* ((<Algebra|URL:http://blade.nagaokaut.ac.jp/~sinara/ruby/math/>))
  - by 原さん
  - 多変数多項式環のグレブナ−基底の実装。
* ((<Ruby Mathematica|URL:http://madscientist.jp/~ikegami/ruby/mathematica/index-j.html>))（リンク切れ）
  - MathematicaへのRubyインタフェース。

* ((<「プログラミングのための線形代数」|URL:http://wiki.fdiary.net/lacs/>)) - 平岡和幸著

=== Earth Science - 地球科学・気象学
* ((<電脳Ruby|URL:http://ruby.gfd-dennou.org/>))
  by ((<地球流体電脳倶楽部|URL:http://www.gfd-dennou.org/>))
  - 多次元データの解析と可視化のための研究開発活動。
  * ((<Ruby DCL|URL:http://www.gfd-dennou.org/arch/ruby/products/ruby-dcl/>))
    - 描画ライブラリー ((<DCL|URL:http://www.gfd-dennou.org/arch/dcl/>))(Dennou Club Library)のRuby用ラッパー
  * ((<RubyNetCDF|URL:https://www.gfd-dennou.org/arch/ruby/products/ruby-netcdf/>))
  * ((<GAVE|URL:http://www.gfd-dennou.org/arch/ruby/products/gave/index-j.html>))
    - Rubyで書かれた解析およびビューワー
  * ((<GPhys|URL:http://www.gfd-dennou.org/arch/ruby/products/gphys/>))
    - 多次元物理データ取り扱いライブラリー

=== Biology - 生物学
* ((<BioRuby|URL:http://bioruby.org/>))
  - ((<Bioinformatics|URL:http://www.jsbi.org/>)) に必要な機能や環境を、
  Ruby を用いて統合的に実装することを目標にしたプロジェクト。
* ((<Ruby-Helix|URL:http://structure.m.u-tokyo.ac.jp/English/software/Ruby-Helix-Page/ruby-helix.html>)) クライオ電子顕微鏡の写真から、高解像度の三次元構造を再構成するためのソフト。NArrayを用いている。

=== Chemistry - 化学
* Chemruby - BioRuby みたいなことを化学でもやりたい。((<15年度未踏|URL:http://www.ipa.go.jp/jinzai/esp/2005mito1/gaiyou/10-26.html>))で開発された？


=== Physics - 物理
* ((<とんでる力学|URL:http://jun.artcompsci.org/rikigaku/>))
  - ((<パリティ|URL:http://pub.maruzen.co.jp/book_magazine/magazine/pub-zassi-j.html>))連載
  * ((<単行本|asin:4621076094>))(丸善 parity books)

=== Astronomy - 天文
* ((<The Kali Code for Dense Stellar Systems|URL:http://www.artcompsci.org/kali/index.html>))
* ((<ruby-sphere|URL:http://ruby-sphere.sourceforge.net/>)) to calculate apparent positions of stars with a moderate accuracy

=== Materials Science - 材料科学
* ((<Virtual sample library|URL:http://www.geocities.jp/tokyo_1406/node2.html#link:vsl>))

== ツール・ライブラリ

* ((<"RubyForge: Scientific/Engineering"|URL:http://rubyforge.org/softwaremap/trove_list.php?form_cat=97>))（リンク切れ）

=== アルゴリズム等
* ((<"Ruby/GSL"|URL:http://rb-gsl.rubyforge.org/>))
  - ((<GNU Scientific Library|URL:http://www.gnu.org/software/gsl/>)) のラッパー
* ((<"Ruby-SSL2"|URL:http://www.gfd-dennou.org/arch/ruby/products/ruby-ssl2/index-j.html>))
  - ((<SSLII|URL:http://software.fujitsu.com/jp/fortranc/ssl2.html>))（リンク切れ） のラッパー
* ((<Math::Statistics|URL:http://raa.ruby-lang.org/list.rhtml?name=math-statistics>))（リンク切れ）
  - 基本統計メソッドのMix-in。
* ((<ruby-cluster|URL:http://bioruby.org/contrib/ruby-cluster/>))
  - 階層型クラスタリング計算。最長距離法・最短距離法・平均距離法。
* ((<ComputationGraph|URL:http://www.tinyforest.gr.jp/memo/ruby/readme.html>))
  - Rubyグラフ処理エンジン。
* ((<Ruby GP|URL:http://raa.ruby-lang.org/project/ruby-gp/>))（リンク切れ）
  - 遺伝的アルゴリズム・ライブラリ。
* ((<Ruby@RjpWiki|URL:http://www.okada.jp.org/RWiki/index.php?Ruby>))（リンク切れ）
  - Rubyからの統計解析システムRの利用

=== 数値配列
* ((<NArray|URL:http://narray.rubyforge.org/>))
  - 多次元数値配列。FFTWのwrapper、Vector, Matrixの配列。
* ((<FortranArray|URL:http://www-lab.imr.tohoku.ac.jp/~t-nissie/computer/software/FortranArray/>))
  - 多次元数値配列。BLAS, LAPACKへの自然なインタフェース。
* ((<linalg|URL:http://linalg.rubyforge.org/>))
  - DMatrix。LAPACKへのインタフェース。
* ((<NumArray|URL:http://www.kurasc.kyoto-u.ac.jp/radar-group/members/kawanabe/numarray.html>))（リンク切れ）
  - 多次元数値配列。欠損値処理。

=== 数値クラス
* ((<Ruby-GMP|URL:http://raa.ruby-lang.org/project/libgmp-ruby/>))（リンク切れ）
  - ((<GMP|URL:http://www.swox.com/gmp/>))のラッパー。可変長 整数、有理数、浮動小数点数。
* ((<BigFloat|URL:http://www.tinyforest.gr.jp/ruby/bigfloat.html>))（リンク切れ）
  - 可変長浮動少数点クラスの拡張ライブラリ - Ruby 1.8 には BigDecimal として標準添付
* ((<Quanty|URL:http://narray.rubyforge.org/#quanty>))
  - 単位つき量のクラス。元の単位は保持される。
* ((<NumRu::Units|URL:http://ruby.gfd-dennou.org/products/numru-units/>))
  - 物理量を扱うクラス。((<UDUNITS Library|URL:http://www.unidata.ucar.edu/packages/udunits/>))のほとんどの機能をカバー。単位は常にMKSAに変換される。

=== データ解析
* ((<Ruby-CERNLIB|URL:http://www-ps.kek.jp/thitoshi/ruby/cern/index-ja.html>))（リンク切れ）
  - CERN Program Library のラッパー
* ((<ruby-root|URL:http://www.csd.uoc.gr/~elathan/rr/>))
  - Object-Oriented Data Analysis Framework ((<Root|URL:http://root.cern.ch/>)) のラッパー
* ((<RubyROOT|URL:http://cern.ch/alcaraz/RubyRoot/>))
  - ((<Root|URL:http://root.cern.ch/>)) のSWIGラッパー

=== プロット・可視化
* ((<Gnuplot module for Ruby (rgplot)|URL:http://rgplot.rubyforge.org/>)) ((<[project page]|URL:http://rubyforge.org/projects/rgplot>)) gnuplot への interface. ((<Gordon James Miller 氏|URL:http://rubyforge.org/users/gmiller/>))作. ((<Ruby Gnuplot|URL:http://rgnuplot.sourceforge.net/>)) は以前の version.
* ((<Ruby libplot,libgraph,GDGraph,etc.|URL:http://www2s.biglobe.ne.jp/~Nori/ruby/ja/index.html>))
* ((<"Ruby/PGPLOT"|URL:http://pgplot.rubyforge.org/>))
* ((<GD::Graph|URL:http://raa.ruby-lang.org/project/gd-graph/>))（リンク切れ）
* ((<RAA:gdchart>))（リンク切れ） - ((<GDChart|URL:http://www.fred.net/brv/chart/>))のRubyバインディング
* ((<Ruby DCL|URL:http://www.gfd-dennou.org/arch/ruby/products/ruby-dcl/>))
  - 描画ライブラリー ((<DCL|URL:http://www.gfd-dennou.org/arch/dcl/>))のRuby用ラッパー
* ((<Ploticus from Ruby|URL:http://www.ntecs.de/blog/Blog/RubyPloticus.rdoc>))（リンク切れ） - ((<Ploticus|URL:http://ploticus.sourceforge.net/>))のラッパー
* ((<MRPlot|URL:http://www.harderweb.de/harderware/index.php?l=en&p=/mrplot>))（リンク切れ） - pure ruby plotting library
* ((<Tioga|URL:https://rubygems.org/gems/tioga/>)) - plots using Ruby, PDF, and TeX

=== 並列計算
* ((<MPI Ruby|URL:http://www-unix.mcs.anl.gov/mpi/mpi_ruby/>))
  - 並列処理のためのメッセージ通信インターフェース MPI の Rubyバインディング。

=== 計測・制御
* ((<Ruby for research|URL:http://yueda.issp.u-tokyo.ac.jp/weda/ruby/ruby.html>)) - gpib.rb (for GPIB)
  * ((<gpib-rb|URL:http://www.ruby-lang.org/en/raa-list.rhtml?name=gpib-rb>))（リンク切れ）
* ((<IO Port Access|URL:http://raa.ruby-lang.org/project/i-o_port_access/>))（リンク切れ）
* ((<HornetsEye|URL:http://www.wedesoft.de/hornetseye-api/>)) is a Ruby real-time computer vision extension.

=== 電卓

* ((<rcalc: Scientific RPN command line calculator|URL:http://rcalc.rubyforge.org/>))
