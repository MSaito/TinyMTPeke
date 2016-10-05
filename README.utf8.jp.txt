TinyMTPeke

TinyMTペケ
周期 2^{128}-1

TinyMTX64 64ビット版　これ

TinyMTX32 32ビット版 (64ビット版よりかなり遅いのでボツ）64ビット版の上位または
下位32ビットだけ使用したほうが速いという体たらく。

TinyMT と TinyMTペケの比較
周期
* TinyMTペケの方が少し長い（倍くらい）
均等分布次元
* TinyMTペケの方が低い（悪い）
速度
* TinyMTペケの方が速い（良い） ただし64bitマシン
TestU01 BigCrush
* TinyMTペケの方が良いような気がする。
dynamic create 速度
* TinyMTペケの方が速い
メモリ使用量
* 可変パラメータは一つにした（そしてデフォルト値を設定した）ので、なんとなくメモリ使用量が少ない
ような印象がある。

背景
XORSHIFT* という疑似乱数生成器が速い。
そこで手法をパクってみた。
https://en.wikipedia.org/wiki/Xorshift#xorshift.2A
